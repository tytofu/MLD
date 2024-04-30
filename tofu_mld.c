#include "tofu_mld.h"
#include <stdio.h>
#include <string.h>

char *DatatypeToString(data_type_t data_type) {
  switch (data_type) {
  case UINT8:
    return "UINT8";
  case UINT32:
    return "UINT32";
  case INT32:
    return "INT32";
  case CHAR:
    return "CHAR";
  case OBJ_PTR:
    return "OBJ_PTR";
  case FLOAT:
    return "FLOAT";
  case DOUBLE:
    return "DOUBLE";
  case OBJ_STRUCT:
    return "OBJ_STRUCT";
  }
}
int add_structure_to_struct_db(struct_db_t *struct_db,
                               struct_db_rec_t *struct_rec) {
  struct_db_rec_t *move_ptr = struct_rec;
  if ((struct_db == NULL) && (struct_rec == NULL)) {
    printf("error!");
    return -1;
  }
  if(struct_db->count == 0){
  struct_db->head = struct_rec;
  struct_rec->next = NULL;
  }
  if (struct_db->count > 0) {
    struct_rec->next = struct_db->head;
    struct_db->head = struct_rec;
  }
  struct_db->count++;
  return 0;
}
void print_structure_rec(struct_db_rec_t *struct_rec) {
  if (struct_rec == NULL) {
    printf("error!");
    return;
  } else {
    printf("|-------------------------------------|\n");
    printf("| %s  | size = %d    |#flds = %d|\n", struct_rec->struct_name,
           struct_rec->ds_size, struct_rec->n_fields);
    printf("|-------------------------------------|----------------------------"
           "-------------------------------------|\n");
    for (int i = 0; i < struct_rec->n_fields; i++) {
      printf("\t|%d %s   \t\t| dtype = %s      \t  | size = %d\t| offset = %d\t| "
             "nstructname = %s      \t|\n",
             i, struct_rec->fields[i].fname,
             DatatypeToString(struct_rec->fields[i].dtype),
             struct_rec->fields[i].size, struct_rec->fields[i].offset,
             struct_rec->fields[i].nested_str_name);
      printf("|-------------------------------------|--------------------------"
             "---------------------------------------|\n");
    }
  }
}
void print_structure_db(struct_db_t *struct_db) {
  if (struct_db == NULL) {
    printf("error!");
    return;
  }
  struct_db_rec_t *move_ptr = struct_db->head;
  printf("printing structure db\n");
  printf("No of Structures Registered = %d\n", struct_db->count);
  for (int i = 0; i < struct_db->count; i++) {
    printf("structure No : %d (%p)\n", i, move_ptr);
    print_structure_rec(move_ptr);
    // move_ptr += 1;
    move_ptr = move_ptr->next;
  }
}

struct_db_rec_t *struct_db_look_up(struct_db_t *struct_db, char *struct_name) {
  struct_db_rec_t *move_ptr = struct_db->head;
  if (struct_db == NULL && struct_name == NULL && struct_db->count == 0) {
    printf("error!");
    return NULL;
  }
  while(move_ptr != NULL){
    if(strcmp(move_ptr->struct_name, struct_name) == 0){
      return move_ptr;
    }
    move_ptr = move_ptr->next; 
  }
  return NULL;
}
