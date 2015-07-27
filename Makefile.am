GTK_CFLAGS = -I$(top_srcdir) $(GTK_DEP_CFLAGS)

GTK_LIBS = $(top_builddir)/gtk/libgtk-3.la \
           $(top_builddir)/gtk/libgdk-3.la \
	   $(GTK_DEP_LIBS)

noinst_PROGRAMS = texteditor

texteditor_CFLAGS = $(GTK_CFLAGS)
texteditor_LDADD = $(GTK_LIBS)
texteditor_SOURCES = \
	main.c       \
	texteditor.c texteditor.h \
	texteditorwindow.c texteditorwindow.h \
	texteditorprefs.c texteditorprefs.h \
	resources.c

BUILT_SOURCES = resources.c gschemas.compiled

resources.c: texteditor.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/texteditor.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $(srcdir)/texteditor.gresource.xml --target=$@ --sourcedir=$(srcdir) --generate-source

gsettings_SCHEMAS = texteditor.gschema.xml

@GSETTINGS_RULES@

gschemas.compiled: texteditor.gschema.xml
	$(GLIB_COMPILE_SCHEMAS)

EXTRA_DIST = window.ui app-menu.ui prefs.ui gears-menu.ui \
	     texteditor.gresource.xml texteditor.gschema.xml

CLEANFILES = gschemas.compiled

