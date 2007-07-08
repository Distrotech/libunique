#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "uniquemessage-bacon.h"

gchar *
unique_message_data_pack (gint               command_id,
                          UniqueMessageData *message_data,
                          guint              time_,
                          gsize             *length)
{
  return NULL;
}

UniqueMessageData *
unique_message_data_unpack (const gchar *data,
                            gsize        length,
                            gint        *command_id,
                            guint       *time_)
{
  return NULL;
}
