#ifndef __UNIQUE_INTERNALS_H__
#define __UNIQUE_INTERNALS_H__

#include <time.h>

#include "uniqueapp.h"
#include "uniquebackend.h"
#include "uniquemessage.h"
#include "uniqueversion.h"

G_BEGIN_DECLS

typedef enum {
  UNIQUE_DEBUG_MISC    = 1 << 0,
  UNIQUE_DEBUG_BACKEND = 1 << 1,
  UNIQUE_DEBUG_APP     = 1 << 2,
  UNIQUE_DEBUG_MESSAGE = 1 << 3
} UniqueDebugFlags;

#ifdef UNIQUE_ENABLE_DEBUG

#define UNIQUE_NOTE(type,x,a...)                G_STMT_START {  \
        if (unique_debug_flags & UNIQUE_DEBUG_##type) {         \
          g_message ("[" #type "]: " G_STRLOC ": " x, ##a);     \
        }                                       } G_STMT_END

#else

#define UNIQUE_NOTE(type,x,a...)

#endif /* UNIQUE_ENABLE_DEBUG */

extern guint unique_debug_flags;

struct _UniqueMessageData
{
  guchar *data;
  gint length;

  GdkScreen *screen;
  gchar *startup_id;
  guint workspace;
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

/* this method emits the UniqueApp::message-received signal on app; it
 * should be called by the backend on its parent UniqueApp instance.
 */
UniqueResponse unique_app_emit_message_received (UniqueApp         *app,
                                                 gint               command_id,
                                                 UniqueMessageData *message,
                                                 guint              time_);

/* transform a command or a response id to something more readable,
 * and then back into an id
 */
UniqueResponse        unique_response_from_string  (const gchar    *response);
G_CONST_RETURN gchar *unique_response_to_string    (UniqueResponse  response);

gint                  unique_command_from_string   (UniqueApp      *app,
                                                    const gchar    *command);
G_CONST_RETURN gchar *unique_command_to_string     (UniqueApp      *app,
                                                    gint            command);

G_END_DECLS

#endif /* __UNIQUE_INTERNALS_H__ */
