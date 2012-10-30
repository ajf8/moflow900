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
#include <pthread.h>
#include <glib-object.h>

#include "moflow.h"

void mcallback(motorway* mway, gpointer *data)
{
  GList *junctions = mway->junctionNames;

  printf("motorway: %s\n", mway->roadName);
  GList *left = mway->leftJunctions;
  
  printf("leftDirection: %s (%d)\n", mway->leftDirection, g_list_length(left));

  int i = 0;
  for (GList *li = left; li != NULL; li = g_list_next(li))
  {
    junction *j = (junction*)li->data;
    printf("junction: %s\n", (gchar*)g_list_nth_data(junctions, i));
    printf("  status: %d\n", j->status);
    printf("  avgSpeed: %d\n", j->avgSpeed);
    
    if (j->cameras != NULL)
    {
      printf("  cameras: ");
      for (GList *cli = j->cameras; cli != NULL; cli = g_list_next(cli))
      {
        int *cameraNumber = (int*)cli->data;
        printf("%d ", *cameraNumber);
      }
      printf("\n");
    }

    if (j->matrixMsgs != NULL)
    {
      printf("  matrix messages:\n");
      for (GList *msg = j->matrixMsgs; msg != NULL; msg = g_list_next(msg))
      {
        printf("   ****** %s ******\n", (char*)msg->data);
      }
    }
    
    for (GList *li = j->roadworks; li != NULL; li = g_list_next(li))
    {
      motorway_event *event = (motorway_event*)li->data;
      printf("  roadworks: %s\n", event->description);
      printf("   delay: %s\n", event->delay);
    }
    
    for (GList *li = j->incidents; li != NULL; li = g_list_next(li))
    {
      motorway_event *event = (motorway_event*)li->data;
      printf("  incident: %s\n", event->description);
      printf("   delay: %s\n", event->delay);
    } 
    i++;
  }
  motorway_free(mway);
}

int main()
{
  g_type_init();

  moflow_client *client = init_moflow();
  moflow_request *request = moflow_init_request();

  if (getenv("MOFLOW_USE_SAMPLE") == NULL)
  {
    GHashTable *mlist = motorway_list(client);
    if (mlist == NULL)
    {
      printf("unable to get motorway list\n");
      exit(1);
    }	 
    request->motorway_id = *((int*)g_hash_table_lookup(mlist, "M25"));
    motorway_list_free(mlist);
  }
  
  moflow_req_generate_url(request);
  
  /*request->min_level = 0;
  request->right = 0;*/

  pthread_t async_get = moflow_get_async(client, request, mcallback);
  pthread_join(async_get, NULL);
  
  moflow_request_free(request);
  moflow_free(client);
}
