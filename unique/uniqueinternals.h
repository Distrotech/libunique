#ifndef __UNIQUE_INTERNALS_H__
#define __UNIQUE_INTERNALS_H__

#include <time.h>

#include "uniqueapp.h"
#include "uniquebackend.h"
#include "uniquemessage.h"

G_BEGIN_DECLS

struct _UniqueMessageData
{
  guchar *data;
  gint length;

  GdkScreen *screen;
  gchar *startup_id;
};

/* GObject ought to export symbols like these */
#define UNIQUE_PARAM_READABLE   (G_PARAM_READABLE | \
                                 G_PARAM_STATIC_NAME | \
                                 G_PARAM_STATIC_NICK | \
                                 G_PARAM_STATIC_BLURB)
#define UNIQUE_PARAM_READWRITE  (G_PARAM_READABLE | G_PARAM_WRITABLE | \
                                 G_PARAM_STATIC_NAME | \
                                 G_PARAM_STATIC_NICK | \
                                 G_PARAM_STATIC_BLURB)

/* these methods _must_ be implemented by the backends */
GType          unique_backend_impl_get_type (void) G_GNUC_CONST;
gboolean       unique_backend_request_name  (UniqueBackend     *backend);
UniqueResponse unique_backend_send_message  (UniqueBackend     *backend,
                                             gint               command_id,
                                             UniqueMessageData *message,
                                             guint              time_);

/* this method emits the UniqueApp::message-received signal on app; it
 * should be called by the backend on its parent UniqueApp instance.
 */
UniqueResponse unique_app_emit_message      (UniqueApp         *app,
                                             gint               command_id,
                                             UniqueMessageData *message,
                                             guint              time_);

/* transform a command or a response id to something more readable,
 * and then back into an id
 */
UniqueResponse        unique_response_from_string  (const gchar    *response);
G_CONST_RETURN gchar *unique_response_to_string    (UniqueResponse  response);
gint                  unique_command_from_string   (const gchar    *command);
G_CONST_RETURN gchar *unique_command_to_string     (gint            command);

G_END_DECLS

#endif /* __UNIQUE_INTERNALS_H__ */
