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
#include <gtk/gtk.h>
#include <glib.h>
#include <hildon/hildon.h>
#include <moflow.h>

#include "main.h"
#include "pixbufstore.h"
#include "mainwindow.h"
#include "wizard.h"

void main_exit(int s)
{
  moflow_free(client);
  exit(s);
}

int main(int argc, char *argv[])
{
  g_type_init();
  g_thread_init(NULL);
  gdk_threads_init();

  hildon_gtk_init(&argc, &argv);
  g_set_application_name(MOFLOW_APP_NAME);

  client = init_moflow();
  pixbufStore = pixbuf_store_new();
  pixbuf_store_populate(pixbufStore);

  if (getenv("MOFLOW_USE_SAMPLE") == NULL)
  {
    motorway_select_window_new();
  }
  else
  {
    moflow_request *dummy = client->activereq = moflow_init_request();
    moflow_req_generate_url(dummy);
    //moflow_getactive_async(client, prefs_found, (gpointer*)dummy);
    int res = 0;
    motorway *mway = moflow_getactive(client, &res);
    prefs_found(mway, &res, (gpointer*)dummy);
  }
  
  gtk_main();

  moflow_free(client);
}
