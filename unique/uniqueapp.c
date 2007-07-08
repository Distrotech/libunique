/* uniqueapp.c - Single instance application object
 * Unique - Single instance applications library
 *
 * Copyright (C) 2007  Emmanuele Bassi, <ebassi@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <gtk/gtk.h>

#include "uniquebackend.h"
#include "uniqueapp.h"
#include "uniquemarshal.h"
#include "uniqueinternals.h"



GType
unique_command_get_type (void)
{
  static GType etype = 0;

  if (G_UNLIKELY (etype == 0))
    {
      static const GEnumValue values[] = {
        { UNIQUE_INVALID, "UNIQUE_INVALID", "invalid" },
        { UNIQUE_ACTIVATE, "UNIQUE_ACTIVATE", "activate" },
        { UNIQUE_NEW, "UNIQUE_NEW", "new" },
        { UNIQUE_OPEN, "UNIQUE_OPEN", "open" },
        { UNIQUE_CLOSE, "UNIQUE_CLOSE", "close" },
        { 0, NULL, NULL }
      };

      etype = g_enum_register_static ("UniqueCommand", values);
    }

  return etype;
}

GType
unique_response_get_type (void)
{
  static GType etype = 0;

  if (G_UNLIKELY (etype == 0))
    {
      static const GEnumValue values[] = {
        { UNIQUE_RESPONSE_INVALID, "UNIQUE_RESPONSE_INVALID", "invalid" },
        { UNIQUE_RESPONSE_OK, "UNIQUE_RESPONSE_OK", "ok" },
        { UNIQUE_RESPONSE_CANCEL, "UNIQUE_RESPONSE_CANCEL", "cancel" },
        { UNIQUE_RESPONSE_FAIL, "UNIQUE_RESPONSE_FAIL", "fail" },
        { 0, NULL, NULL }
      };

      etype = g_enum_register_static ("UniqueResponse", values);
    }

  return etype;
}



G_DEFINE_TYPE (UniqueApp, unique_app, G_TYPE_OBJECT);

#define UNIQUE_APP_GET_PRIVATE(obj)     (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                         UNIQUE_TYPE_APP, \
                                         UniqueAppPrivate))

struct _UniqueAppPrivate
{
  UniqueBackend *backend;

  guint is_running : 1;
};

enum
{
  PROP_0,

  PROP_NAME,
  PROP_STARTUP_ID,
  PROP_SCREEN,
  PROP_IS_RUNNING
};

enum
{
  MESSAGE_RECEIVED,

  LAST_SIGNAL
};

static guint unique_app_signals[LAST_SIGNAL] = { 0, };

static gboolean
message_accumulator (GSignalInvocationHint *ihint,
                     GValue                *return_accu,
                     const GValue          *handler_return,
                     gpointer               dummy)
{
  gboolean continue_emission;
  UniqueResponse response;

  response = g_value_get_enum (handler_return);
  g_value_set_enum (return_accu, response);

  continue_emission = (response == UNIQUE_RESPONSE_OK);

  return continue_emission;
}

static GObject *
unique_app_constructor (GType                  gtype,
                        guint                  n_params,
                        GObjectConstructParam *params)
{
  GObjectClass *parent_class;
  GObject *retval;
  UniqueApp *app;
  UniqueAppPrivate *priv;

  parent_class = G_OBJECT_CLASS (unique_app_parent_class);
  retval = parent_class->constructor (gtype, n_params, params);
  app = UNIQUE_APP (retval);
  priv = app->priv;

  /* this is where the magic happens; we require a name and if the
   * backend returns TRUE then it means that we have it, and that
   * this is the first instance.
   */
  g_assert (UNIQUE_IS_BACKEND (priv->backend));
  priv->is_running = (unique_backend_request_name (priv->backend) == FALSE);

  return retval;
}

static void
unique_app_set_property (GObject      *gobject,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  UniqueApp *app = UNIQUE_APP (gobject);
  UniqueBackend *backend = app->priv->backend;

  switch (prop_id)
    {
    case PROP_NAME:
      unique_backend_set_name (backend, g_value_get_string (value));
      break;
    case PROP_STARTUP_ID:
      unique_backend_set_startup_id (backend, g_value_get_string (value));
      break;
    case PROP_SCREEN:
      unique_backend_set_screen (backend, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
unique_app_get_property (GObject    *gobject,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  UniqueApp *app = UNIQUE_APP (gobject);
  UniqueBackend *backend = app->priv->backend;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, unique_backend_get_name (backend));
      break;
    case PROP_STARTUP_ID:
      g_value_set_string (value, unique_backend_get_startup_id (backend));
      break;
    case PROP_SCREEN:
      g_value_set_object (value, unique_backend_get_screen (backend));
      break;
    case PROP_IS_RUNNING:
      g_value_set_boolean (value, app->priv->is_running);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
unique_app_class_init (UniqueAppClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->constructor = unique_app_constructor;
  gobject_class->set_property = unique_app_set_property;
  gobject_class->get_property = unique_app_get_property;

  /**
   * UniqueApp:name:
   *
   * The unique name of the application. It must be in form of
   * a domain-like string, like <literal>org.gnome.MyApplication</literal>.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "The unique name of the application",
                                                        NULL,
                                                        UNIQUE_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY));
  /**
   * UniqueApp:startup-id:
   *
   * The startup notification id, needed to complete the startup
   * notification sequence. If not set, a default id will be
   * automatically given.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_STARTUP_ID,
                                   g_param_spec_string ("startup-id",
                                                        "Startup Id",
                                                        "The startup notification id for the application",
                                                        NULL,
                                                        UNIQUE_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY));
  /**
   * UniqueApp:screen:
   *
   * The #GdkScreen of the application.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_SCREEN,
                                   g_param_spec_object ("screen",
                                                        "Screen",
                                                        "The GdkScreen of the application",
                                                        GDK_TYPE_SCREEN,
                                                        UNIQUE_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));
  /**
   * UniqueApp:is-running:
   *
   * Whether another instance of the application is running.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_IS_RUNNING,
                                   g_param_spec_boolean ("is-running",
                                                         "Is Running",
                                                         "Whether another instance is running",
                                                         FALSE,
                                                         UNIQUE_PARAM_READABLE));
  /**
   * UniqueApp::message-received:
   * @app: the object which received the signal
   * @command: command received
   * @message_data: message data
   * @time_: timestamp of the command
   *
   * The ::message-received signal is emitted each time a second instance
   * of #UniqueApp with the same name as @app is launched and sends a
   * message using unique_app_send_message(). The currently running instance
   * should check @command for the action to execute and @message_data for
   * eventual other parameters (see #UniqueMessageData).
   *
   * The signal handler should return a #UniqueResponse value depending on
   * whether the command was successfully completed or not.
   */
  unique_app_signals[MESSAGE_RECEIVED] =
    g_signal_new ("message-received",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (UniqueAppClass, message_received),
                  message_accumulator, NULL,
                  _unique_marshal_ENUM__INT_BOXED_UINT,
                  UNIQUE_TYPE_RESPONSE,
                  3,
                  G_TYPE_INT,               /* command */
                  UNIQUE_TYPE_MESSAGE_DATA, /* message_data */
                  G_TYPE_UINT               /* time_ */);

  g_type_class_add_private (klass, sizeof (UniqueAppPrivate));
}

static void
unique_app_init (UniqueApp *app)
{
  UniqueAppPrivate *priv;
  UniqueBackend *backend;

  priv = app->priv = UNIQUE_APP_GET_PRIVATE (app);

  backend = g_object_new (unique_backend_impl_get_type (), NULL);
  backend->parent = app;

  priv->backend = backend;

  priv->is_running = FALSE;
}

/**
 * unique_app_new_with_startup_id:
 * @name: the name of the application
 * @startup_id: the startup id or %NULL
 *
 * Creates a new #UniqueApp instance for @name passing a start-up notification
 * id @startup_id.  The name must be a unique identifier for the application,
 * and it must be in form of a domain name, like
 * <literal>org.gnome.YourApplication</literal>.
 *
 * Once you have created a #UniqueApp instance, you should check if
 * any other instance is running, using unique_app_is_running().
 * If another instance is running you can send a command to it, using
 * the unique_app_send_message() function; after that, the second instance
 * should quit. If no other instance is running, the usual logic for
 * creating the application can follow.
 * 
 * Return value: the newly created #UniqueApp instance. Use
 *   g_object_unref() when finished.
 */
UniqueApp *
unique_app_new_with_startup_id (const gchar *name,
                                const gchar *startup_id)
{
  g_return_val_if_fail (name != NULL, NULL);

  return g_object_new (UNIQUE_TYPE_APP,
                       "name", name,
                       "startup-id", startup_id,
                       NULL);
}

/* taken from nautilus */
static guint32
slowly_and_stupidly_obtain_timestamp (GdkDisplay *display)
{
  Display *xdisplay;
  Window xwindow;
  XEvent event;
  XSetWindowAttributes attrs;
  Atom atom_name;
  Atom atom_type;
  char *name;

  xdisplay = GDK_DISPLAY_XDISPLAY (display);

  attrs.override_redirect = True;
  attrs.event_mask = PropertyChangeMask | StructureNotifyMask;

  xwindow = XCreateWindow (xdisplay, RootWindow (xdisplay, 0),
                           -100, -100, 1, 1,
                           0,
                           CopyFromParent,
                           CopyFromParent,
                           CopyFromParent,
                           CWOverrideRedirect | CWEventMask,
                           &attrs);

  atom_name = XInternAtom (xdisplay, "WM_NAME", TRUE);
  g_assert (atom_name != None);

  atom_type = XInternAtom (xdisplay, "STRING", TRUE);
  g_assert (atom_type != None);

  name = "Fake Window";
  XChangeProperty (xdisplay, xwindow, atom_name, atom_type,
		   8, PropModeReplace,
                   (unsigned char *) name, strlen (name));

  XWindowEvent (xdisplay, xwindow,
                PropertyChangeMask,
                &event);

  XDestroyWindow(xdisplay, xwindow);

  return event.xproperty.time;
}

/**
 * unique_app_new:
 * @name: the name of the application's instance
 *
 * Creates a new #UniqueApp instance for @name. See
 * unique_app_new_with_startup_id() for more informations.
 * 
 * Return value: the newly created #UniqueApp instance.
 */
UniqueApp *
unique_app_new (const gchar *name)
{
  gchar *startup_id;
  UniqueApp *retval;

  g_return_val_if_fail (name != NULL, NULL);

  startup_id = g_strdup (g_getenv ("DESKTOP_STARTUP_ID"));
  if (!startup_id)
    {
      GdkDisplay *display;
      guint32 timestamp;

      display = gdk_display_get_default ();
      timestamp = slowly_and_stupidly_obtain_timestamp (display);
      startup_id = g_strdup_printf ("_TIME%lu", (unsigned long) timestamp);
    }

  retval = unique_app_new_with_startup_id (name, startup_id);
  g_free (startup_id);

  return retval;
}

/**
 * unique_app_is_running:
 * @app: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
gboolean
unique_app_is_running (UniqueApp *app)
{
  g_return_val_if_fail (UNIQUE_IS_APP (app), FALSE);

  return app->priv->is_running;
}

/**
 * unique_app_send_message:
 * @app: FIXME
 * @command_id: FIXME
 * @command_data: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
UniqueResponse
unique_app_send_message (UniqueApp         *app,
                         gint               command_id,
                         UniqueMessageData *message_data)
{
  UniqueAppPrivate *priv;
  UniqueResponse response = UNIQUE_RESPONSE_INVALID;
  guint now;

  g_return_val_if_fail (UNIQUE_IS_APP (app), UNIQUE_RESPONSE_INVALID);
  g_return_val_if_fail (command_id != 0, UNIQUE_RESPONSE_INVALID);

  priv = app->priv;

  now = (guint) time (NULL);
  response = unique_backend_send_message (priv->backend,
                                          command_id,
                                          message_data,
                                          now);

  return response;
}

UniqueResponse
unique_app_emit_message (UniqueApp         *app,
                         gint               command_id,
                         UniqueMessageData *message_data,
                         guint              time_)
{
  UniqueResponse response;

  g_return_val_if_fail (UNIQUE_IS_APP (app), UNIQUE_RESPONSE_INVALID);

  g_signal_emit (app, unique_app_signals[MESSAGE_RECEIVED], 0,
                 command_id,
                 message_data,
                 time_,
                 &response);

  return response;
}

G_LOCK_DEFINE_STATIC (commands);
static GHashTable *commands_by_name = NULL;
static GHashTable *commands_by_id   = NULL;

/**
 * unique_command_register:
 * @command_name: FIXME
 * @command_id: FIXME
 *
 * FIXME
 */
void
unique_command_register (const gchar *command_name,
                         guint        command_id)
{
  gchar *command_nick;

  g_return_if_fail (command_name != NULL);
  g_return_if_fail (command_id > 0);

  G_LOCK (commands);

  if (G_UNLIKELY (!commands_by_name))
    {
      commands_by_name = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                g_free, NULL);
      commands_by_id = g_hash_table_new (NULL, NULL);
    }

  command_nick = g_strdup (command_name);
  g_hash_table_replace (commands_by_name,
                        command_nick,
                        GUINT_TO_POINTER (command_id));
  g_hash_table_replace (commands_by_id,
                        GUINT_TO_POINTER (command_id),
                        command_nick);

  G_UNLOCK (commands);
}

G_CONST_RETURN gchar *
unique_command_to_string (gint command)
{
  const gchar *retval;

  g_return_val_if_fail (command != 0, NULL);

  if (command < 0)
    {
      GEnumClass *enum_class;
      GEnumValue *enum_value;

      enum_class = g_type_class_ref (UNIQUE_TYPE_COMMAND);
      enum_value = g_enum_get_value (enum_class, command);
      retval = enum_value->value_nick;

      g_type_class_unref (enum_class);
    }
  else
    {
      if (!commands_by_id)
        {
          g_warning ("No user commands defined. You should add new commands "
                     "with unique_command_register() before creating the "
                     "UniqueApp instance.");
          return NULL;
        }

      retval = g_hash_table_lookup (commands_by_id, GINT_TO_POINTER (command));
    }

  return retval;
}

gint
unique_command_from_string (const gchar *command)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value;
  gint retval = 0;

  g_return_val_if_fail (command != NULL, 0);

  enum_class = g_type_class_ref (UNIQUE_TYPE_COMMAND);
  enum_value = g_enum_get_value_by_nick (enum_class, command);
  if (enum_value)
    {
      retval = enum_value->value;
      g_type_class_unref (enum_class);
    }
  else
    {
      if (!commands_by_name)
        {
          g_warning ("No user commands defined. You should add new commands "
                     "with unique_command_register() before creating the "
                     "UniqueApp instance.");
          return 0;
        }

      retval = GPOINTER_TO_UINT (g_hash_table_lookup (commands_by_name,
                                                      command));
    }

  return retval;
}

G_CONST_RETURN gchar *
unique_response_to_string (UniqueResponse response)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value;
  const gchar *retval;

  enum_class = g_type_class_ref (UNIQUE_TYPE_RESPONSE);
  enum_value = g_enum_get_value (enum_class, response);
  if (!enum_value)
    return "invalid";

  retval = enum_value->value_nick;

  g_type_class_unref (enum_class);

  return retval;
}

UniqueResponse
unique_response_from_string (const gchar *response)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value;
  gint retval = UNIQUE_RESPONSE_INVALID;

  g_return_val_if_fail (response != NULL, 0);

  enum_class = g_type_class_ref (UNIQUE_TYPE_RESPONSE);
  enum_value = g_enum_get_value_by_nick (enum_class, response);
  if (enum_value)
    {
      retval = enum_value->value;
      g_type_class_unref (enum_class);
    }
  
  return retval;
}
