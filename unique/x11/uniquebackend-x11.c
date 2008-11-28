/* Unique - Single Instance Backendlication library
 * uniquebackend-bacon.c: Xlibs-based backend 
 *
 * Copyright (C) 2008  Emmanuele Bassi  <ebassi@gnome.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#include "../uniqueinternals.h"
#include "uniquebackend-x11.h"

struct _UniqueBackendX11
{
  UniqueBackend parent_instance;

  guint is_server : 1;
};

struct _UniqueBackendX11Class
{
  UniqueBackendClass parent_class;
};

G_DEFINE_TYPE (UniqueBackendX11, unique_backend_x11, UNIQUE_TYPE_BACKEND);


static void
unique_backend_x11_finalize (GObject *gobject)
{
  G_OBJECT_CLASS (unique_backend_x11_parent_class)->finalize (gobject);
}

static UniqueResponse
unique_backend_bacon_send_message (UniqueBackend     *backend,
                                   gint               command_id,
                                   UniqueMessageData *message,
                                   guint              time_)
{
  return UNIQUE_RESPONSE_INVALID;
}

static gboolean
unique_backend_x11_request_name (UniqueBackend *backend)
{
  UniqueBackendX11 *backend_x11 = UNIQUE_BACKEND_X11 (backend);

  return backend_x11->is_server;
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
  backend_x11->is_server = FALSE;
}
