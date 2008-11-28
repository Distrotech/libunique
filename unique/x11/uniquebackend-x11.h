/* Unique - Single Instance Backendlication library
 * uniquebackend-x11.h: Xlibs-based backend 
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

#ifndef __UNIQUE_BACKEND_X11_H__
#define __UNIQUE_BACKEND_X11_H__

#include <unique/uniquebackend.h>

G_BEGIN_DECLS

#define UNIQUE_TYPE_BACKEND_X11                 (unique_backend_x11_get_type ())
#define UNIQUE_BACKEND_X11(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNIQUE_TYPE_BACKEND_X11, UniqueBackendX11))
#define UNIQUE_IS_BACKEND_X11(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNIQUE_TYPE_BACKEND_X11))
#define UNIQUE_BACKEND_X11_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), UNIQUE_TYPE_BACKEND_X11, UniqueBackendX11Class))
#define UNIQUE_IS_BACKEND_X11_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), UNIQUE_TYPE_BACKEND_X11))
#define UNIQUE_BACKEND_X11_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), UNIQUE_TYPE_BACKEND_X11, UniqueBackendX11Class))

typedef struct _UniqueBackendX11       UniqueBackendX11;
typedef struct _UniqueBackendX11Class  UniqueBackendX11Class;

GType unique_backend_x11_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __UNIQUE_BACKEND_X11_H__ */
