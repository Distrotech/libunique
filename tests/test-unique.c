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

  switch (command)
    {
    case UNIQUE_NEW:
      message = NULL;
    case UNIQUE_OPEN:
      title = NULL;
      message = NULL;
    case UNIQUE_ACTIVATE:
      title = NULL;
      message = NULL;
    case COMMAND_FOO:
      title = NULL;
      message = NULL;
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

  screen = unique_message_data_get_screen (message_data);
  gtk_window_set_screen (GTK_WINDOW (dialog), screen);
  gtk_dialog_run (GTK_DIALOG (dialog));

  gtk_widget_destroy (dialog);

  g_free (message);
  g_free (title);

  return UNIQUE_RESPONSE_OK;
}

int
main (int argc, char *argv[])
{
  UniqueApp *app;
  const gchar *startup_id;
  gboolean new = FALSE;
  gboolean activate = FALSE;
  gchar **uris = NULL;
  GError *init_error = NULL;
  GOptionEntry entries[] = {
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
    { NULL },
  };

  startup_id = g_getenv ("DESKTOP_STARTUP_ID");

  gtk_init_with_args (&argc, &argv,
                      "Test GtkUnique",
                      entries,
                      NULL,
                      &init_error);
  if (init_error)
    {
      g_print ("*** Error: %s\n"
               "Usage: test-unique [COMMAND]\n",
               init_error->message);
      g_error_free (init_error);

      exit (1);
    }

  unique_command_register ("foo", COMMAND_FOO);
  unique_command_register ("bar", COMMAND_BAR);
  
  app = unique_app_new_with_startup_id ("org.gnome.TestUnique", startup_id);
  if (unique_app_is_running (app))
    {
      UniqueMessageData *message;
      UniqueResponse response;
      gint command;

      message = unique_message_data_new ();

      if (new)
        {
          command = UNIQUE_NEW;
          unique_message_data_set (message, NULL, NULL, 0);
        }
      else if (uris && uris[0])
        {
          command = UNIQUE_OPEN;
          unique_message_data_set_uris (message, NULL, uris); 
        }
      else if (activate)
        {
          command = UNIQUE_ACTIVATE;
          unique_message_data_set (message, NULL, NULL, 0);
        }
      else
        {
          command = COMMAND_FOO;
          unique_message_data_set (message, NULL, "foo", 3);
        }
      
      response = unique_app_send_message (app, command, message);
      g_print ("Response code: %d\n", response);

      gdk_notify_startup_complete ();
      
      g_object_unref (app);

      exit ((response == UNIQUE_RESPONSE_OK));
    }
  else
    {
      main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      g_signal_connect (main_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
      gtk_window_set_title (GTK_WINDOW (main_window), "Test GtkUnique");
      gtk_window_set_default_size (GTK_WINDOW (main_window), 400, 300);
      gtk_container_set_border_width (GTK_CONTAINER (main_window), 12);

      g_signal_connect (app, "message-received",
                        G_CALLBACK (app_message_cb), NULL);

      gtk_widget_show (main_window);
    }
  
  gtk_main ();

  g_object_unref (app);

  return EXIT_SUCCESS;
}
