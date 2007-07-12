/* Unique - Single Instance Backendlication library
 * uniqueapp.h: Base class for single instance applications
 *
 * Copyright (C) 2007  Emmanuele Bassi  <ebassi@o-hand.com>
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

/**
 * SECTION:unique-app
 * @short_description: Base class for singleton applications
 *
 * #UniqueApp is the base class for single instance applications. You
 * can either create an instance of #UniqueApp via unique_app_new()
 * and unique_app_new_with_commands(); or you can subclass #UniqueApp
 * with your own application class.
 *
 * A #UniqueApp instance is guaranteed to either be the first running
 * at the time of creation or be able to send messages to the currently
 * running instance; there is no race possible between the creation
 * of the #UniqueApp instance and the call to unique_app_is_running().
 *
 * The usual method for using the #UniqueApp API is to create a new
 * instance, passing an application-dependent name as constuctio-only
 * property; the UniqueApp:name property is required, and should be in the
 * form of a domain name, like <literal>org.gnome.YourApplication</literal>.
 *
 * After the creation, you should check whether an instance of your
 * application is already running, using unique_app_is_running(); if this
 * method returns %FALSE the usual application construction sequence can
 * continue; if it returns %TRUE you can either exit or send a message using
 * #UniqueMessageData and unique_app_send_message().
 *
 * You can define custom commands using unique_app_add_command(): you
 * need to provide an arbitrary integer and a string for the command.
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

  GHashTable *commands_by_name;
  GHashTable *commands_by_id;
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
unique_app_dispose (GObject *gobject)
{
  UniqueApp *app = UNIQUE_APP (gobject);
  UniqueAppPrivate *priv = app->priv;

  if (priv->backend)
    {
      g_object_unref (priv->backend);
      priv->backend = NULL;
    }

  G_OBJECT_CLASS (unique_app_parent_class)->dispose (gobject);
}

static void
unique_app_finalize (GObject *gobject)
{
  UniqueApp *app = UNIQUE_APP (gobject);
  UniqueAppPrivate *priv = app->priv;

  if (priv->commands_by_name)
    g_hash_table_destroy (priv->commands_by_name);

  if (priv->commands_by_id)
    g_hash_table_destroy (priv->commands_by_id);

  G_OBJECT_CLASS (unique_app_parent_class)->finalize (gobject);
}

static void
unique_app_class_init (UniqueAppClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->constructor = unique_app_constructor;
  gobject_class->set_property = unique_app_set_property;
  gobject_class->get_property = unique_app_get_property;
  gobject_class->dispose = unique_app_dispose;
  gobject_class->finalize = unique_app_finalize;

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
                  unique_marshal_ENUM__INT_BOXED_UINT,
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

  backend = unique_backend_create ();
  backend->parent = app;

  priv->backend = backend;
  priv->is_running = FALSE;
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
 * @startup_id: the startup notification id, or %NULL
 *
 * Creates a new #UniqueApp instance for @name passing a start-up notification
 * id @startup_id.  The name must be a unique identifier for the application,
 * and it must be in form of a domain name, like
 * <literal>org.gnome.YourApplication</literal>.
 *
 * If @startup_id is %NULL the <literal>DESKTOP_STARTUP_ID</literal>
 * environment variable will be check, and if that fails a "fake" startup
 * notification id will be created.
 *
 * Once you have created a #UniqueApp instance, you should check if
 * any other instance is running, using unique_app_is_running().
 * If another instance is running you can send a command to it, using
 * the unique_app_send_message() function; after that, the second instance
 * should quit. If no other instance is running, the usual logic for
 * creating the application can follow.
 * 
 * Return value: the newly created #UniqueApp instance.
 */
UniqueApp *
unique_app_new (const gchar *name,
                const gchar *startup_id)
{
  UniqueApp *retval;
  gchar *id;

  g_return_val_if_fail (name != NULL, NULL);

  if (startup_id && startup_id != '\0')
    id = g_strdup (startup_id);
  else
    {
      id = g_strdup (g_getenv ("DESKTOP_STARTUP_ID"));
      if (!id)
        {
          GdkDisplay *display;
          guint32 timestamp;

          display = gdk_display_get_default ();
          timestamp = slowly_and_stupidly_obtain_timestamp (display);
          id = g_strdup_printf ("_TIME%lu", (unsigned long) timestamp);
        }
    }

  retval = g_object_new (UNIQUE_TYPE_APP,
                         "name", name,
                         "startup-id", id,
                         NULL); 

  g_free (id);

  return retval;
}

static void
unique_app_add_commands_valist (UniqueApp   *app,
                                const gchar *first_command_name,
                                va_list      args)
{
  const gchar *command;
  gint command_id;

  g_return_if_fail (UNIQUE_IS_APP (app));

  command = first_command_name;
  command_id = va_arg (args, gint);

  while (command != NULL)
    {
      unique_app_add_command (app, command, command_id);

      command = va_arg (args, gchar *);
      if (command == NULL)
        break;

      command_id = va_arg (args, gint);
    }
}

/**
 * unique_app_new_with_commands:
 * @name: the name of the application
 * @startup_id: startup notification id, or %NULL
 * @first_command_name: first custom command
 * @Varargs: %NULL terminated list of command names and command ids
 *
 * Creates a new #UniqueApp instance, with @name and @startup_id,
 * and registers a list of custom commands. See unique_app_new() and
 * unique_app_add_command().
 *
 * Return value: the newly created #UniqueApp instance.
 */
UniqueApp *
unique_app_new_with_commands (const gchar *name,
                              const gchar *startup_id,
                              const gchar *first_command_name,
                              ...)
{
  UniqueApp *retval;
  va_list args;

  g_return_val_if_fail (name != NULL, NULL);

  retval = unique_app_new (name, startup_id);

  va_start (args, first_command_name);
  unique_app_add_commands_valist (retval, first_command_name, args);
  va_end (args);

  return retval;
}


/**
 * unique_app_is_running:
 * @app: a #UniqueApp
 *
 * Checks whether another instance of @app is running.
 *
 * Return value: %TRUE if there already is an instance running
 */
gboolean
unique_app_is_running (UniqueApp *app)
{
  g_return_val_if_fail (UNIQUE_IS_APP (app), FALSE);

  return app->priv->is_running;
}

/**
 * unique_app_send_message:
 * @app: a #UniqueApp
 * @command_id: command to send
 * @message_data: #UniqueMessageData, or %NULL
 *
 * Sends @command to a running instance of @app. If you need to pass data
 * to the instance, you should create a #UniqueMessageData object using
 * unique_message_data_new() and then fill it with the data you intend to
 * pass.
 *
 * The running application will receive a UniqueApp::message-received signal
 * and will call the various signal handlers attach to it. If any handler
 * returns a #UniqueResponse different than %UNIQUE_RESPONSE_OK, the emission
 * will stop.
 *
 * Return value: The #UniqueResponse returned by the running instance
 */
UniqueResponse
unique_app_send_message (UniqueApp         *app,
                         gint               command_id,
                         UniqueMessageData *message_data)
{
  UniqueAppPrivate *priv;
  UniqueBackend *backend;
  UniqueMessageData *message;
  UniqueResponse response = UNIQUE_RESPONSE_INVALID;
  guint now;

  g_return_val_if_fail (UNIQUE_IS_APP (app), UNIQUE_RESPONSE_INVALID);
  g_return_val_if_fail (command_id != 0, UNIQUE_RESPONSE_INVALID);

  priv = app->priv;
  
  if (message_data)
    message = unique_message_data_copy (message_data);
  else
    message = unique_message_data_new ();

  message->screen = unique_backend_get_screen (backend);
  message->startup_id = g_strdup (unique_backend_get_startup_id (backend));
  now = (guint) time (NULL);

  /* This is a pathological case, and if you're doing this you're
   * either testing or you are doing something very wrong, so there's
   * no need to run around screaming bloody murder.
   */
  if (G_UNLIKELY (!app->is_running))
    return UNIQUE_RESPONSE_INVALID;
  else
    response = unique_backend_send_message (backend, command_id, message, now);

  unique_message_data_free (message);

  return response;
}

/*
 * unique_app_emit_message:
 * @app: a #UniqueApp
 * @command_id: a command
 * @message_data: a #UniqueMessageData
 * @time_: time of the command
 *
 * Emits the UniqueApp::message-received on @app. This function should
 * be called by the backend implementation when it receives a message.
 *
 * Return value: the response accumulated from the signal
 */
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

/**
 * unique_app_add_command:
 * @app: a #UniqueApp 
 * @command_name: command name
 * @command_id: command logical id
 *
 * Adds @command_name as a custom command that can be used by @app. You
 * must call unique_app_add_command() before unique_app_send_message() in
 * order to use the newly added command.
 *
 * The command name is used internally: you need to use the command's logical
 * id in unique_app_send_message() and inside the UniqueApp::message-received
 * signal.
 */
void
unique_app_add_command (UniqueApp   *app,
                        const gchar *command_name,
                        gint         command_id)
{
  UniqueAppPrivate *priv;
  gchar *command_nick;

  g_return_if_fail (UNIQUE_IS_APP (app));
  g_return_if_fail (command_name != NULL);
  g_return_if_fail (command_id > 0);

  priv = app->priv;

  if (G_UNLIKELY (!priv->commands_by_name))
    {
      priv->commands_by_name = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                      g_free, NULL);
      priv->commands_by_id = g_hash_table_new (NULL, NULL);
    }

  command_nick = g_strdup (command_name);
  g_hash_table_replace (priv->commands_by_name,
                        command_nick,
                        GUINT_TO_POINTER (command_id));
  g_hash_table_replace (priv->commands_by_id,
                        GUINT_TO_POINTER (command_id),
                        command_nick);
}

G_CONST_RETURN gchar *
unique_command_to_string (UniqueApp *app,
                          gint       command)
{
  const gchar *retval;

  g_return_val_if_fail (UNIQUE_IS_APP (app), NULL);
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
      UniqueAppPrivate *priv;

      priv = app->priv;

      if (!priv->commands_by_id)
        {
          g_warning ("No user commands defined. You should add new commands "
                     "with unique_command_register() before creating the "
                     "UniqueApp instance.");
          return NULL;
        }

      retval = g_hash_table_lookup (priv->commands_by_id,
                                    GINT_TO_POINTER (command));
    }

  return retval;
}

gint
unique_command_from_string (UniqueApp   *app,
                            const gchar *command)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value;
  gint retval = 0;

  g_return_val_if_fail (UNIQUE_IS_APP (app), 0);
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
      UniqueAppPrivate *priv = app->priv;

      if (!priv->commands_by_name)
        {
          g_warning ("No user commands defined. You should add new commands "
                     "with unique_command_register() before creating the "
                     "UniqueApp instance.");
          return 0;
        }

      retval = GPOINTER_TO_UINT (g_hash_table_lookup (priv->commands_by_name,
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
