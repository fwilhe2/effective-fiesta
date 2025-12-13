#include "spreadsheet-view.h"

typedef struct
{
  GHashTable* cells;
} Sheet;

struct _SpreadsheetView
{
  GtkWidget parent_instance;

  Sheet* sheet;

  double scroll_x;
  double scroll_y;

  int row_height;
  int col_width;

  guint64 sel_row;
  guint64 sel_col;
};

G_DEFINE_TYPE(SpreadsheetView, spreadsheet_view, GTK_TYPE_WIDGET)
static guint64
cell_key(guint64 row, guint64 col)
{
  return (row << 32) | col;
}

static void
sheet_set_cell(Sheet* sheet, guint64 row, guint64 col, const char* text)
{
  g_hash_table_insert(sheet->cells, GUINT_TO_POINTER(cell_key(row, col)),
                      g_strdup(text));
}

static const char*
sheet_get_cell(Sheet* sheet, guint64 row, guint64 col)
{
  return g_hash_table_lookup(sheet->cells,
                             GUINT_TO_POINTER(cell_key(row, col)));
}

static void
spreadsheet_view_init(SpreadsheetView* self)
{
  self->row_height = 24;
  self->col_width = 100;

  self->sheet = g_new0(Sheet, 1);
  self->sheet->cells
      = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

  sheet_set_cell(self->sheet, 0, 0, "Hello");
  sheet_set_cell(self->sheet, 5, 5, "Hello");
}

static void
draw_cell(SpreadsheetView* view, GtkSnapshot* snapshot, guint64 row,
          guint64 col)
{
  float x = col * view->col_width - view->scroll_x;
  float y = row * view->row_height - view->scroll_y;

  graphene_rect_t rect;
  graphene_rect_init(&rect, x, y, view->col_width, view->row_height);

  // Background
  gtk_snapshot_append_color(snapshot, &(GdkRGBA){ 1, 1, 1, 1 }, &rect);

  // Selection
  if (row == view->sel_row && col == view->sel_col)
    {
      gtk_snapshot_append_color(snapshot, &(GdkRGBA){ 0.3, 0.5, 0.9, 0.3 },
                                &rect);
    }

  // Text
  const char* text = sheet_get_cell(view->sheet, row, col);
  if (text)
    {
      PangoLayout* layout
          = gtk_widget_create_pango_layout(GTK_WIDGET(view), text);

      GdkRGBA black = { 0, 0, 0, 1 };

      gtk_snapshot_append_layout(snapshot, layout, &black);

      g_object_unref(layout);
    }
}

static void
spreadsheet_view_snapshot(GtkWidget* widget, GtkSnapshot* snapshot)
{
  SpreadsheetView* view = SPREADSHEET_VIEW(widget);

  int width = gtk_widget_get_width(widget);
  int height = gtk_widget_get_height(widget);

  guint first_row = view->scroll_y / view->row_height;
  guint first_col = view->scroll_x / view->col_width;

  guint rows = height / view->row_height + 2;
  guint cols = width / view->col_width + 2;

  for (guint r = 0; r < rows; r++)
    for (guint c = 0; c < cols; c++)
      draw_cell(view, snapshot, first_row + r, first_col + c);
}

static void
on_click(GtkGestureClick* gesture, int n_press, double x, double y,
         gpointer user_data)
{
  SpreadsheetView* view = user_data;

  view->sel_row = (y + view->scroll_y) / view->row_height;
  view->sel_col = (x + view->scroll_x) / view->col_width;

  gtk_widget_queue_draw(GTK_WIDGET(view));
}

static void
spreadsheet_view_class_init(SpreadsheetViewClass* klass)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(klass);

  widget_class->snapshot = spreadsheet_view_snapshot;

  gtk_widget_class_set_css_name(widget_class, "spreadsheet");
}

GtkWidget*
spreadsheet_view_new(void)
{
  SpreadsheetView* view = g_object_new(SPREADSHEET_TYPE_VIEW, NULL);

  GtkGesture* click = gtk_gesture_click_new();
  g_signal_connect(click, "pressed", G_CALLBACK(on_click), view);
  gtk_widget_add_controller(GTK_WIDGET(view), GTK_EVENT_CONTROLLER(click));

  return GTK_WIDGET(view);
}
