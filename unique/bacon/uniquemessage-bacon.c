#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <gdk/gdk.h>

#include "../uniqueinternals.h"
#include "uniquemessage-bacon.h"

gchar *
unique_message_data_pack (gint               command_id,
                          UniqueMessageData *message_data,
                          guint              time_,
                          gsize             *length)
{
  GString *buffer;
  gchar *escape;
  gsize len;

  buffer = g_string_new (NULL);
  len = 0;

  if (command_id == 0)
    return NULL;

  /* command */
  escape = g_strescape (unique_command_to_string (command_id), NULL);
  g_string_append (buffer, escape);
  len += strlen (escape);
  g_string_append_c (buffer, '\t');
  len += 1;
  g_free (escape);

  /* message_data: data */
  if (message_data->data)
    escape = g_strescape ((gchar *) message_data->data, NULL);
  else
    escape = g_strdup ("none");

  g_string_append (buffer, escape);
  len += strlen (escape);
  g_string_append_c (buffer, '\t');
  len += 1;
  g_free (escape);

  /* message_data: screen_n */
  escape = g_strdup_printf ("%u", gdk_screen_get_number (message_data->screen));
  g_string_append (buffer, escape);
  len += strlen (escape);
  g_string_append_c (buffer, '\t');
  len += 1;
  g_free (escape);

  /* message_data: startup_id */
  if (message_data->startup_id)
    escape = g_strescape (message_data->startup_id, NULL);
  else
    escape = g_strdup ("none");

  g_string_append (buffer, escape);
  len += strlen (escape);
  g_string_append_c (buffer, '\t');
  len += 1;
  g_free (escape);

  /* time */
  escape = g_strdup_printf ("%d", time_);
  g_string_append (buffer, escape);
  len += strlen (escape);
  g_free (escape);

  /* terminator */
  g_string_append (buffer, "\r\n");
  len += 2;

  if (length)
    *length = len;

  return g_string_free (buffer, FALSE);
}

UniqueMessageData *
unique_message_data_unpack (const gchar *data,
                            gint        *command_id,
                            guint       *time_)
{
  gchar **blocks;
  gchar *buf;
  GdkDisplay *display;
  gint screen_n;
  UniqueMessageData *message_data;

  blocks = g_strsplit (data, "\t", 5);
  if (g_strv_length (blocks) != 5)
    goto error;

  if (command_id)
    {
      buf = g_strcompress (blocks[0]);
      *command_id = unique_command_from_string (buf);
      g_free (buf);
    }

  message_data = g_slice_new (UniqueMessageData);

  if (strcmp (blocks[1], "none") != 0)
    {
      buf = g_strcompress (blocks[1]);
      message_data->data = (guchar *) g_strdup (buf);
      message_data->length = strlen (buf);
      g_free (buf);
    }
  else
    {
      message_data->data = NULL;
      message_data->length = 0;
    }

  screen_n = g_ascii_strtoll (blocks[3], NULL, 10);
  display = gdk_display_get_default ();
  message_data->screen = gdk_display_get_screen (display, screen_n);

  if (strcmp (blocks[3], "none") != 0)
    {
      buf = g_strcompress (blocks[3]);
      message_data->startup_id = g_strdup (buf);
      g_free (buf);
    }
  else
    message_data->startup_id = NULL;

  if (time_)
    *time_ = (guint) g_ascii_strtoll (blocks[4], NULL, 10);

error:
  g_strfreev (blocks);

  return message_data;
}
