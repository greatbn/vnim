#include <gtk/gtk.h>
#include <gio/gio.h>

int status;

static void
activate (GtkStatusIcon *status_icon,
          gpointer      user_data)
{
    printf("activated\n");
//   GtkWidget *widget;

//   widget = gtk_application_window_new (GTK_APPLICATION (app));
//   gtk_widget_show (widget);
  
//     GNotification *notification;
//   //GIcon *icon;

// notification = g_notification_new ("Test");
// g_notification_set_body (notification, "Hello world");

// g_application_send_notification (app, "test1", notification);
// printf("sended\n");
//   g_application_send_notification (app, NULL, notification);

// g_dbus_connection_flush_sync (g_application_get_dbus_connection (app), NULL, NULL);
// g_object_unref (notification);
//   GtkStatusIcon * statusIcon = gtk_status_icon_new();
//   gtk_status_icon_set_visible(statusIcon, TRUE);
    status = !(status);
    gtk_status_icon_set_from_stock (status_icon,(status)?GTK_STOCK_APPLY:GTK_STOCK_CANCEL);
}

int
main (int argc, char **argv)
{
    gtk_init(&argc, &argv);
//   GtkApplication *app;
//   int status;
GtkStatusIcon * statusIcon = gtk_status_icon_new_from_stock(GTK_STOCK_OK);
    gtk_status_icon_set_visible(statusIcon, TRUE);
//   app = gtk_application_new ("org.gnome.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (statusIcon, "activate", G_CALLBACK (activate), NULL);
  status = 1;

  
//   status = g_application_run (G_APPLICATION (app), argc, argv);
//   g_object_unref (app);

//   return status;
    gtk_main();
}

