#pragma once
#include <gtk/gtk.h>

#define SPREADSHEET_TYPE_VIEW (spreadsheet_view_get_type())
G_DECLARE_FINAL_TYPE(SpreadsheetView, spreadsheet_view, SPREADSHEET, VIEW,
                     GtkWidget)

GtkWidget* spreadsheet_view_new(void);
