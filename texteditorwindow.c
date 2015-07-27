#include <gtk/gtk.h>
#include "texteditor.h"
#include "texteditorwindow.h"

struct _TextEditorWindow{
  GtkApplicationWindow parent;
};

struct _TextEditorWindowClass{
  GtkApplicationWindowClass parent_class;
};

typedef struct _TextEditorWindowPrivate TextEditorWindowPrivate;
struct _TextEditorWindowPrivate{
  GSettings* settings;
  GtkWidget* stack;
  GtkWidget* search;
  GtkWidget* searchbar;
  GtkWidget* searchentry;
  GtkWidget* gears;
  GtkWidget* sidebar;
  GtkWidget* words;
  GtkWidget* lines;
  GtkWidget* lines_label;
};

G_DEFINE_TYPE_WITH_PRIVATE(TextEditorWindow, text_editor_window, GTK_TYPE_APPLICATION_WINDOW);

/* NOTE: these function names were hard-coded into window.ui*/
static void search_text_changed(GtkEntry* entry){
  TextEditorWindow* win;
  TextEditorWindowPrivate* priv;
  const gchar* text;
  GtkWidget* tab, *view;
  GtkTextBuffer* buffer;
  GtkTextIter start, match_start, match_end;

  text = gtk_entry_get_text(entry);
  if(text[0] == '\0')
    return;

  win = TEXT_EDITOR_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(entry)));
  priv = text_editor_window_get_instance_private(win);
  tab = gtk_stack_get_visible_child(GTK_STACK(priv->stack));
  view = gtk_bin_get_child(GTK_BIN(tab));
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

  /* le lame search algorithm*/
  gtk_text_buffer_get_start_iter(buffer, &start);
  if(gtk_text_iter_forward_search(&start, text, GTK_TEXT_SEARCH_CASE_INSENSITIVE, &match_start, &match_end, NULL)){
    gtk_text_buffer_select_range(buffer, &match_start, &match_end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &match_start, 0.0, FALSE, 0.0, 0.0);
  }
}

static void find_word(GtkButton* button, TextEditorWindow* win){
  TextEditorWindowPrivate* priv;
  const gchar* word;
  priv = text_editor_window_get_instance_private(win);
  word = gtk_button_get_label(button);
  gtk_entry_set_text(GTK_ENTRY(priv->searchentry), word);
}

static void update_words(TextEditorWindow* win){
  TextEditorWindowPrivate* priv;
  GHashTable* strings;
  GHashTableIter iter;
  GtkWidget* tab, *view, *row;
  GtkTextBuffer* buffer;
  GtkTextIter start, end;
  GList* children, *l;
  gchar* word, *key;

  priv = text_editor_window_get_instance_private(win);
  tab = gtk_stack_get_visible_child(GTK_STACK(priv->stack));
  if(tab == NULL)
    return;
  view = gtk_bin_get_child(GTK_BIN(tab));
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  strings = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  gtk_text_buffer_get_start_iter(buffer, &start);
  while(!gtk_text_iter_is_end(&start)){
    while(!gtk_text_iter_starts_word(&start)){
      if(!gtk_text_iter_forward_char(&start))
	goto done;
    }
    end = start;
    if(!gtk_text_iter_forward_word_end(&end))
      goto done;
    word = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    g_hash_table_add(strings, g_utf8_strdown(word, -1));
    g_free(word);
    start = end;
  }

 done:
  children = gtk_container_get_children(GTK_CONTAINER(priv->words));
  for(l = children; l; l = l->next){
    gtk_container_remove(GTK_CONTAINER(priv->words), GTK_WIDGET(l->data));
  }
  g_list_free(children);

  g_hash_table_iter_init(&iter, strings);
  while(g_hash_table_iter_next(&iter, (gpointer*)&key, NULL)){
    row = gtk_button_new_with_label(key);
    g_signal_connect(row, "clicked", G_CALLBACK(find_word), win);
    gtk_widget_show(row);
    gtk_container_add(GTK_CONTAINER(priv->words), row);
  }
  g_hash_table_unref(strings);
}

static void update_lines(TextEditorWindow* win){
  TextEditorWindowPrivate* priv;
  GtkWidget* tab, *view;
  GtkTextBuffer* buffer;
  GtkTextIter iter;
  int count;
  gchar* lines;

  priv = text_editor_window_get_instance_private(win);
  tab = gtk_stack_get_visible_child(GTK_STACK(priv->stack));
  if(tab == NULL)
    return;
  view = gtk_bin_get_child(GTK_BIN(tab));
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  count = 0;
  gtk_text_buffer_get_start_iter(buffer, &iter);
  while(!gtk_text_iter_is_end(&iter)){
    count++;
    if(!gtk_text_iter_forward_line(&iter))
      break;
  }

  lines = g_strdup_printf("%d", count);
  gtk_label_set_text(GTK_LABEL(priv->lines), lines);
  g_free(lines);
}

static void visible_child_changed(GObject* stack, GParamSpec* pspec){
  TextEditorWindow* win;
  TextEditorWindowPrivate* priv;

  if(gtk_widget_in_destruction(GTK_WIDGET(stack)))
    return;
  win = TEXT_EDITOR_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(stack)));
  priv = text_editor_window_get_instance_private(win);
  gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(priv->searchbar), FALSE);
  update_words(win);
  update_lines(win);
}

static void words_changed(GObject* sidebar, GParamSpec* pspec, TextEditorWindow* win){
  update_words(win);
}

static void text_editor_window_init(TextEditorWindow* win){
  TextEditorWindowPrivate* priv;
  GtkBuilder* builder;
  GMenuModel* menu;
  GAction* action;

  priv = text_editor_window_get_instance_private(win);
  gtk_widget_init_template(GTK_WIDGET(win));
  priv->settings = g_settings_new("texteditor");
  g_settings_bind(priv->settings, "transition", priv->stack, "transition-type", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(priv->settings, "show-words", priv->sidebar, "reveal-child", G_SETTINGS_BIND_DEFAULT);
  g_object_bind_property(priv->search, "active", priv->searchbar, "search-mode-enabled", G_BINDING_BIDIRECTIONAL);
  g_signal_connect(priv->sidebar, "notify::reveal-child", G_CALLBACK(words_changed), win);

  builder = gtk_builder_new_from_resource("/gears-menu.ui");
  menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu"));
  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(priv->gears), menu);
  g_object_unref(builder);

  action = g_settings_create_action(priv->settings, "show-words");
  g_action_map_add_action(G_ACTION_MAP(win), action);
  g_object_unref(action);

  action = (GAction*)g_property_action_new("show-lines", priv->lines, "visible");
  g_action_map_add_action(G_ACTION_MAP(win), action);
  g_object_unref(action);

  g_object_bind_property(priv->lines, "visible", priv->lines_label, "visible", G_BINDING_DEFAULT);
}

static void text_editor_window_dispose(GObject* object){
  TextEditorWindow* win;
  TextEditorWindowPrivate* priv;
  win = TEXT_EDITOR_WINDOW(object);
  priv = text_editor_window_get_instance_private(win);
  g_clear_object(&priv->settings);
  G_OBJECT_CLASS(text_editor_window_parent_class)->dispose(object);
}

static void text_editor_window_class_init(TextEditorWindowClass* class){
  G_OBJECT_CLASS(class)->dispose = text_editor_window_dispose;
  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/window.ui");
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, stack);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, search);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, searchbar);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, searchentry);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, gears);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, words);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, sidebar);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, lines);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), TextEditorWindow, lines_label);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), search_text_changed);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), visible_child_changed);
}

TextEditorWindow* text_editor_window_new(TextEditor* app){
  return g_object_new(TEXT_EDITOR_WINDOW_TYPE, "application", app, NULL);
}

void text_editor_window_open(TextEditorWindow* win, GFile* file){
  TextEditorWindowPrivate* priv;
  gchar* basename;
  GtkWidget* scrolled, *view;
  gchar* contents;
  gsize length;
  GtkTextBuffer* buffer;
  GtkTextTag* tag;
  GtkTextIter start_iter, end_iter;

  priv = text_editor_window_get_instance_private(win);
  basename = g_file_get_basename(file);
  scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolled);
  gtk_widget_set_hexpand(scrolled, TRUE);
  gtk_widget_set_vexpand(scrolled, TRUE);
  view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
  gtk_widget_show(view);
  gtk_container_add(GTK_CONTAINER(scrolled), view);
  gtk_stack_add_titled(GTK_STACK(priv->stack), scrolled, basename, basename);

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

  if(g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)){  
    gtk_text_buffer_set_text(buffer, contents, length);
    g_free(contents);
  }

  tag = gtk_text_buffer_create_tag(buffer, NULL, NULL);
  g_settings_bind(priv->settings, "font", tag, "font", G_SETTINGS_BIND_DEFAULT);
  gtk_text_buffer_get_start_iter(buffer, &start_iter);
  gtk_text_buffer_get_end_iter(buffer, &end_iter);
  gtk_text_buffer_apply_tag(buffer, tag, &start_iter, &end_iter);
  g_free(basename);
  gtk_widget_set_sensitive(priv->search,TRUE);
  update_words(win);
  update_lines(win);
}
