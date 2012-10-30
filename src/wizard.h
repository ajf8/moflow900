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

void prefs_found(motorway *mway, int *res, gpointer *data);
void motorway_select_cb(GtkWidget *w, gpointer *data);
void motorway_list_handler(GHashTable *table, int *res, gpointer *data);
void motorway_select_window_new();
gint wizard_delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data);

struct prefs_found_args
{
  moflow_request *request;
  GHashTable *table;
};
