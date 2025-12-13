#include <gtk/gtk.h>
#include "spreadsheet-view.h"

static void
activate(GtkApplication* app)
{
  GtkWidget* window = gtk_application_window_new(app);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  GtkWidget* scroller = gtk_scrolled_window_new();

  GtkWidget* sheet = spreadsheet_view_new();

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), sheet);

  gtk_window_set_child(GTK_WINDOW(window), scroller);
  gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char** argv)
{
  GtkApplication* app
      = gtk_application_new("demo.spreadsheet", G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  return g_application_run(G_APPLICATION(app), argc, argv);
}
