#include <config.h>

#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>

#include "uniquemessage.h"
#include "uniqueinternals.h"

/**
 * SECTION:unique-message
 * @short_description: Message container for UniqueApp
 *
 * #UniqueMessageData contains the data sent from a #UniqueApp to a
 * running instance of the same application. It can contain arbitrary
 * binary data, and provides convenience functions to set plain text
 * or URI list.
 *
 * You should create a #UniqueMessageData structure using
 * unique_message_data_new(), you can copy it using the
 * unique_message_data_copy() and you should free it using
 * unique_message_data_free().
 *
 * You can set data using unique_message_data_set(),
 * unique_message_data_set_text() or unique_message_data_set_uris().
 *
 * You can retrieve the data set using unique_message_data_get(),
 * unique_message_data_get_text() or unique_message_data_get_uris().
 */

GType
unique_message_data_get_type (void)
{
  static GType data_type = 0;

  if (G_UNLIKELY (data_type == 0))
    {
      data_type =
        g_boxed_type_register_static ("UniqueMessageData",
                                      (GBoxedCopyFunc) unique_message_data_copy,
                                      (GBoxedFreeFunc) unique_message_data_free);
    }

  return data_type;
}

UniqueMessageData *
unique_message_data_new (void)
{
  return g_slice_new0 (UniqueMessageData);
}

/**
 * unique_message_data_copy:
 * @message_data: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
UniqueMessageData *
unique_message_data_copy (UniqueMessageData *message_data)
{
  UniqueMessageData *retval;

  retval = g_slice_new (UniqueMessageData);
  *retval = *message_data;

  if (message_data->data)
    {
      retval->data = g_malloc (message_data->length + 1);
      memcpy (retval->data, message_data->data, message_data->length + 1);
    }

  retval->screen = message_data->screen;

  return retval;
}

/**
 * unique_message_data_free:
 * @message_data: FIXME
 *
 * FIXME
 */
void
unique_message_data_free (UniqueMessageData *message_data)
{
  if (G_LIKELY (message_data))
    {
      g_free (message_data->data);
      g_slice_free (UniqueMessageData, message_data);
    }
}

/**
 * unique_message_data_set:
 * @message_data: FIXME
 * @screen: FIXME
 * @data: FIXME
 * @length: FIXME
 *
 * FIXME
 */
void
unique_message_data_set (UniqueMessageData *message_data,
                         GdkScreen         *screen,
                         const guchar      *data,
                         gsize              length)
{
  g_return_if_fail (message_data != NULL);
  g_return_if_fail (screen == NULL || GDK_IS_SCREEN (screen));

  g_free (message_data->data);

  message_data->screen = screen;
  if (!message_data->screen)
    message_data->screen = gdk_screen_get_default ();

  if (data)
    {
      message_data->data = g_new (guchar, length + 1);
      memcpy (message_data->data, data, length);
      message_data->data[length] = 0;
    }
  else
    {
      g_return_if_fail (length <= 0);

      if (length < 0)
        message_data->data = NULL;
      else
        message_data->data = (guchar *) g_strdup ("");
    }

  message_data->length = length;
}

/* taken from gtkselection.c */
/* normalize \r and \n into \r\n */
static gchar *
normalize_to_crlf (const gchar *str, 
                   gint         len)
{
  GString *result = g_string_sized_new (len);
  const gchar *p = str;

  while (1)
    {
      if (*p == '\n')
        g_string_append_c (result, '\r');

      if (*p == '\r')
        {
          g_string_append_c (result, *p);
          p++;
          if (*p != '\n')
            g_string_append_c (result, '\n');
        }

      if (*p == '\0')
        break;

      g_string_append_c (result, *p);
      p++;
    }

  return g_string_free (result, FALSE);  
}

/* taken from gtkselection.c */
/* normalize \r and \r\n into \n */
static gchar *
normalize_to_lf (gchar *str, 
                 gint   len)
{
  GString *result = g_string_sized_new (len);
  const gchar *p = str;

  while (1)
    {
      if (*p == '\r')
        {
          p++;
          if (*p != '\n')
            g_string_append_c (result, '\n');
        }

      if (*p == '\0')
        break;

      g_string_append_c (result, *p);
      p++;
    }

  return g_string_free (result, FALSE);  
}

static gboolean
message_data_set_text_plain (UniqueMessageData *message_data,
                             GdkScreen         *screen,
                             const gchar       *str,
                             gsize              length)
{
  const gchar *charset = NULL;
  gchar *result, *tmp;
  GError *error = NULL;

  result = normalize_to_crlf (str, length);
  
  g_get_charset (&charset);
  if (!charset)
    charset = "ASCII";

  tmp = result;
  result = g_convert_with_fallback (tmp, -1, charset, "UTF-8",
                                    NULL, NULL, NULL,
                                    &error);
  g_free (tmp);

  if (!result)
    {
      g_warning ("Error converting from %s to %s: %s",
                 "UTF-8", charset, error->message);
      g_error_free (error);

      return FALSE;
    }

  unique_message_data_set (message_data,
                           screen,
                           (guchar *) result, strlen (result));
  
  return TRUE;
}

static gchar *
message_data_get_text_plain (UniqueMessageData *message_data)
{
  const gchar *charset = NULL;
  gchar *str, *tmp, *result;
  gsize len;
  GError *error = NULL;

  str = g_strdup ((gchar *) message_data->data);
  len = message_data->length;

  g_get_charset (&charset);
  if (!charset)
    charset = "ISO-8859-1";

  tmp = str;
  str = g_convert_with_fallback (tmp, len,
                                 charset, "UTF-8",
                                 NULL, NULL, &len,
                                 &error);
  g_free (tmp);

  if (!str)
    {
      g_warning ("Error converting from %s to %s: %s",
                 charset, "UTF-8", error->message);
      g_error_free (error);

      return NULL;
    }
  else if (!g_utf8_validate (str, -1, NULL))
    {
      g_warning ("Error converting from %s to %s: %s",
                 "text/plain;charset=utf-8", "UTF-8", "invalid UTF-8");
      g_free (str);

      return NULL;
    }

  result = normalize_to_lf (str, len);
  g_free (str);

  return result;
}

/**
 * unique_message_data_set_text:
 * @str: FIXME
 * @length: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
gboolean
unique_message_data_set_text (UniqueMessageData *message_data,
                              GdkScreen         *screen,
                              const gchar       *str,
                              gsize              length)
{
  if (length < 0)
    length = strlen (str);

  if (g_utf8_validate (str, length, NULL))
    {
      unique_message_data_set (message_data,
                               screen,
                               (guchar *) str, length);
      return TRUE;
    }

  return message_data_set_text_plain (message_data, screen, str, length);
}

/**
 * unique_message_data_get_text:
 * @message_data: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
gchar *
unique_message_data_get_text (UniqueMessageData *message_data)
{
  return message_data_get_text_plain (message_data);
}

/**
 * unique_message_data_set_uris:
 * @message_data: FIXME
 * @uris: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
gboolean
unique_message_data_set_uris (UniqueMessageData  *message_data,
                              GdkScreen          *screen,
                              gchar             **uris)
{
  GString *list;
  gint i;
  gchar *result;
  gsize length;

  list = g_string_new (NULL);
  for (i = 0; uris[i]; i++)
    {
      g_string_append (list, uris[i]);
      g_string_append (list, "\r\n");
    }

  result = g_convert (list->str, list->len,
                      "ASCII", "UTF-8",
                      NULL, &length, NULL);
  g_string_free (list, TRUE);

  if (result)
    {
      unique_message_data_set (message_data,
                               screen,
                               (guchar *) result, length);
      g_free (result);

      return TRUE;
    }

  return FALSE;
}

/**
 * unique_message_data_get_uris:
 * @message_data: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
gchar **
unique_message_data_get_uris (UniqueMessageData *message_data)
{
  gchar **result = NULL;

  if (message_data->length >= 0)
    {
      gchar *text;

      text = message_data_get_text_plain (message_data);
      if (text)
        {
          result = g_uri_list_extract_uris (text);
          g_free (text);
        }
    }

  return result;
}

/**
 * unique_message_data_get_screen:
 * @message_data: a #UniqueMessageData
 *
 * FIXME
 *
 * Return value: a #GdkScreen
 */
GdkScreen *
unique_message_data_get_screen (UniqueMessageData *message_data)
{
  g_return_val_if_fail (message_data != NULL, NULL);

  return message_data->screen;
}

/**
 * unique_message_data_get_startup_id:
 * @message_data: a #UniqueMessageData
 *
 * Retrieves the startup notification id set inside @message_data.
 *
 * Return value: the startup notification id. The returned string is
 *   owned by the #UniqueMessageData structure and should not be
 *   modified or freed
 */
G_CONST_RETURN gchar *
unique_message_data_get_startup_id (UniqueMessageData *message_data)
{
  g_return_val_if_fail (message_data != NULL, NULL);

  return message_data->startup_id;
}
