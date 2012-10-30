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

#include "mainwindow.h"
#include "main.h"
#include "wizard.h"
#include "camera.h"
#include "pixbufstore.h"
#include "util.h"

gint delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
  gtk_main_quit();
  return(FALSE);
}

GtkWidget *event_widget_new(motorway_event *event, gchar *type, gchar *iconName)
{
  GtkWidget *incidentWidget = gtk_hbox_new(FALSE, 2);
  gtk_widget_set_mftype(incidentWidget, type);
  GtkWidget *incidentVbox = gtk_vbox_new(FALSE, 2);
  GtkWidget *icon = GTK_WIDGET(pixbuf_store_to_image(pixbufStore, iconName));

  GtkWidget *incidentLabel = gtk_label_new(NULL);
  gchar *incidentLabelMarkup = g_strdup_printf("<span font_weight=\"bold\" color=\"red\">%s - %s</span>", type, g_markup_escape_text(event->delay, -1));
  gtk_label_set_markup(GTK_LABEL(incidentLabel), incidentLabelMarkup);
  g_object_set_data(G_OBJECT(incidentWidget), "titleLabel", incidentLabel);
  g_object_set_data(G_OBJECT(incidentWidget), "delay", g_strdup(event->delay));
  g_signal_connect(incidentWidget, "destroy", G_CALLBACK(event_widget_destroy), NULL);
  g_free(incidentLabelMarkup);
  gtk_misc_set_alignment(GTK_MISC(incidentLabel), 0, 0);
  gtk_box_pack_start(GTK_BOX(incidentVbox), incidentLabel, TRUE, TRUE, 5);
     
  GtkWidget *desc = gtk_label_new(event->description);
  gtk_misc_set_alignment(GTK_MISC(desc), 0, 0);
  gtk_label_set_line_wrap(GTK_LABEL(desc), TRUE);
  g_object_set_data(G_OBJECT(incidentWidget), "descLabel", desc);
  gtk_box_pack_start(GTK_BOX(incidentVbox), desc, TRUE, TRUE, 5);
  
  gtk_box_pack_start(GTK_BOX(incidentWidget), spacer_new(40, 0), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(incidentWidget), incidentVbox, TRUE, TRUE, 2);
  gtk_misc_set_alignment(GTK_MISC(icon), 0.5, 0.5);
  gtk_box_pack_start(GTK_BOX(incidentWidget), icon, TRUE, FALSE, 2);

  return incidentWidget;
}

GtkWidget *find_parent_by_type(GtkWidget *w, gchar *type)
{
  while ((w = gtk_widget_get_parent(w)) != NULL)
  {
    gpointer *value;
    if ((value = g_object_get_data(G_OBJECT(w), "mftype")) != NULL)
      if (g_strcmp0(type, (gchar*)value) == 0)
        return w;
  }
  return NULL;
}

void gtk_widget_set_mftype(GtkWidget *w, gchar *type)
{
  g_object_set_data(G_OBJECT(w), "mftype", type);
}

void cam_refresh_cb(GtkWidget *w, gpointer *data)
{
  GtkWidget *camerasWidget = find_parent_by_type(w, "camerasWidget");
  GtkWidget *junctionWidget = find_parent_by_type(camerasWidget, "junctionWidget");
  junction *j = g_object_get_data(G_OBJECT(find_parent_by_type(camerasWidget, "junctionWidget")), "junction");
  GtkWidget *image = g_object_get_data(G_OBJECT(camerasWidget), "imageWidget");
  gpointer *camIndex = g_object_get_data(G_OBJECT(junctionWidget), "camIndex");
  camera_get_async(j, GPOINTER_TO_INT(camIndex), image, (gpointer*)camerasWidget, camera_update_receive);
}

GtkWidget *get_widget_from_gobject(GObject *object, gchar *name)
{
  return GTK_WIDGET(g_object_get_data(object, name));
}

void cameras_close_cb(GtkWidget *w, gpointer *data)
{
  GtkWidget *camerasWidget = find_parent_by_type(w, "camerasWidget");
  GtkWidget *openButton = get_widget_from_gobject(G_OBJECT(camerasWidget), "openButton");
  gtk_widget_set_sensitive(openButton, TRUE);
  gtk_widget_destroy(camerasWidget);
}

gboolean event_present_in_list(GList *list, gchar *delay, const gchar *desc)
{
  for (GList *li = list; li != NULL; li = g_list_next(li))
  {
    motorway_event *event = (motorway_event*)li->data;
    if (g_strcmp0(event->delay, delay) == 0 && g_strcmp0(event->description, desc) == 0)
      return TRUE;
  }
  return FALSE;
}

gchar *get_delay_from_event_widget(GtkWidget *w)
{
  return (gchar*)g_object_get_data(G_OBJECT(w), "delay");
}

const gchar *get_desc_from_event_widget(GtkWidget *w)
{
  GtkLabel *descLabel = GTK_LABEL(g_object_get_data(G_OBJECT(w), "descLabel"));
  return gtk_label_get_text(descLabel);
}

gboolean event_present_in_widgets(GList *ews, gchar *type, gchar *delay, gchar *desc)
{
  for (GList *li = ews; li != NULL; li = g_list_next(li))
  {
    GtkWidget *widget = GTK_WIDGET(li->data);
    if (g_strcmp0(get_desc_from_event_widget(widget), desc) == 0 && g_strcmp0(get_delay_from_event_widget(widget), delay) == 0)
      return TRUE;
  }
  return FALSE;
}

void event_widget_destroy(GtkWidget *w, gpointer *data)
{
  g_free(get_delay_from_event_widget(w));
}

gint matrix_widget_get_count(GtkWidget *w)
{
  return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "matrixCount"));
}

gchar *matrix_widget_get_text(GtkWidget *w)
{
  return (gchar*)g_object_get_data(G_OBJECT(w), "matrixText");
}

GtkWidget *find_matrix_msg_widget(GList *mws, gchar *text, gint count)
{
  for (GList *li = mws; li != NULL; li = g_list_next(li))
  {
    GtkWidget *w = (GtkWidget*)li->data;
    if (g_strcmp0(matrix_widget_get_text(w), text) == 0 && count == matrix_widget_get_count(w))
      return w;
  }
  return NULL;
}

void sync_matrix_msgs(GtkWidget *junctionWidget, GList *list)
{
  GList *mws = gtk_container_get_children_of_mftype(GTK_CONTAINER(junctionWidget), "matrixMsgWidget");
  GHashTable *table = string_list_to_count_hash(list);
  for (GList *li = mws; li != NULL; li = g_list_next(li))
  {
    GtkWidget *widget = GTK_WIDGET(li->data);
    gchar *text = matrix_widget_get_text(widget);
    gpointer *count;
    if ((count = g_hash_table_lookup(table, text)) == NULL || GPOINTER_TO_INT(count) != matrix_widget_get_count(widget))
      gtk_widget_destroy(widget);
  }
  g_list_free(mws);
  mws = gtk_container_get_children_of_mftype(GTK_CONTAINER(junctionWidget), "matrixMsgWidget");
  GList *tableKeys = g_hash_table_get_keys(table);
  for (GList *li = tableKeys; li != NULL; li = g_list_next(li))
  {
    gchar *text = (gchar*)li->data;
    gint count = GPOINTER_TO_INT(g_hash_table_lookup(table, li->data));

    if (find_matrix_msg_widget(mws, text, count) == NULL)
    {
      GtkWidget *newWidget = matrix_widget_new(text, count);
      gtk_box_pack_start(GTK_BOX(junctionWidget), newWidget, FALSE, FALSE, 2);
      gtk_widget_show_all(newWidget);
    }
  }
  g_list_free(tableKeys);
  g_list_free(mws);
  g_hash_table_destroy(table);
}

void sync_events(GtkWidget *junctionWidget, gchar *type, gchar *iconName, GList *list)
{
  GList *ews = gtk_container_get_children_of_mftype(GTK_CONTAINER(junctionWidget), type);
  for (GList *li = ews; li != NULL; li = g_list_next(li))
  {
    GtkWidget *widget = GTK_WIDGET(li->data);

    gchar *delay = get_delay_from_event_widget(widget);
    if (event_present_in_list(list, delay, get_desc_from_event_widget(widget)) == FALSE)
    {
      gtk_widget_destroy(widget);
    }
  }
  g_list_free(ews);

  ews = gtk_container_get_children_of_mftype(GTK_CONTAINER(junctionWidget), type);
  for (GList *li = list; li != NULL; li = g_list_next(li))
  {
    motorway_event *event = (motorway_event*)li->data;
    if (event_present_in_widgets(ews, type, event->delay, event->description) == FALSE)
    {
      GtkWidget *newWidget = event_widget_new(event, type, iconName);
      gtk_box_pack_start(GTK_BOX(junctionWidget), newWidget, TRUE, FALSE, 2);
      gtk_box_reorder_child(GTK_BOX(junctionWidget), newWidget, 1);
      gtk_widget_show_all(newWidget);
    }
  }

  g_list_free(ews);
}

void updates_applied(int *res, gboolean updated, gpointer *data)
{ 
  gdk_threads_enter();
  GtkWidget *junctionsContainer = GTK_WIDGET(data);
  hildon_gtk_window_set_progress_indicator(GTK_WINDOW(gtk_widget_get_toplevel(junctionsContainer)), 0);

  if (*res != CURLE_OK)
  {
    moflow_error_dialog(*res, GTK_WINDOW(gtk_widget_get_toplevel(junctionsContainer)), GTK_BUTTONS_OK);
  }
  
  if (updated == TRUE)
  {
    GList *jws = gtk_container_get_children(GTK_CONTAINER(junctionsContainer));
    int i = 0;
    for (GList *li = jws; li != NULL; li = g_list_next(li))
    {
      GtkWidget *junctionWidget = GTK_WIDGET(li->data);
      junction *j = (junction*)g_object_get_data(G_OBJECT(junctionWidget), "junction");
    
      GtkLabel *titleLabel = GTK_LABEL(g_object_get_data(G_OBJECT(junctionWidget), "titleLabel"));
      if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(titleLabel), "mph")) != j->avgSpeed)
        set_title_markup(titleLabel, j);
    
      GtkImage *statusIcon = GTK_IMAGE(g_object_get_data(G_OBJECT(junctionWidget), "statusIcon"));
      if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(statusIcon), "statusIndex")) != j->status)
        change_status_icon(statusIcon, j->status);
   
      sync_events(junctionWidget, "Roadworks", "roadworks", j->roadworks);
      sync_events(junctionWidget, "Incident", "queueing", j->incidents);
      sync_matrix_msgs(junctionWidget, j->matrixMsgs);
      i++;
    }
    g_list_free(jws);
  }
  
  gdk_threads_leave();
}

void update_data(GtkWidget *w, gpointer *data)
{
  motorway *mway = (motorway*)g_object_get_data(G_OBJECT(data), "motorway");
  hildon_gtk_window_set_progress_indicator(GTK_WINDOW(data), 1);
  moflow_update_async(client, mway, updates_applied, g_object_get_data(G_OBJECT(data), "junctionsContainer"));
}

void flipped_data_received(motorway *mway, int *res, gpointer *data)
{
  struct flipped_data_received_args *args = (struct flipped_data_received_args*)data;
  gdk_threads_enter();
  if (*res != CURLE_OK)
  {
    moflow_error_dialog(*res, NULL, GTK_BUTTONS_OK);
  }
  else
  {
    GtkWidget *window = main_window_new(args->table, mway, args->direction);
    gtk_widget_show_all(window);
  }
  g_free(args);
  gdk_threads_leave();
}

void flip_stretch(GtkWidget *window, motorway *mway, GList *stretch, gint direction)
{
  GHashTable *table = (GHashTable*)g_object_get_data(G_OBJECT(window), "mwayTable");
  gtk_widget_destroy(window);
  if (stretch != NULL)
  {
    GtkWidget *newWindow = main_window_new(table, mway, direction);
    gtk_widget_show_all(newWindow);
  }
  else
  {
    client->activereq->direction = direction;
    moflow_req_generate_url(client->activereq);
    g_free(mway);
    struct flipped_data_received_args *args = g_new(struct flipped_data_received_args, 1);
    args->direction = direction;
    args->table = table;
    moflow_getactive_async(client, flipped_data_received, args);
  }
}

void flip_cb(GtkWidget *w, gpointer *data)
{
  GtkWidget *window = GTK_WIDGET(data);
  motorway *mway = (motorway*)g_object_get_data(G_OBJECT(window), "motorway");
  gint direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "direction"));

  if (direction == MF_DIRECTION_RIGHT)
    flip_stretch(window, mway, mway->leftJunctions, MF_DIRECTION_LEFT);
  else if (direction == MF_DIRECTION_LEFT)
    flip_stretch(window, mway, mway->rightJunctions, MF_DIRECTION_RIGHT);
}

GList *gtk_container_get_children_of_mftype(GtkContainer *container, gchar *type)
{
  GList *children = gtk_container_get_children(GTK_CONTAINER(container));
  GList *output = NULL;

  for (GList *li = children; li != NULL; li = g_list_next(li))
  {
    if (g_strcmp0(g_object_get_data(G_OBJECT(li->data), "mftype"), type) == 0)
      output = g_list_append(output, li->data);
  }

  g_list_free(children);
  return output;
}

void matrix_label_destroy(GtkWidget *w, gpointer *data)
{
  g_free(g_object_get_data(G_OBJECT(w), "matrixText"));
}

GHashTable *string_list_to_count_hash(GList *list)
{
  GHashTable *table = g_hash_table_new(g_str_hash, g_str_equal);
  for (GList *li = list; li != NULL; li = g_list_next(li))
  {
    gpointer *data;
    if ((data = g_hash_table_lookup(table, li->data)) != NULL)
      g_hash_table_insert(table, li->data, GINT_TO_POINTER(GPOINTER_TO_INT(data)+1));
    else
      g_hash_table_insert(table, li->data, GINT_TO_POINTER(1));
  }
  return table;
}

GtkWidget *matrix_widget_new(gchar *msg, int count)
{
  GtkWidget *widget = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(widget), spacer_new(40, 0), FALSE, FALSE, 0);
  gchar *markup = g_markup_printf_escaped("<span font_weight=\"bold\" font=\"Monospace\" foreground=\"yellow\">%s</span>", msg);
  if (count > 1)
  {
    gchar *tmp = g_strdup_printf("%s   <span font_size=\"small\">x%d</span>", markup, count);
    g_free(markup);
    markup = tmp;
  }
  GtkWidget *label = gtk_label_new(NULL);
  g_object_set_data(G_OBJECT(widget), "matrixText", g_strdup(msg));
  g_object_set_data(G_OBJECT(widget), "matrixCount", GINT_TO_POINTER(count));
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
  g_signal_connect(widget, "destroy", G_CALLBACK(matrix_label_destroy), NULL);
  gtk_widget_set_mftype(widget, "matrixMsgWidget");
  gtk_label_set_markup(GTK_LABEL(label), markup);
  g_free(markup);
  gtk_box_pack_start(GTK_BOX(widget), label, FALSE, FALSE, 0);
  g_object_set_data(G_OBJECT(widget), "matrixLabel", label);
  return widget;
}

void add_matrix_msgs_to_junction_widget(GtkWidget *jw, junction *junction)
{
  GHashTable *matrixCounts = string_list_to_count_hash(junction->matrixMsgs);
  GList *matrixMsgs = g_hash_table_get_keys(matrixCounts);
  for (GList *lj = matrixMsgs; lj != NULL; lj = g_list_next(lj))
  {
    GtkWidget *newWidget = matrix_widget_new((gchar*)lj->data, GPOINTER_TO_INT(g_hash_table_lookup(matrixCounts, lj->data)));
    gtk_box_pack_start(GTK_BOX(jw), newWidget, FALSE, FALSE, 5);
    gtk_widget_show_all(newWidget);
  }
  g_list_free(matrixMsgs);
  g_hash_table_destroy(matrixCounts);
}

void matrix_toggle(GtkWidget *w, gpointer *data)
{
  GtkWindow *window = GTK_WINDOW(data);
  int showMatrix = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "showMatrix"));
  gtk_button_set_label(GTK_BUTTON(w), showMatrix == 1 ? "Hide Matrix Msgs" : "Show Matrix Msgs");
  g_object_set_data(G_OBJECT(w), "showMatrix", GINT_TO_POINTER(showMatrix == 1 ? 0 : 1));
  GtkVBox *junctionContainer = GTK_VBOX(g_object_get_data(G_OBJECT(window), "junctionsContainer"));
  GList *jws = gtk_container_get_children(GTK_CONTAINER(junctionContainer));
  if (showMatrix == 1)
  {
    for (GList *li = jws; li != NULL; li = g_list_next(li))
    {
      junction *junction = g_object_get_data(G_OBJECT(li->data), "junction");
      add_matrix_msgs_to_junction_widget((GtkWidget*)li->data, junction);
    }
  }
  else
  {
    GList *jws = gtk_container_get_children(GTK_CONTAINER(junctionContainer));
    for (GList *li = jws; li != NULL; li = g_list_next(li))
    {
      GList *matrixMsgs = gtk_container_get_children_of_mftype(GTK_CONTAINER(li->data), "matrixMsgWidget");
      for (GList *lj = matrixMsgs; lj != NULL; lj = g_list_next(lj))
      {
        gtk_widget_destroy(lj->data);
      }
      g_list_free(matrixMsgs);
    }
  }
  g_list_free(jws);
}

void change_cb(GtkWidget *w, gpointer *data)
{
  GtkWidget *window = (GtkWidget*)data;
  GHashTable *table = (GHashTable*)g_object_get_data(G_OBJECT(data), "mwayTable");
  gtk_widget_destroy(window);
  gint res = NO_ASYNC;
  motorway_list_handler(table, &res, NULL);
}

HildonAppMenu *myapp_menu_new(GtkWidget *window, gchar *flipName)
{
  HildonAppMenu* menu = HILDON_APP_MENU(hildon_app_menu_new());

  GtkWidget *flip = gtk_button_new_with_label(flipName);
  g_signal_connect_after(flip, "clicked", G_CALLBACK(flip_cb), window);
  hildon_app_menu_append(menu, GTK_BUTTON(flip));

  GtkWidget *refresh = gtk_button_new_with_label("Refresh");
  g_signal_connect_after(refresh, "clicked", G_CALLBACK(update_data), window);
  hildon_app_menu_append(menu, GTK_BUTTON(refresh));
        
  GtkWidget *matrixMsgs = gtk_button_new_with_label("Hide Matrix Msgs");
  g_signal_connect_after(matrixMsgs, "clicked", G_CALLBACK(matrix_toggle), window);
  g_object_set_data(G_OBJECT(matrixMsgs), "showMatrix", GINT_TO_POINTER(0));
  hildon_app_menu_append(menu, GTK_BUTTON(matrixMsgs));
 
  GtkWidget *change = gtk_button_new_with_label("Change Motorway");
  g_signal_connect_after(change, "clicked", G_CALLBACK(change_cb), window);
  hildon_app_menu_append(menu, GTK_BUTTON(change));
  
  return menu;
}

void camera_changed_cb(HildonTouchSelector *w, gint column, gpointer *user_data)
{
  gint index = hildon_touch_selector_get_active(w, column);
  hildon_touch_selector_center_on_selected(HILDON_TOUCH_SELECTOR(w));
  GtkWidget *junctionWidget = find_parent_by_type(GTK_WIDGET(w), "junctionWidget");
  g_object_set_data(G_OBJECT(junctionWidget), "camIndex", GINT_TO_POINTER(index));
  GtkWidget *camerasWidget = find_parent_by_type(GTK_WIDGET(w), "camerasWidget");
  junction *j = g_object_get_data(G_OBJECT(junctionWidget), "junction");
  GtkWidget *image = g_object_get_data(G_OBJECT(camerasWidget), "imageWidget");
  GtkWidget *upArrow = g_object_get_data(G_OBJECT(camerasWidget), "upButton");
  GtkWidget *downArrow = g_object_get_data(G_OBJECT(camerasWidget), "downButton");
  gtk_widget_set_sensitive(upArrow, index <= 0 ? FALSE : TRUE);
  gtk_widget_set_sensitive(downArrow, index+1 >= g_list_length(j->cameras) ? FALSE : TRUE);
  camera_get_async(j, index, image, (gpointer*)camerasWidget, camera_update_receive);
}

void camera_up_cb(GtkWidget *w, gpointer *data)
{
  HildonTouchSelector *selector = (HildonTouchSelector*)data;
  gint index = hildon_touch_selector_get_active(selector, 0);
  hildon_touch_selector_set_active(selector, 0, index-1);
}

void camera_down_cb(GtkWidget *w, gpointer *data)
{
  HildonTouchSelector *selector = (HildonTouchSelector*)data;
  gint index = hildon_touch_selector_get_active(selector, 0);
  hildon_touch_selector_set_active(selector, 0, index+1);
}

GtkWidget *spacer_new(int x, int y)
{
  GtkWidget *spacer = gtk_label_new(NULL);
  gtk_widget_set_size_request(spacer, x, y);
  return spacer;
}

GtkWidget *cameras_widget_new(GdkPixbufLoader *loader, junction *j, int index)
{
  GtkWidget *widget = gtk_hbox_new(FALSE, 2);
  gtk_widget_set_mftype(widget, "camerasWidget");

  gtk_box_pack_start(GTK_BOX(widget), spacer_new(40, 0), FALSE, FALSE, 0);
    
  GtkWidget *image = gtk_image_new_from_pixbuf(gdk_pixbuf_loader_get_pixbuf(loader));
  gtk_box_pack_start(GTK_BOX(widget), image, FALSE, FALSE, 2);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
    
  GtkWidget *closeButton = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
                                             HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
  hildon_button_set_image(HILDON_BUTTON (closeButton), pixbuf_store_to_image(pixbufStore, "close"));
  gtk_signal_connect(GTK_OBJECT(closeButton), "clicked", GTK_SIGNAL_FUNC(cameras_close_cb), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), closeButton, FALSE, FALSE, 2);
    
  GtkWidget *refreshButton = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
                                                 HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
  hildon_button_set_image(HILDON_BUTTON (refreshButton), pixbuf_store_to_image(pixbufStore, "refresh"));
  gtk_signal_connect(GTK_OBJECT(refreshButton), "clicked", GTK_SIGNAL_FUNC(cam_refresh_cb), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), refreshButton, FALSE, FALSE, 2);
  g_object_set_data(G_OBJECT(widget), "refreshButton", (gpointer*)refreshButton);
  gtk_box_pack_start(GTK_BOX(widget), vbox, FALSE, FALSE, 2);

  if (g_list_length(j->cameras) > 1)
  {
    GtkWidget *selector = hildon_touch_selector_new_text();
    g_object_set_data(G_OBJECT(widget), "imageWidget", (gpointer*)image);
    for (int i = 1; i <= g_list_length(j->cameras); i++)
    {
      gchar *text = g_strdup_printf("%d", i);
      hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), text);
      g_free(text);
    }
    hildon_touch_selector_set_active(HILDON_TOUCH_SELECTOR(selector), 0, index);
    gtk_signal_connect(GTK_OBJECT(selector), "changed", GTK_SIGNAL_FUNC(camera_changed_cb), NULL);
    
    GtkWidget *upButton = hildon_button_new(HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
                                                 HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    gtk_widget_set_sensitive(upButton, FALSE);
    hildon_button_set_image(HILDON_BUTTON(upButton), pixbuf_store_to_image(pixbufStore, "up"));
    gtk_signal_connect(GTK_OBJECT(upButton), "clicked", GTK_SIGNAL_FUNC(camera_up_cb), selector);
    g_object_set_data(G_OBJECT(widget), "upButton", upButton);
      
    GtkWidget *downButton = hildon_button_new(HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
                                                 HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    hildon_button_set_image(HILDON_BUTTON(downButton), pixbuf_store_to_image(pixbufStore, "down"));
    gtk_signal_connect(GTK_OBJECT(downButton), "clicked", GTK_SIGNAL_FUNC(camera_down_cb), selector);
    g_object_set_data(G_OBJECT(widget), "downButton", downButton);
    
    gtk_box_pack_start(GTK_BOX(vbox), upButton, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), downButton, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(widget), selector, TRUE, TRUE, 2);
  }
  
  g_object_unref(loader);
  return widget;
}

void camera_update_receive(GdkPixbufLoader *loader, int *res, GtkWidget *jw, gpointer *data, junction *j, int index)
{
  gdk_threads_enter();
  if (loader != NULL && *res == CURLE_OK)
  {
    GdkPixbuf *newPixBuf = gdk_pixbuf_loader_get_pixbuf(loader);
    gtk_image_set_from_pixbuf(GTK_IMAGE(jw), newPixBuf);
    g_object_unref(loader);
  }
  gdk_threads_leave();
}

void first_camera_receive(GdkPixbufLoader *loader, int *res, GtkWidget *jw, gpointer *data, junction *j, int index)
{
  gdk_threads_enter();
  if (loader != NULL && *res == CURLE_OK)
  {
    GtkWidget *camerasWidget = cameras_widget_new(loader, j, index);
    g_object_set_data(G_OBJECT(camerasWidget), "openButton", data);
    g_object_set_data(G_OBJECT(jw), "camIndex", GINT_TO_POINTER(index));
    gtk_box_pack_start(GTK_BOX(jw), camerasWidget, FALSE, FALSE, 2);
    gtk_box_reorder_child(GTK_BOX(jw), camerasWidget, 1);
    gtk_widget_show_all(camerasWidget);
  }
  gdk_threads_leave();
}

void camera_select_cb(GtkWidget *w, gpointer *data)
{
  gtk_widget_set_sensitive(w, FALSE);
  GtkWidget *junctionWidget = find_parent_by_type(w, "junctionWidget");
  junction *j = (junction*)g_object_get_data(G_OBJECT(junctionWidget), "junction");
  gpointer *lastCam = g_object_get_data(G_OBJECT(junctionWidget), "camIndex");
  camera_get_async(j, lastCam != NULL ? GPOINTER_TO_INT(lastCam) : 0, junctionWidget, (gpointer*)w, first_camera_receive);
}

void set_title_markup(GtkLabel *label, junction *j)
{
  gchar *titleLabelMarkup = g_strdup_printf("<span font_size=\"large\" font_weight=\"bold\">%s: %s</span>", g_markup_escape_text((gchar*)j->junctionId, -1), g_markup_escape_text((gchar*)j->junctionName, -1));

  if (j->avgSpeed > 0)
  {
    gchar *tmp;
/*    if (j->status == 2)
      tmp = g_strdup_printf("%s   <span color=\"red\">%d mph</span>", titleLabelMarkup, j->avgSpeed);
    else if (j->status == 1)
      tmp = g_strdup_printf("%s   <span color=\"yellow\">%d mph</span>", titleLabelMarkup, j->avgSpeed);
    else*/
      tmp = g_strdup_printf("%s   %d mph", titleLabelMarkup, j->avgSpeed);

    g_free(titleLabelMarkup);
    titleLabelMarkup = tmp;
  }

  g_object_set_data(G_OBJECT(label), "mph", GINT_TO_POINTER(j->avgSpeed));
  gtk_label_set_markup(GTK_LABEL(label), titleLabelMarkup);
  g_free(titleLabelMarkup);
}

void change_status_icon(GtkImage *statusIcon, int status)
{
  GdkPixbuf *pixBuf = NULL;
  switch (status)
  {
    case 1:
      pixBuf = pixbuf_store_get(pixbufStore, "orangecircle");
      break;
    case 2:
      pixBuf = pixbuf_store_get(pixbufStore, "itsbroken");
      break;
    default:
      pixBuf = pixbuf_store_get(pixbufStore, "greencircle");
      break;
  }
  gtk_image_set_from_pixbuf(statusIcon, pixBuf);
  g_object_set_data(G_OBJECT(statusIcon), "statusIndex", GINT_TO_POINTER(status));
}

GtkWidget *junction_widget_new(junction *j)
{
  GtkWidget *junctionWidget = gtk_vbox_new(FALSE, 0);
  gtk_widget_set_mftype(junctionWidget, "junctionWidget");
  g_object_set_data(G_OBJECT(junctionWidget), "junction", (gpointer*)j);
  GtkWidget *titleHbox = gtk_hbox_new(FALSE, 0);
  
  GtkWidget *statusIcon = gtk_image_new();
  change_status_icon(GTK_IMAGE(statusIcon), j->status);
  g_object_set_data(G_OBJECT(junctionWidget), "statusIcon", statusIcon);
  gtk_box_pack_start(GTK_BOX(titleHbox), statusIcon, FALSE, FALSE, 0);

  GtkWidget *titleLabel = gtk_label_new(NULL);
  set_title_markup(GTK_LABEL(titleLabel), j);

  g_object_set_data(G_OBJECT(junctionWidget), "titleLabel", titleLabel);
  gtk_misc_set_alignment(GTK_MISC(titleLabel), 0, 0);
  gtk_box_pack_start(GTK_BOX(titleHbox), titleLabel, TRUE, TRUE, 10);
  
  gtk_box_pack_start(GTK_BOX(junctionWidget), titleHbox, TRUE, TRUE, 0);
  
  for (GList *li = j->incidents; li != NULL; li = g_list_next(li))
  {
   motorway_event *event = (motorway_event*)li->data;
   gtk_box_pack_start(GTK_BOX(junctionWidget), event_widget_new(event, "Incident", "queueing"), TRUE, FALSE, 2);
  }

  for (GList *li = j->roadworks; li != NULL; li = g_list_next(li))
  {
   motorway_event *event = (motorway_event*)li->data;
   gtk_box_pack_start(GTK_BOX(junctionWidget), event_widget_new(event, "Roadworks", "roadworks"), TRUE, FALSE, 2);
  }
  
  if (j->cameras != NULL)
  {
    GtkWidget *button = hildon_button_new(HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
    GtkWidget *camButtonBox = gtk_hbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(camButtonBox), pixbuf_store_to_image(pixbufStore, "cctv"), FALSE, FALSE, 2);
    hildon_button_set_style(HILDON_BUTTON(button), HILDON_BUTTON_STYLE_PICKER);

    GtkWidget *camLabel = gtk_label_new(NULL);
    gchar *camLabelMarkup = g_strdup_printf("<span font_size=\"x-small\">%d</span>", g_list_length(j->cameras));
    gtk_label_set_markup(GTK_LABEL(camLabel), camLabelMarkup);
    g_free(camLabelMarkup);
    g_object_set_data(G_OBJECT(button), "camLabel", camLabel);
    gtk_box_pack_start(GTK_BOX(camButtonBox), camLabel, FALSE, FALSE, 1);
    gtk_container_add(GTK_CONTAINER(button), camButtonBox);
    
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(camera_select_cb), NULL);
    gtk_box_pack_start(GTK_BOX(titleHbox), button, FALSE, FALSE, 2);
  }
  
  add_matrix_msgs_to_junction_widget(junctionWidget, j);
  
  return junctionWidget;
}

GtkWidget *main_window_new(GHashTable *table, motorway *mway, gint direction)
{
  GtkWidget *window = hildon_window_new();
  HildonAppMenu *menu = myapp_menu_new(window, direction == MF_DIRECTION_RIGHT ? mway->leftDirection : mway->rightDirection);
  gtk_widget_show_all(GTK_WIDGET(menu));
  hildon_window_set_app_menu(HILDON_WINDOW(window), menu);
  gtk_signal_connect(GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
//  gtk_signal_connect(GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (destroy_event), NULL);

  GtkWidget *pan = hildon_pannable_area_new();
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  g_object_set_data(G_OBJECT(window), "junctionsContainer", vbox);
  g_object_set_data(G_OBJECT(window), "motorway", mway);
  g_object_set_data(G_OBJECT(window), "direction", GINT_TO_POINTER(direction));
  g_object_set_data(G_OBJECT(window), "mwayTable", table);

  GList *stretch;
  gchar *stretchTitle;

  client->activereq->direction = direction;
  moflow_req_generate_url(client->activereq);

  if (direction == MF_DIRECTION_RIGHT)
  {
    stretch = mway->rightJunctions;
    stretchTitle = mway->rightDirection;
  }
  else
  {
    stretch = mway->leftJunctions;
    stretchTitle = mway->leftDirection;
  }
  
  gchar *title = g_strdup_printf("%s %s", mway->roadName, stretchTitle);
  gtk_window_set_title(GTK_WINDOW(window), title);
  g_free(title);

  if (direction == MF_DIRECTION_RIGHT)
  {
    for (GList *li = stretch; li != NULL; li = g_list_next(li))
    {
      junction *j = (junction*)li->data;
      GtkWidget *junctionWidget = junction_widget_new(j);
      gtk_box_pack_start (GTK_BOX (vbox), junctionWidget, FALSE, FALSE, 5);
    }
  }
  else
  {
    for (GList *li = g_list_last(stretch); li != NULL; li = g_list_previous(li))
    {
      junction *j = (junction*)li->data;
      GtkWidget *junctionWidget = junction_widget_new(j);
      gtk_box_pack_start (GTK_BOX (vbox), junctionWidget, FALSE, FALSE, 5);
    }
  }
  
  hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA(pan), vbox);
  gtk_container_add (GTK_CONTAINER (window), pan);
  
  return window;
}


