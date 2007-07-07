#ifndef __UNIQUE_BACKEND_H__
#define __UNIQUE_BACKEND_H__

#include <glib-object.h>
#include <gdk/gdkscreen.h>
#include <unique/uniqueapp.h>

G_BEGIN_DECLS

#define UNIQUE_TYPE_BACKEND             (unique_backend_get_type ())
#define UNIQUE_BACKEND(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNIQUE_TYPE_BACKEND, UniqueBackend))
#define UNIQUE_IS_BACKEND(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNIQUE_TYPE_BACKEND))
#define UNIQUE_BACKEND_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), UNIQUE_TYPE_BACKEND, UniqueBackendClass))
#define UNIQUE_IS_BACKEND_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), UNIQUE_TYPE_BACKEND))
#define UNIQUE_BACKEND_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), UNIQUE_TYPE_BACKEND, UniqueBackendClass))

typedef struct _UniqueBackend           UniqueBackend;
typedef struct _UniqueBackendClass      UniqueBackendClass;

struct _UniqueBackend
{
  GObject parent_instance;

  /*< private >*/
  UniqueApp *parent;

  gchar *name;
  gchar *startup_id;

  GdkScreen *screen;
};

struct _UniqueBackendClass
{
  GObjectClass parent_instance;

  /* padding for future expansion */
  void (*_unique_reserved1) (void);
  void (*_unique_reserved2) (void);
  void (*_unique_reserved3) (void);
  void (*_unique_reserved4) (void);
  void (*_unique_reserved5) (void);
  void (*_unique_reserved6) (void);
};

GType                 unique_backend_get_type       (void) G_GNUC_CONST;

G_CONST_RETURN gchar *unique_backend_get_name       (UniqueBackend *backend);
void                  unique_backend_set_name       (UniqueBackend *backend,
                                                     const gchar   *name);
G_CONST_RETURN gchar *unique_backend_get_startup_id (UniqueBackend *backend);
void                  unique_backend_set_startup_id (UniqueBackend *backend,
                                                     const gchar   *startup_id);
GdkScreen *           unique_backend_get_screen     (UniqueBackend *backend);
void                  unique_backend_set_screen     (UniqueBackend *backend,
                                                     GdkScreen     *screen);

G_END_DECLS

#endif /* __UNIQUE_BACKEND_H__ */
