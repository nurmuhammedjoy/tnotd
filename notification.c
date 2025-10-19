#include <gtk/gtk.h>
#include <stdlib.h>

#define NOTIFICATION_WIDTH 250
#define NOTIFICATION_HEIGHT 80
#define NOTIFICATION_IMAGE_SIZE 60
#define NOTIFICATION_TIMEOUT 10
#define NOTIFICATION_MARGIN 10
#define NOTIFICATION_TOP_OFFSET 33

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Escape) {
        gtk_main_quit();
        return TRUE;
    }
    return FALSE;
}

static GtkWidget *create_notification(const char *title, const char *message, const char *image_path) {
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Notification");
    gtk_window_set_default_size(GTK_WINDOW(window), NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);  
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_NOTIFICATION);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_widget_set_name(window, "notification-window");

    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (!monitor) {
        monitor = gdk_display_get_monitor(display, 0);
    }
    
    if (!monitor) {
        g_printerr("Error: No monitor detected.\n");
        gtk_widget_destroy(window);
        return NULL;
    }

    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);
    int pos_x = geometry.x + geometry.width - NOTIFICATION_WIDTH - NOTIFICATION_MARGIN;
    int pos_y = geometry.y + NOTIFICATION_TOP_OFFSET;
    gtk_window_move(GTK_WINDOW(window), pos_x, pos_y);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
  
    GtkWidget *image;
    if (image_path && g_file_test(image_path, G_FILE_TEST_EXISTS)) {
        GError *error = NULL;
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(image_path, NOTIFICATION_IMAGE_SIZE, NOTIFICATION_IMAGE_SIZE, TRUE, &error);
        if (error) {
            g_printerr("Error loading image: %s\n", error->message);
            g_error_free(error);
            image = gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_DIALOG);
            gtk_image_set_pixel_size(GTK_IMAGE(image), NOTIFICATION_IMAGE_SIZE);
        } else {
            image = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref(pixbuf);
        }
    } else {
        image = gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_DIALOG);
        gtk_image_set_pixel_size(GTK_IMAGE(image), NOTIFICATION_IMAGE_SIZE);
    }
    
    GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    
    GtkWidget *title_label = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<b>%s</b>", title);
    gtk_label_set_markup(GTK_LABEL(title_label), markup);
    g_free(markup);
    gtk_widget_set_halign(title_label, GTK_ALIGN_START);
    
    GtkWidget *message_label = gtk_label_new(message);
    gtk_label_set_line_wrap(GTK_LABEL(message_label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(message_label), 30);
    gtk_widget_set_halign(message_label, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(text_box), title_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(text_box), message_label, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(main_box), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), text_box, TRUE, TRUE, 5);
    
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    
    g_timeout_add_seconds(NOTIFICATION_TIMEOUT, (GSourceFunc)gtk_main_quit, NULL);
    
    return window;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    const char *title = "Title/Songs name";
    const char *message = "It just a description\n......";
    const char *image_path = NULL;  
    
    if (argc > 1) title = argv[1];
    if (argc > 2) message = argv[2];
    if (argc > 3) image_path = argv[3];
    
    GtkWidget *notification = create_notification(title, message, image_path);
    
    if (!notification) {
        return 1;
    }
    
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    gtk_css_provider_load_from_data(provider,
        "#notification-window { background-color: #3C3836; }"
        "label { color: #ffffff; }", -1, &error);
    
    if (error) {
        g_printerr("Error loading CSS: %s\n", error->message);
        g_error_free(error);
    }
    
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    g_object_unref(provider);
    
    gtk_widget_show_all(notification);
    gtk_main();
    
    return 0;
}
