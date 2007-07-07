#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <glib/gi18n.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

#include "uniquebackend.h"
#include "uniqueinternals.h"

G_DEFINE_ABSTRACT_TYPE (UniqueBackend, unique_backend, G_TYPE_OBJECT);

static void
unique_backend_finalize (GObject *gobject)
{
  UniqueBackend *backend = UNIQUE_BACKEND (gobject);

  g_free (backend->name);
  g_free (backend->startup_id);

  G_OBJECT_CLASS (unique_backend_parent_class)->finalize (gobject);
}

static void
unique_backend_class_init (UniqueBackendClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = unique_backend_finalize;
}

static void
unique_backend_init (UniqueBackend *backend)
{
  backend->name = NULL;
  backend->startup_id = NULL;
  backend->screen = gdk_screen_get_default ();
}

/**
 * unique_backend_set_name:
 * @backend: FIXME
 * @name: FIXME
 *
 * FIXME
 */
void
unique_backend_set_name (UniqueBackend *backend,
                         const gchar   *name)
{
  g_return_if_fail (UNIQUE_IS_BACKEND (backend));
  g_return_if_fail (name != NULL);

  if (!backend->name)
    {
      backend->name = g_strdup (name);
      return;
    }

  if (strcmp (backend->name, name) != 0)
    {
      g_free (backend->name);
      backend->name = g_strdup (name);
    }
}

/**
 * unique_backend_get_name:
 * @backend: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
G_CONST_RETURN gchar *
unique_backend_get_name (UniqueBackend *backend)
{
  g_return_val_if_fail (UNIQUE_IS_BACKEND (backend), NULL);

  return backend->name;
}

/**
 * unique_backend_set_startup_id:
 * @backend: FIXME
 * @startup_id: FIXME
 *
 * FIXME
 */
void
unique_backend_set_startup_id (UniqueBackend *backend,
                               const gchar   *startup_id)
{
  g_return_if_fail (UNIQUE_IS_BACKEND (backend));
  g_return_if_fail (startup_id != NULL);

  if (!backend->startup_id)
    {
      backend->startup_id = g_strdup (startup_id);
      return;
    }

  if (strcmp (backend->startup_id, startup_id) != 0)
    {
      g_free (backend->startup_id);
      backend->startup_id = g_strdup (startup_id);
    }
}

/**
 * unique_backend_get_startup_id:
 * @backend: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
G_CONST_RETURN gchar *
unique_backend_get_startup_id (UniqueBackend *backend)
{
  g_return_val_if_fail (UNIQUE_IS_BACKEND (backend), NULL);

  return backend->startup_id;
}

/**
 * unique_backend_set_screen:
 * @backend: FIXME
 * @screen: FIXME
 *
 * FIXME
 */
void
unique_backend_set_screen (UniqueBackend *backend,
                           GdkScreen     *screen)
{
  g_return_if_fail (UNIQUE_IS_BACKEND (backend));
  g_return_if_fail (screen == NULL || GDK_IS_SCREEN (screen));

  if (!screen)
    screen = gdk_screen_get_default ();

  backend->screen = screen;
}

/**
 * unique_backend_get_screen:
 * @backend: FIXME
 *
 * FIXME
 *
 * Return value: FIXME
 */
GdkScreen *
unique_backend_get_screen (UniqueBackend *backend)
{
  g_return_val_if_fail (UNIQUE_IS_BACKEND (backend), NULL);

  return backend->screen;
}
