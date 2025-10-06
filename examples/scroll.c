/*
gcc `pkg-config --cflags gtk+-3.0` -c scroll.c 
gcc scroll.o `pkg-config --libs gtk+-3.0` -o scroll
*/

#include <gtk/gtk.h>

// Define the column indices for the GtkListStore
enum {
    COL_NAME = 0,
    COL_VALUE,
    NUM_COLS
};

// --- Model and View Creation Functions ---

// Creates a GtkListStore model and fills it with some data
static GtkTreeModel *create_and_fill_model(void) {
    GtkListStore *store;
    GtkTreeIter iter;

    // Create a new list store with two columns: a string and an integer
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_INT);

    // Add some rows to the model
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, COL_NAME, "Item One with a very long name that will require horizontal scrolling", COL_VALUE, 10, -1);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, COL_NAME, "Item Two", COL_VALUE, 20, -1);

    // Add many rows to ensure vertical scrolling is needed
    for (int i = 3; i <= 50; i++) {
        char name[50];
        g_snprintf(name, 50, "Item %d", i);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, COL_NAME, name, COL_VALUE, i * 10, -1);
    }

    return GTK_TREE_MODEL(store);
}

// Creates the GtkTreeView and sets up its columns
static GtkWidget *create_view(GtkTreeModel *model) {
    GtkWidget *treeview;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    treeview = gtk_tree_view_new_with_model(model);

    // Create a cell renderer for text
    renderer = gtk_cell_renderer_text_new();

    // Column 1: Name
    column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", COL_NAME, NULL);
    // Set column sizing to expand, but for horizontal scrolling to appear,
    // the content of the cells must exceed the initial width of the treeview
    // which is set by the window's default size.
    // gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Column 2: Value
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Value", renderer, "text", COL_VALUE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Release the model reference held by create_and_fill_model
    g_object_unref(model);

    return treeview;
}

// --- Main Program ---

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *scrolled_window;
    GtkWidget *treeview;
    GtkTreeModel *model;

    // Initialize GTK+
    gtk_init(&argc, &argv);

    // 1. Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GtkTreeView with Scrollbars");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 2. Create the Scrolled Window
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, // Horizontal Policy
                                   GTK_POLICY_AUTOMATIC); // Vertical Policy
    // Optional: Add a subtle shadow around the scrolled content
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_ETCHED_IN);

    // 3. Create the GtkTreeModel and GtkTreeView
    model = create_and_fill_model();
    treeview = create_view(model);

    // 4. Place the TreeView inside the Scrolled Window
    gtk_container_add(GTK_CONTAINER(scrolled_window), treeview);

    // 5. Place the Scrolled Window inside the main Window
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    // 6. Show all widgets and start the GTK+ main loop
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
