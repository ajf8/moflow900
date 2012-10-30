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

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <moflow.h>

#include "main.h"
#include "pixbufstore.h"

void pixbuf_store_populate(GHashTable *table)
{
  pixbuf_store_add(table, "cctv", IMAGES_DIR"/cctv.png");
  pixbuf_store_add(table, "close", IMAGES_DIR"/close.png");
  pixbuf_store_add(table, "greencircle", IMAGES_DIR"/greencircle.png");
  pixbuf_store_add(table, "itsbroken", IMAGES_DIR"/itsbroken.png");
  pixbuf_store_add(table, "orangecircle", IMAGES_DIR"/orangecircle.png");
  pixbuf_store_add(table, "queueing", IMAGES_DIR"/queueing.png");
  pixbuf_store_add(table, "refresh", IMAGES_DIR"/refresh.png");
  pixbuf_store_add(table, "roadworks", IMAGES_DIR"/roadworks.png");
  pixbuf_store_add(table, "up", IMAGES_DIR"/up.png");
  pixbuf_store_add(table, "down", IMAGES_DIR"/down.png");
}

GHashTable *pixbuf_store_new()
{
  return g_hash_table_new(g_str_hash, g_str_equal);
}

void pixbuf_store_add(GHashTable* table, gchar *name, gchar *file)
{
  g_hash_table_insert(table, name, gdk_pixbuf_new_from_file(file, NULL));
}

GtkWidget *pixbuf_store_to_image(GHashTable *table, gchar *name)
{
  gpointer *result = g_hash_table_lookup(table, name);
  if (result != NULL)
    return GTK_WIDGET(gtk_image_new_from_pixbuf((GdkPixbuf*)result));

  return NULL;
}

GdkPixbuf *pixbuf_store_get(GHashTable *table, gchar *name)
{
  return (GdkPixbuf*)g_hash_table_lookup(table, name);
}

