#include <gtk/gtk.h>
#include "texteditor.h"

int main(int argc, char* argv[]){
  g_setenv("GSETTINGS_SCHEMA_DIR", ".", FALSE);
  return g_application_run(G_APPLICATION(text_editor_new()), argc, argv);
}
