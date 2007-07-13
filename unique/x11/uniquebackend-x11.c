/* Unique - Single Instance Application library
 * uniquebackend-x11.h: Xlibs implementation of UniqueBackend
 *
 * Copyright (C) 2007  Emmanuele Bassi  <ebassi@gnome.com>
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

#include "config.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "../uniqueinternals.h"
#include "uniquebackend-x11.h"

struct _UniqueBackendX11
{
  UniqueBackend parent_instance;

  Display *xdisplay;
  Atom selection_atom;
  Window root_xwindow;
};

struct _UniqueBackendX11Class
{
  UniqueBackendClass parent_class;
};

G_DEFINE_TYPE (UniqueBackendX11, unique_backend_x11, UNIQUE_TYPE_BACKEND);

#if 0
enum {
  /* GtkUnique window protocol */
  UNIQUE_ATOM_VERSION,
  UNIQUE_ATOM_NAME,
  UNIQUE_ATOM_COMMAND,
  UNIQUE_ATOM_MESSAGE_DATA,
  UNIQUE_ATOM_RESPONSE,
  
  /* utility atoms */
  UNIQUE_ATOM_WM_STATE,
  UNIQUE_ATOM_UTF8_STRING,
  UNIQUE_NET_CLIENT_LIST,

  UNIQUE_N_ATOMS
};

static const gchar *unique_atom_names[UNIQUE_N_ATOMS] = {
  "_UNIQUE_VERSION",
  "_UNIQUE_NAME",
  "_UNIQUE_COMMAND",
  "_UNIQUE_MESSAGE_DATA",
  "_UNIQUE_RESPONSE",

  "WM_STATE",
  "UTF8_STRING",
  "_NET_CLIENT_LIST",

  NULL
};

static Atom         unique_atom[UNIQUE_N_ATOMS] = { 0, };

static gpointer
get_win_prop_data_and_validate (Display      *xdisplay,
                                Window        xwin, 
				Atom          prop, 
				Atom          type, 
				int           expected_format,
				unsigned int  expected_n_items,
				unsigned int *n_items_ret)
{
  Atom type_ret;
  int format;
  unsigned long n_items, bytes_after;
  unsigned char *data_return = 0;
  int status, err;

  gdk_error_trap_push ();

  status = XGetWindowProperty (xdisplay, xwin, 
			       prop, 
			       0, G_MAXLONG, 
			       False,
			       type, 
			       &type_ret, &format, &n_items, &bytes_after, 
			       &data_return);

  err = gdk_error_trap_pop ();

  if (err != Success || status != Success)
    goto fail;

  if (!data_return)
    goto fail;

  if (expected_format && format != expected_format)
    goto fail;

  if (expected_n_items && n_items != expected_n_items)
    goto fail;

  if (n_items_ret)
    *n_items_ret = n_items;
  
  return data_return;

fail:
  if (data_return)
    XFree (data_return);

  return NULL;
}

static gpointer
steal_win_prop_data_and_validate (Display      *xdisplay,
                                  Window        xwin, 
                                  Atom          prop, 
                                  Atom          type, 
                                  int           expected_format,
                                  unsigned int  expected_n_items,
                                  unsigned int *n_items_ret)
{
  Atom type_ret;
  int format;
  unsigned long n_items, bytes_after;
  unsigned char *data_return = 0;
  int status, err;

  gdk_error_trap_push ();

  status = XGetWindowProperty (xdisplay, xwin, 
			       prop, 
			       0, G_MAXLONG, 
			       True,
			       type, 
			       &type_ret, &format, &n_items, &bytes_after, 
			       &data_return);

  err = gdk_error_trap_pop ();

  if (err != Success || status != Success)
    goto fail;

  if (!data_return)
    goto fail;

  if (expected_format && format != expected_format)
    goto fail;

  if (expected_n_items && n_items != expected_n_items)
    goto fail;

  if (n_items_ret)
    *n_items_ret = n_items;
  
  return data_return;

fail:
  if (data_return)
    XFree (data_return);

  return NULL;
}
#endif

static void
unique_backend_x11_finalize (GObject *gobject)
{
  G_OBJECT_CLASS (unique_backend_x11_parent_class)->finalize (gobject); 
}

static gboolean
unique_backend_x11_request_name (UniqueBackend *backend)
{
  UniqueBackendX11 *backend_x11 = UNIQUE_BACKEND_X11 (backend);
  GdkDisplay *display;
  GdkScreen *screen;
  GdkWindow *root_window;
  gboolean retval;

  /* the selection is per display, on the default screen */
  display = gdk_screen_get_display (backend->screen);
  screen = gdk_display_get_default_screen (display);
  root_window = gdk_screen_get_root_window (screen);

  if (!backend_x11->xdisplay)
    backend_x11->xdisplay = GDK_DISPLAY_XDISPLAY (display);

  if (!backend_x11->selection_atom)
    backend_x11->selection_atom = XInternAtom (GDK_DISPLAY_XDISPLAY (display),
                                               backend->name,
                                               False);

  if (!backend_x11->root_xwindow)
    backend_x11->root_xwindow = GDK_WINDOW_XID (root_window);

  XGrabServer (GDK_DISPLAY_XDISPLAY (display));

  if (XGetSelectionOwner (backend_x11->xdisplay, backend_x11->selection_atom))
    retval = FALSE;
  else
    {
      gint err;

      gdk_error_trap_push ();

      XSetSelectionOwner (backend_x11->xdisplay,
                          backend_x11->selection_atom,
                          backend_x11->root_xwindow,
                          GDK_CURRENT_TIME);

      err = gdk_error_trap_pop ();

      if (err == Success)
        retval = TRUE;
      else
        retval = FALSE;
    }

  gdk_flush ();
  XUngrabServer (GDK_DISPLAY_XDISPLAY (display));

  return retval;
}

static UniqueResponse
unique_backend_x11_send_message (UniqueBackend     *backend,
                                 gint               command,
                                 UniqueMessageData *message_data,
                                 guint              time_)
{
  UniqueBackendX11 *backend_x11;

  backend_x11 = UNIQUE_BACKEND_X11 (backend);



  return UNIQUE_RESPONSE_OK;
}

static void
unique_backend_x11_class_init (UniqueBackendX11Class *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  UniqueBackendClass *backend_class = UNIQUE_BACKEND_CLASS (klass);

  gobject_class->finalize = unique_backend_x11_finalize;

  backend_class->request_name = unique_backend_x11_request_name;
  backend_class->send_message = unique_backend_x11_send_message;
}

static void
unique_backend_x11_init (UniqueBackendX11 *backend_x11)
{
  backend_x11->selection_atom = 0;
}
