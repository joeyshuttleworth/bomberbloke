#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"


/*TODO: multi_out - keep track to see if we've sent the packet - if so can forget about it.
  Shouldn't cause an issue but might run out of IDs*/


void free_multi_in(multi_in *multi){
  /*Free all the data inside the multi_in object then delete the object itself*/
 
  for(unsigned int i = 0; i < multi->no_parts; i++)
    free(multi->parts[i]);

  free(multi->sizes);
   
  free(multi);
  return;
}

net_message *assemble_multi(multi_in *multi){
  /*This function takes a pointer to a multi_in structure and reassembles 
    the parts back into a net_message object which is returned */ 
  
  net_message *msg;

  /*First calculate the total size of the packet*/
  int total_size = 0;
  for(unsigned int i = 0; i < multi->no_parts; i++){
    total_size += multi->sizes[i];
  }

  /*Allocate space for the data */
  
  msg->data = malloc(sizeof(char)*total_size);
  msg->data_size = total_size;
  msg->data = multi->operation;
  msg->id   = multi->id;

  /*Populate msg->data */
  int offset = 0;
  for(unsigned int i = 0; i < multi->no_parts; i++){
    memcpy(msg->data + offset, multi->parts[i], multi->sizes[i]); 
    offset += multi->sizes[i];
  }
  
  return msg;
}

void remove_multi_in(list_node *head, list_node *remove){
  /*Remove a multi_in object from a list*/

  list_node *current = head;
  do{
    list_node *prev = current;
    if(current == remove){
      multi_in *tmp_multi;
      free_multi(tmp_multi);
      prev->next = current->next;
      free(current);
      return;
    }
    current = current->next;
  }while(current);

  /*Warning: we didnt remove anything!*/
  printf("remove_multi: No multi_in object was removed from the list\n");
  return;
}


void refresh_multi_lists(){
  /*Iterate over the lists and decrement their ttls */

  list_node *current = _net_multi_in_head;

  do{
    multi_in *multi = current->data;
    multi->time_to_live--;
    current = current->next;
    if(multi->time_to_live == 0){
      list_node *remove = current;
      current = current->next;
      remove_multi_in(_net_multi_in_head, remove);
    }
  }while(current);

  
  do{
    net_message *msg = current->data;
    msg->attempts--;
    current = current->next;
    if(msg->attempts == 0){
      list_node *remove = current;
      current = current->next;
      net_remove_message(remove);
    }
  }while(current);

  return;
}
