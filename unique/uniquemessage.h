#ifndef __UNIQUE_MESSAGE_H__
#define __UNIQUE_MESSAGE_H__

#include <glib-object.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

#define UNIQUE_TYPE_MESSAGE_DATA        (unique_message_data_get_type ())

typedef struct _UniqueMessageData       UniqueMessageData;

GType              unique_message_data_get_type       (void) G_GNUC_CONST;
UniqueMessageData *unique_message_data_new            (void);
UniqueMessageData *unique_message_data_copy           (UniqueMessageData *message_data);
void               unique_message_data_free           (UniqueMessageData *message_data);
void               unique_message_data_set            (UniqueMessageData *message_data,
                                                       GdkScreen         *screen,
                                                       const guchar      *data,
                                                       gsize              length);
gboolean           unique_message_data_set_text       (UniqueMessageData *message_data,
                                                       GdkScreen         *screen,
                                                       const gchar       *str,
                                                       gsize              length);
guchar *           unique_message_data_get_text       (UniqueMessageData *message_data);
gboolean           unique_message_data_set_uris       (UniqueMessageData *message_data,
                                                       GdkScreen         *screen,
                                                       gchar            **uris);
gchar **           unique_message_data_get_uris       (UniqueMessageData *message_data);

GdkScreen *        unique_message_data_get_screen     (UniqueMessageData *message_data);
gchar *            unique_message_data_get_startup_id (UniqueMessageData *message_data);

G_END_DECLS

#endif /* __UNIQUE_MESSAGE_H__ */
