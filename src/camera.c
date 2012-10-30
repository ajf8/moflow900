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

#include <moflow.h>
#include <curl/curl.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

#include "camera.h"

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  GdkPixbufLoader *loader = (GdkPixbufLoader*)data;

  if (gdk_pixbuf_loader_write(loader, (guchar*)ptr, realsize, NULL) == TRUE)
    return realsize;
  else
    return 0;
}

pthread_t camera_get_async(junction *j, int index, GtkWidget *jw, gpointer *data, void(*callback)(GdkPixbufLoader*, int *, GtkWidget*, junction*, int))
{
  pthread_t tid;
  struct camera_request_async *async_req = malloc(sizeof(struct camera_request_async));
  async_req->j = j;
  async_req->jw = jw;
  async_req->data = data;
  async_req->index = index;
  async_req->callback = callback;
  pthread_create(&tid, NULL, camera_get_async_thread, (void*)async_req);
  return tid; 
}

void camera_get_async_thread(void *arg)
{
  struct camera_request_async *async_req = (struct camera_request_async*)arg;
  GList *cameraIdNode = g_list_nth(async_req->j->cameras, async_req->index);
  int *cameraId = (int*)cameraIdNode->data;
  int res;
  GdkPixbufLoader *result = camera_get(*cameraId, &res);
  (async_req->callback)(result, &res, async_req->jw, async_req->data, async_req->j, async_req->index);
  g_free(async_req);
}

GdkPixbufLoader *camera_get(int cameraId, int *res)
{
  *res = -1;
  CURL *curl = curl_easy_init();

  if (curl == NULL)
    return NULL;

  gchar *jpgUrl = g_strdup_printf("http://public.hanet.org.uk/cctvpublicaccess/images/%d.jpg", cameraId);
  gchar *referer = g_strdup_printf("http://public.hanet.org.uk/cctvpublicaccess/html/%d.html", cameraId);
  GdkPixbufLoader *loader = gdk_pixbuf_loader_new_with_type("jpeg", NULL);
 
  curl_easy_setopt(curl, CURLOPT_URL, jpgUrl); 
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)loader);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)");
  curl_easy_setopt(curl, CURLOPT_REFERER, referer);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  
  g_free(referer);
  g_free(jpgUrl);

  if ((*res = curl_easy_perform(curl)) != CURLE_OK || gdk_pixbuf_loader_close(loader, NULL) != TRUE)
  {
    g_object_unref(G_OBJECT(loader));
    loader = NULL;
  }
  
  curl_easy_cleanup(curl);
  
  return loader;
}

