/* Unique - Single Instance Application library
 *
 * Copyright (C) 2007  Emmanuele Bassi  <ebassi@o-hand.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This progra, is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <unique/unique.h>

enum {
  COMMAND_0,

  COMMAND_FOO,
  COMMAND_BAR
};

static GtkWidget *main_window = NULL;

static UniqueResponse
app_message_cb (UniqueApp         *app,
                gint               command,
                UniqueMessageData *message_data,
                guint              time_,
                gpointer           user_data)
{
  GtkWidget *dialog;
  gchar *message, *title;
  GdkScreen *screen;
  const gchar *startup_id;

  screen = unique_message_data_get_screen (message_data);
  startup_id = unique_message_data_get_startup_id (message_data);

  g_print ("Message received from screen: %d, startup-id: %s, workspace: %d\n",
           gdk_screen_get_number (screen),
           startup_id,
           unique_message_data_get_workspace (message_data));
  
  switch (command)
    {
    case UNIQUE_NEW:
      title = g_strdup ("Received the NEW command");
      message = NULL;
      break;
    case UNIQUE_OPEN:
      {
        gchar **uris;
        gint n_uris, i;
        GString *buf;

        title = g_strdup ("Received the OPEN command");
        
        uris = unique_message_data_get_uris (message_data);
        n_uris = g_strv_length (uris);
        buf = g_string_new (NULL);
        for (i = 0; i < n_uris; i++)
          g_string_append_printf (buf, "uri: %s\n", uris[i]);

        message = g_string_free (buf, FALSE);
        g_strfreev (uris);
      }
      break;
    case UNIQUE_ACTIVATE:
      title = g_strdup ("Received the ACTIVATE command");
      message = NULL;
      break;
    case COMMAND_FOO:
      title = g_strdup ("Received the FOO command");
      message = unique_message_data_get_text (message_data);
      break;
    case COMMAND_BAR:
      title = g_strdup ("Received the BAR command");
      message = g_strdup ("Thid command doesn't do anything special");
      break;
    default:
      break;
    }

  dialog = gtk_message_dialog_new (GTK_WINDOW (main_window),
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_CLOSE,
                                   title);
  if (message)
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                              message);

  gtk_window_set_screen (GTK_WINDOW (dialog), screen);
  
  gtk_window_present (GTK_WINDOW (main_window));
  gtk_window_set_screen (GTK_WINDOW (main_window), screen);
#if GTK_CHECK_VERSION (2, 12, 0)
  gtk_window_set_startup_id (GTK_WINDOW (main_window), startup_id);
#endif

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  g_free (message);
  g_free (title);

  return UNIQUE_RESPONSE_OK;
}

static void
app_replaced_cb (UniqueApp *app,
                 gpointer   user_data)
{
  if (main_window)
    {
      gtk_widget_destroy (main_window);
      main_window = NULL;
    }

  gtk_main_quit ();
}

int
main (int argc, char *argv[])
{
  UniqueApp *app;
  gboolean new = FALSE;
  gboolean activate = FALSE;
  gboolean foo = FALSE;
  gchar **uris = NULL;
  GError *init_error = NULL;
  GOptionContext *context;
  GOptionGroup *gtk_group, *unique_group;
  const GOptionEntry entries[] = {
    { "new", 'n',
      0,
      G_OPTION_ARG_NONE, &new,
      "Send 'new' command", NULL,
    },
    { "open-uri", 'o',
      0,
      G_OPTION_ARG_STRING_ARRAY, &uris,
      "Send 'open' command", "URI",
    },
    {
      "activate", 'a',
      0,
      G_OPTION_ARG_NONE, &activate,
      "Send 'activate' command", NULL,
    },
    {
      "foo", 'f',
      0,
      G_OPTION_ARG_NONE, &foo,
      "Send 'foo' command", NULL,
    },
    { NULL },
  };

  context = g_option_context_new ("Unique test");

  gtk_group = gtk_get_option_group (FALSE);
  unique_group = unique_get_option_group ();

  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gtk_group);
  g_option_context_add_group (context, unique_group);

  if (!g_option_context_parse (context, &argc, &argv, &init_error))
    {
      g_print ("*** Error: %s\n"
               "Usage: test-unique [COMMAND]\n",
               init_error->message);
      g_error_free (init_error);

      exit (1);
    }

  gtk_init (&argc, &argv);

  app = unique_app_new_with_commands ("org.gnome.TestUnique", NULL,
                                      "foo", COMMAND_FOO,
                                      "bar", COMMAND_BAR,
                                      NULL);
  if (unique_app_is_running (app))
    {
      UniqueMessageData *message = NULL;
      UniqueResponse response;
      gint command;


      if (new)
        command = UNIQUE_NEW;
      else if (uris && uris[0])
        {
          command = UNIQUE_OPEN;
      
          message = unique_message_data_new ();
          unique_message_data_set_uris (message, uris); 
        }
      else if (activate)
        command = UNIQUE_ACTIVATE;
      else if (foo)
        {
          command = COMMAND_FOO;
          
          message = unique_message_data_new ();
          unique_message_data_set (message, (const guchar *) "bar", 3);
        }
      else
        command = COMMAND_BAR;
      
      if (message)
        {
          response = unique_app_send_message (app, command, message);
          unique_message_data_free (message);
        }
      else
        response = unique_app_send_message (app, command, NULL);

      g_print ("Response code: %d\n", response);

      gdk_notify_startup_complete ();
      
      g_object_unref (app);

      return (response == UNIQUE_RESPONSE_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  else
    {
      main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      g_signal_connect (main_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
      gtk_window_set_title (GTK_WINDOW (main_window), "Test GtkUnique");
      gtk_window_set_default_size (GTK_WINDOW (main_window), 400, 300);
      gtk_container_set_border_width (GTK_CONTAINER (main_window), 12);

      unique_app_watch_window (app, GTK_WINDOW (main_window));

      g_signal_connect (app, "message-received", G_CALLBACK (app_message_cb), NULL);
      g_signal_connect (app, "replaced", G_CALLBACK (app_replaced_cb), NULL);

      gtk_widget_show (main_window);
    }
  
  gtk_main ();

  g_object_unref (app);

  return EXIT_SUCCESS;
}
