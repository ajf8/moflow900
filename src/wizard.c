// moflow900
// Copyright (C) 2009 Alan F
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdlib.h>
#include <glib-object.h>
#include <hildon/hildon.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <moflow.h>

#include "main.h"
#include "wizard.h"
#include "mainwindow.h"
#include "util.h"

gint wizard_delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
  main_exit(0);
  return FALSE;
}

void direction_select_cb(GtkWidget *w, gpointer *data)
{
  motorway *mway = (motorway*)data;
  gint direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "direction"));
  GHashTable *table = g_object_get_data(G_OBJECT(w), "mwayTable");
  GtkWidget *mainWindow = main_window_new(table, mway, direction);
  gtk_widget_show_all(mainWindow);

  GtkWidget *window = gtk_widget_get_toplevel(w);
  gtk_widget_destroy(window);
}
  
GtkWidget *direction_widget_new(GHashTable *table, motorway *mway, int direction)
{
  GtkWidget *directionWidget = gtk_hbox_new(FALSE, 2);
  GtkWidget *arrow;
  
  if (direction == MF_DIRECTION_LEFT)
    arrow = gtk_image_new_from_file(IMAGES_DIR"/arrowup.png");
  else
    arrow = gtk_image_new_from_file(IMAGES_DIR"/arrowdown.png");
  
  gtk_box_pack_start(GTK_BOX(directionWidget), arrow, FALSE, FALSE, 0);

  GtkWidget *label = gtk_label_new(NULL);
  gchar *markup = g_markup_printf_escaped("<span font_size=\"xx-large\" font_weight=\"ultrabold\">%s %s</span>", mway->roadName, direction == MF_DIRECTION_LEFT ? mway->leftDirection : mway->rightDirection);
  gtk_label_set_markup(GTK_LABEL(label), markup);
  g_free(markup);
  gtk_box_pack_start(GTK_BOX(directionWidget), label, TRUE, TRUE, 0);
  
  GtkWidget *button = hildon_button_new(HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  hildon_button_set_style(HILDON_BUTTON(button), HILDON_BUTTON_STYLE_PICKER);
  hildon_button_set_image (HILDON_BUTTON (button), arrow);
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(direction_select_cb), mway);
  g_object_set_data(G_OBJECT(button), "mwayTable", table);
  g_object_set_data(G_OBJECT(button), "direction", GINT_TO_POINTER(direction));
  gtk_box_pack_start(GTK_BOX(directionWidget), button, FALSE, FALSE, 20);

  return directionWidget;
}

void prefs_found(motorway *mway, int *res, gpointer *data)
{
  struct prefs_found_args *args = (struct prefs_found_args*)data;
  
  gdk_threads_enter();
  if (mway == NULL)
  {
    moflow_error_dialog(*res, NULL, GTK_BUTTONS_OK);
    main_exit(1);
  }
  client->activereq = args->request;
  
  GtkWidget *window = GTK_WIDGET(hildon_window_new());
  gtk_signal_connect(GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (wizard_delete_event), NULL);
  gtk_window_set_title(GTK_WINDOW(window), mway->roadName);

  GtkWidget *vbox = GTK_WIDGET(gtk_vbox_new(FALSE, 20));
  
  gtk_box_pack_start(GTK_BOX (vbox), direction_widget_new(args->table, mway, MF_DIRECTION_LEFT), TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX (vbox), direction_widget_new(args->table, mway, MF_DIRECTION_RIGHT), TRUE, TRUE, 10);
  
  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show_all(window);
  gdk_threads_leave();
  g_free(args);
}

void motorway_select_cb(GtkWidget *w, gpointer *data)
{
  GHashTable *table = (GHashTable*)g_object_get_data(G_OBJECT(w), "motorwayTable");
  HildonTouchSelector *selector = HILDON_TOUCH_SELECTOR(g_object_get_data(G_OBJECT(w), "motorwaySelector"));
  gchar *selectionText = hildon_touch_selector_get_current_text(selector);
  int *selectionId = g_hash_table_lookup(table, (gpointer*)selectionText);
  
  struct prefs_found_args *args = g_new(struct prefs_found_args, 1);
  moflow_request *req = moflow_init_request();
  req->motorway_id = *selectionId;
  moflow_req_generate_url(req);
  args->table = table;
  args->request = req;
  moflow_get_async(client, req, prefs_found, (gpointer*)args);

  GtkWidget *window = gtk_widget_get_toplevel(w);
  gtk_widget_destroy(window);
}

gint motorwaycmp(gpointer *a, gpointer *b)
{
  gint ai, bi;
  if (sscanf((gchar*)a, "M%d", &ai) < 1)
    return -1;
  if (sscanf((gchar*)b, "M%d", &bi) < 1)
    return 1;

  if (ai < bi)
    return -1;
  else if (ai > bi)
    return 1;
  else
    return 0;
}

void selection_changed(HildonTouchSelector *selector, gint column, gpointer *data)
{
  gtk_widget_set_sensitive(GTK_WIDGET(data), hildon_touch_selector_get_active(selector, column) >= 0 ? TRUE : FALSE);
}
	
void motorway_list_handler(GHashTable *table, int *res, gpointer *data)
{
  if (*res != NO_ASYNC)
   gdk_threads_enter();
  
  GtkWidget *window = GTK_WIDGET( hildon_window_new() );
  gtk_signal_connect(GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (wizard_delete_event), NULL);
  gtk_window_set_title(GTK_WINDOW(window), "Pick a motorway");

  if (table == NULL)
  {
    moflow_error_dialog(*res, NULL, GTK_BUTTONS_OK);
    main_exit(1);
  }
  
  GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
  GtkWidget *selector = hildon_touch_selector_new_text();
  
  GList *sortedList = g_list_sort(g_hash_table_get_keys(table), (GCompareFunc)motorwaycmp);
  for (GList *li = sortedList; li != NULL; li = g_list_next(li))
  {
    gchar *key = (gchar*)li->data;
    hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), key);
  }
  g_list_free(sortedList);
  gtk_box_pack_start(GTK_BOX(vbox), selector, TRUE, TRUE, 2);

  GtkWidget *button = hildon_button_new_with_text(HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_VERTICAL, "Go!", NULL);
  gtk_widget_set_sensitive(button, FALSE);
  g_signal_connect(G_OBJECT(selector), "changed", G_CALLBACK(selection_changed), button);
  g_object_set_data(G_OBJECT(button), "motorwayTable", table);
  g_object_set_data(G_OBJECT(button), "motorwaySelector", selector);
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(motorway_select_cb), NULL);
  gtk_widget_set_size_request(button, -1, 70);
  gtk_box_pack_end(GTK_BOX(vbox), button, FALSE, FALSE, 2);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 2);
  
  GtkWidget *logo = gtk_image_new_from_file("/usr/share/pixmaps/moflow/motorway.png");
  gtk_box_pack_end(GTK_BOX(hbox), logo, FALSE, FALSE, 20);
  
  gtk_container_add(GTK_CONTAINER(window), hbox);
  gtk_widget_show_all(window);

  if (*res != NO_ASYNC)
    gdk_threads_leave();
}


void motorway_select_window_new()
{
  motorway_list_async(client, motorway_list_handler, NULL);
}
