#ifndef __TEXTEDITORWINDOW_H
#define __TEXTEDITORWINDOW_H

#include <gtk/gtk.h>
#include "texteditor.h"

#define TEXT_EDITOR_WINDOW_TYPE (text_editor_window_get_type())
#define TEXT_EDITOR_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXT_EDITOR_WINDOW_TYPE, TextEditorWindow))

typedef struct _TextEditorWindow TextEditorWindow;
typedef struct _TextEditorWindowClass TextEditorWindowClass;

GType text_editor_window_get_type(void);
TextEditorWindow* text_editor_window_new(TextEditor* app);
void text_editor_window_open(TextEditorWindow* win, GFile* file);

#endif /* __TEXTEDITORWINDOW_H */
