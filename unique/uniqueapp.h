#ifndef __UNIQUE_APP_H__
#define __UNIQUE_APP_H__

#include <glib-object.h>
#include <gtk/gtkwindow.h>
#include <unique/uniquemessage.h>

G_BEGIN_DECLS

#define UNIQUE_TYPE_COMMAND             (unique_command_get_type ())
#define UNIQUE_TYPE_RESPONSE            (unique_response_get_type ())
#define UNIQUE_TYPE_APP                 (unique_app_get_type ())
#define UNIQUE_APP(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNIQUE_TYPE_APP, UniqueApp))
#define UNIQUE_IS_APP(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNIQUE_TYPE_APP))
#define UNIQUE_APP_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), UNIQUE_TYPE_APP, UniqueAppClass))
#define UNIQUE_IS_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), UNIQUE_TYPE_APP))
#define UNIQUE_APP_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), UNIQUE_TYPE_APP, UniqueAppClass))

/**
 * UniqueCommand:
 *
 * Command to send to a currently active instance. User defined commands
 * should be positive integers, and should be added using
 * unique_command_register() before creating the UniqueApp instance.
 *
 * @UNIQUE_INVALID: used internally
 * @UNIQUE_ACTIVATE: request to activate a currently active instance; this
 *   usually means calling gtk_window_present() on the application window.
 * @UNIQUE_NEW: request to create a new file.
 * @UNIQUE_OPEN: request to open a file.
 * @UNIQUE_CLOSE: requests to close the currently running instance.
 */
typedef enum { /*< prefix=UNIQUE >*/
  UNIQUE_INVALID  = 0,
  UNIQUE_ACTIVATE = -1,
  UNIQUE_NEW      = -2,
  UNIQUE_OPEN     = -3,
  UNIQUE_CLOSE    = -4
} UniqueCommand;

GType  unique_command_get_type (void) G_GNUC_CONST;
void   unique_command_register (const gchar *command_name,
                                guint        command_id);

/**
 * UniqueResponse:
 *
 * Response that a currently active instance of the application should
 * return to the caller which sent a command.
 *
 * @UNIQUE_RESPONSE_INVALID: Internal error code, should never be used.
 * @UNIQUE_RESPONSE_OK: The command was successfully executed.
 * @UNIQUE_RESPONSE_CANCEL: The command was cancelled by the user.
 * @UNIQUE_RESPONSE_FAIL: The command failed due to a IPC failure.
 */
typedef enum { /*< prefix=UNIQUE_RESPONSE >*/
  UNIQUE_RESPONSE_INVALID,
  UNIQUE_RESPONSE_OK,
  UNIQUE_RESPONSE_CANCEL,
  UNIQUE_RESPONSE_FAIL
} UniqueResponse;

GType unique_response_get_type (void) G_GNUC_CONST;

typedef struct _UniqueApp               UniqueApp;
typedef struct _UniqueAppPrivate        UniqueAppPrivate;
typedef struct _UniqueAppClass          UniqueAppClass;

struct _UniqueApp
{
  /*< private >*/
  GObject parent_instance;

  UniqueAppPrivate *priv;
};

struct _UniqueAppClass
{
  GObjectClass parent_class;

  UniqueResponse (* message_received) (UniqueApp         *app,
                                       gint               command,
                                       UniqueMessageData *message_data,
                                       guint              time_);
  
  /*< private >*/

  /* padding */
  void (*_unique_reserved1) (void);
  void (*_unique_reserved2) (void);
  void (*_unique_reserved3) (void);
  void (*_unique_reserved4) (void);
};

GType           unique_app_get_type           (void) G_GNUC_CONST;

UniqueApp *    unique_app_new                 (const gchar       *name);
UniqueApp *    unique_app_new_with_startup_id (const gchar       *name,
                                               const gchar       *startup_id);
gboolean       unique_app_is_running          (UniqueApp         *app);
UniqueResponse unique_app_send_message        (UniqueApp         *app,
                                               gint               command_id,
                                               UniqueMessageData *message_data);

G_END_DECLS

#endif /* __UNIQUE_APP_H__ */
