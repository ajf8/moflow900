#include <gtk/gtk.h>
#include <moflow.h>

#include "util.h"

gint moflow_error_dialog(gint res, GtkWindow *parent, GtkButtonsType buttons)
{
  gchar *errorMsg = moflow_easy_strerror(res);
  GtkWidget *dialog = gtk_message_dialog_new(parent, parent != NULL ? GTK_DIALOG_MODAL : GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, buttons, errorMsg);
  gint response = gtk_dialog_run(GTK_DIALOG(dialog));
  g_free(errorMsg);
  return response;
}
