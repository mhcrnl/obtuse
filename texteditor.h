#ifndef __TEXTEDITOR_H
#define __TEXTEDITOR_H

#include <gtk/gtk.h>

#define TEXT_EDITOR_TYPE (text_editor_get_type())
#define TEXT_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXT_EDITOR_TYPE, TextEditor))

typedef struct _TextEditor TextEditor;
typedef struct _TextEditorClass TextEditorClass;

GType text_editor_get_type(void);
TextEditor* text_editor_new(void);

#endif /* __TEXTEDITOR_H */
