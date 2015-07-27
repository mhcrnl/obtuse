#ifndef __TEXTEDITORPREFS_H
#define __TEXTEDITORPREFS_H

#include <gtk/gtk.h>
#include "texteditorwindow.h"

#define TEXT_EDITOR_PREFS_TYPE (text_editor_prefs_get_type())
#define TEXT_EDITOR_PREFS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXT_EDITOR_PREFS_TYPE, TextEditorPrefs))

typedef struct _TextEditorPrefs TextEditorPrefs;
typedef struct _TextEditorPrefsClass TextEditorPrefsClass;

GType text_editor_prefs_get_type(void);
TextEditorPrefs* text_editor_prefs_new(TextEditorWindow* win);

#endif /* __TEXTEDITORPREFS_H */
