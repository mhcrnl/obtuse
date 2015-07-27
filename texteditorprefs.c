#include <gtk/gtk.h>
#include "texteditor.h"
#include "texteditorwindow.h"
#include "texteditorprefs.h"

struct _TextEditorPrefs{
  GtkDialog parent;
};

struct _TextEditorPrefsClass{
  GtkDialogClass parent_class;
};

typedef struct _TextEditorPrefsPrivate TextEditorPrefsPrivate;
struct _TextEditorPrefsPrivate{
  GSettings* settings;
  GtkWidget* font;
  GtkWidget* transition;
};

G_DEFINE_TYPE_WITH_PRIVATE(TextEditorPrefs, text_editor_prefs, GTK_TYPE_DIALOG)

static void text_editor_prefs_init(TextEditorPrefs* prefs){
  TextEditorPrefsPrivate* priv = text_editor_prefs_get_instance_private(prefs);
  gtk_widget_init_template(GTK_WIDGET(prefs));
  priv->settings = g_settings_new("texteditor");
  g_settings_bind(priv->settings, "font", priv->font, "font", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(priv->settings, "transition", priv->transition, "active-id", G_SETTINGS_BIND_DEFAULT);
}

static void text_editor_prefs_dispose(GObject* object){
  TextEditorPrefsPrivate* priv = text_editor_prefs_get_instance_private(TEXT_EDITOR_PREFS(object));
  g_clear_object(&priv->settings);
  G_OBJECT_CLASS(text_editor_prefs_parent_class)->dispose(object);
}

static void text_editor_prefs_class_init(TextEditorPrefsClass* class){
  G_OBJECT_CLASS(class)->dispose = text_editor_prefs_dispose;
  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/prefs.ui");
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorPrefs, font);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorPrefs, transition);
}

TextEditorPrefs* text_editor_prefs_new(TextEditorWindow* win){
  return g_object_new(TEXT_EDITOR_PREFS_TYPE, "transient-for", win, "use-header-bar", TRUE, NULL);
}
