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

GdkPixbufLoader *camera_get(int cameraId, int *res);
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data);
pthread_t camera_get_async(junction *j, int index, GtkWidget *jw, gpointer *data, void(*callback)(GdkPixbufLoader*, int*, GtkWidget*, junction*, int));
void camera_get_async_thread(void *arg);

struct camera_request_async
{
  junction *j;
  int index;
  GtkWidget *jw;
  gpointer *data;
  void(*callback)(GdkPixbufLoader*, int*, GtkWidget*, GtkWidget*, junction*, int);
};
