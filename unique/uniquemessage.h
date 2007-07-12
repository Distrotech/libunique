/* Unique - Single Instance Backendlication library
 * uniquemessage.h: Wrapper around messages
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

#ifndef __UNIQUE_MESSAGE_H__
#define __UNIQUE_MESSAGE_H__

#include <glib-object.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

#define UNIQUE_TYPE_MESSAGE_DATA        (unique_message_data_get_type ())

typedef struct _UniqueMessageData       UniqueMessageData;

GType                 unique_message_data_get_type       (void) G_GNUC_CONST;
UniqueMessageData *   unique_message_data_new            (void);
UniqueMessageData *   unique_message_data_copy           (UniqueMessageData *message_data);
void                  unique_message_data_free           (UniqueMessageData *message_data);

void                  unique_message_data_set            (UniqueMessageData *message_data,
                                                          const guchar      *data,
                                                          gsize              length);
gboolean              unique_message_data_set_text       (UniqueMessageData *message_data,
                                                          const gchar       *str,
                                                          gsize              length);
gchar *               unique_message_data_get_text       (UniqueMessageData *message_data);
gboolean              unique_message_data_set_uris       (UniqueMessageData *message_data,
                                                          gchar            **uris);
gchar **              unique_message_data_get_uris       (UniqueMessageData *message_data);

GdkScreen *           unique_message_data_get_screen     (UniqueMessageData *message_data);
G_CONST_RETURN gchar *unique_message_data_get_startup_id (UniqueMessageData *message_data);
guint                 unique_message_data_get_workspace  (UniqueMessageData *message_data);

G_END_DECLS

#endif /* __UNIQUE_MESSAGE_H__ */
