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

#define NO_ASYNC 1000

GtkWidget *junction_widget_new(junction *j);
GtkWidget *matrix_widget_new(gchar *msg, int count);
GtkWidget *main_window_new(GHashTable *table, motorway *mway, gint direction);
GtkWidget *event_widget_new(motorway_event *event, gchar *type, gchar *iconName);
HildonAppMenu *myapp_menu_new(GtkWidget *window, gchar *flipName);
GtkWidget *spacer_new(int x, int y);
GtkWidget *cameras_widget_new(GdkPixbufLoader *loader, junction *j, int index);

gchar *get_delay_from_event_widget(GtkWidget *w);
const gchar *get_desc_from_event_widget(GtkWidget *w);
GtkWidget *get_widget_from_gobject(GObject *object, gchar *name);
GList *gtk_container_get_children_of_mftype(GtkContainer *container, gchar *type);
gint matrix_widget_get_count(GtkWidget *w);
gchar *matrix_widget_get_text(GtkWidget *w);

void sync_matrix_msgs(GtkWidget *junctionWidget, GList *list);
void sync_events(GtkWidget *junctionWidget, gchar *type, gchar *iconName, GList *list);
GtkWidget *find_parent_by_type(GtkWidget *w, gchar *type);
void gtk_widget_set_mftype(GtkWidget *w, gchar *type);
gboolean event_present_in_list(GList *list, gchar *delay, const gchar *desc);
gboolean event_present_in_widgets(GList *ews, gchar *type, gchar *delay, gchar *desc);
GtkWidget *find_matrix_msg_widget(GList *mws, gchar *text, gint count);
void flip_stretch(GtkWidget *window, motorway *mway, GList *stretch, gint direction);
GHashTable *string_list_to_count_hash(GList *list);
void add_matrix_msgs_to_junction_widget(GtkWidget *jw, junction *junction);
void set_title_markup(GtkLabel *label, junction *j);
void change_status_icon(GtkImage *statusIcon, int status);

void updates_applied(int *res, gboolean updated, gpointer *data);
void flipped_data_received(motorway *mway, int *res, gpointer *data);
void camera_update_receive(GdkPixbufLoader *loader, int *res, GtkWidget *jw, gpointer *data, junction *j, int index);
void first_camera_receive(GdkPixbufLoader *loader, int *res, GtkWidget *jw, gpointer *data, junction *j, int index);

gint delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
void event_widget_destroy(GtkWidget *w, gpointer *data);
void matrix_label_destroy(GtkWidget *w, gpointer *data);

void matrix_toggle(GtkWidget *w, gpointer *data);
void camera_changed_cb(HildonTouchSelector *w, gint column, gpointer *user_data);
void camera_up_cb(GtkWidget *w, gpointer *data);
void camera_down_cb(GtkWidget *w, gpointer *data);
void update_data(GtkWidget *w, gpointer *data);
void cam_refresh_cb(GtkWidget *w, gpointer *data);
void flip_cb(GtkWidget *w, gpointer *data);
void cameras_close_cb(GtkWidget *w, gpointer *data);
void camera_select_cb(GtkWidget *w, gpointer *data);

struct flipped_data_received_args
{
  gint direction;
  GHashTable *table;
};
