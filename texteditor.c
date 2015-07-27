#include <gtk/gtk.h>
#include "texteditor.h"
#include "texteditorwindow.h"
#include "texteditorprefs.h"

struct _TextEditor{
  GtkApplication parent;
};

struct _TextEditorClass{
  GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(TextEditor, text_editor, GTK_TYPE_APPLICATION);

static void text_editor_init(TextEditor* app){
  
}

/* A "*_activated" method for each item in app-menu.ui*/
static void preferences_activated(GSimpleAction* action, GVariant* parameter, gpointer app){
  TextEditorPrefs* prefs;
  GtkWindow* win;
  win = gtk_application_get_active_window(GTK_APPLICATION(app));
  prefs = text_editor_prefs_new(TEXT_EDITOR_WINDOW(win));
  gtk_window_present(GTK_WINDOW(prefs));
}

static void quit_activated(GSimpleAction* action, GVariant* parameter, gpointer app){
  g_application_quit(G_APPLICATION(app));
}

static GActionEntry app_entries[]= {
  {"preferences", preferences_activated, NULL, NULL, NULL},
  {"quit", quit_activated, NULL, NULL, NULL}
};

static void text_editor_startup(GApplication* app){
  GtkBuilder* builder;
  GMenuModel* app_menu;
  const gchar* quit_accels[2] = {"<Ctrl>Q", NULL};

  G_APPLICATION_CLASS(text_editor_parent_class)->startup(app);
  g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
  gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit", quit_accels);
  builder = gtk_builder_new_from_resource("/app-menu.ui");
  app_menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
  gtk_application_set_app_menu(GTK_APPLICATION(app), app_menu);
  g_object_unref(builder);
}

static void text_editor_activate(GApplication* app){
  TextEditorWindow* win = text_editor_window_new(TEXT_EDITOR(app));
  gtk_window_present(GTK_WINDOW(win));
}

static void text_editor_open(GApplication* app, GFile** files, gint n_files, const gchar* hint){
  GList* windows;
  TextEditorWindow* win;
  int i;

  windows = gtk_application_get_windows(GTK_APPLICATION(app));
  if(windows)
    win = TEXT_EDITOR_WINDOW(windows->data);
  else
    win = text_editor_window_new(TEXT_EDITOR(app));

  for(i = 0; i < n_files; i++)
    text_editor_window_open(win, files[i]);

  gtk_window_present(GTK_WINDOW(win));
}

static void text_editor_class_init(TextEditorClass* class){
  G_APPLICATION_CLASS(class)->startup = text_editor_startup;
  G_APPLICATION_CLASS(class)->activate = text_editor_activate;
  G_APPLICATION_CLASS(class)->open = text_editor_open;
}

TextEditor* text_editor_new(){
  return g_object_new(TEXT_EDITOR_TYPE, "application-id", "mhk98.gtk.texteditor","flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
