#include "tofu_mld.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*phase 1: Function Implementation Start*/
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
    printf("|-------------------------------------------|\n");
    printf("| %-11s  | size = %-3d    | #flds = %-3d|\n", struct_rec->struct_name,
           struct_rec->ds_size, struct_rec->n_fields);
    printf("|------------------------------------------------------------------"
           "-------------------------------------|\n");
    for (int i = 0; i < struct_rec->n_fields; i++) {
      printf("\t|%-2d %-15s | dtype = %-7s | size = %-2d | offset = %-2d | "
             "nstructname = %s      \t|\n",
             i, struct_rec->fields[i].fname,
             DatatypeToString(struct_rec->fields[i].dtype),
             struct_rec->fields[i].size, struct_rec->fields[i].offset,
             struct_rec->fields[i].nested_str_name);
      printf("|----------------------------------------------------------------"
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
    // move_ptr += 1;//遇到的第一个错误(将move_ptr当成某个数组中指向数据元素类型的指针了,move_ptr + 1以为是move_ptr下一个指向的对象)
    //move_ptr下一个对象是申请的动态内存,因此地址百分之99不可能是连续的.
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
/*phase 1: Function Implementation End*/

/*phase 2: Function Implementation Start*/
object_db_rec_t *object_db_look_up(object_db_t *object_db, object_db_rec_t *ptr){
  assert(object_db != NULL && ptr != NULL);
  if (object_db->count == 0)
    return NULL;
  object_db_rec_t *head = object_db->head;
  for (;head;head = head->next){
    if(ptr == head->ptr)
      return head;
  }
  return NULL;
}
static void add_object_to_object_db(object_db_t *object_db,void *ptr,int units,struct_db_rec_t *struct_db_rec) {
  /*检测对象数据库中是否已有ptr的对象记录*/
  object_db_rec_t *obj_rec = object_db_look_up(object_db, ptr);
  /*如果能找到,则不添加,使用断言报错*/
  assert(!obj_rec);//如果可以找到,则obj_rec的值一定不是NULL,则需要使用!运算符触发断言
  //如果没找到,则添加一条对象记录
  obj_rec = calloc(1, sizeof(object_db_rec_t));
  obj_rec->next = NULL;
  obj_rec->ptr = ptr;
  obj_rec->units = units;
  obj_rec->struct_rec = struct_db_rec;
  if(object_db->count == 0){
    object_db->head = obj_rec;
    obj_rec->next = NULL;
    object_db->count++;
    return;
  }
  obj_rec->next = object_db->head;
  object_db->head = obj_rec;
  object_db->count++;
}
void *xcalloc(object_db_t *object_db, char *struct_name, int units) {
  /*在object_db中的struct_db找到对应结构体类型名称的结构记录的地址*/
  struct_db_rec_t *struct_db_rec = struct_db_look_up(object_db->struct_db,struct_name);
  //对象数据库中的struct_db我并没有进行初始化就开始使用,导致段错误,应该在app.c中创建对象数据库开始就将之前
  //创建的结构体数据库的值赋值给对象数据库中的struct_db成员.
  assert(struct_db_rec);
  void *ptr = calloc(units, struct_db_rec->ds_size);
  add_object_to_object_db(object_db, ptr, units, struct_db_rec);
  return ptr;
}
void print_object_rec(object_db_rec_t *object_rec, int i){
  if(!object_rec) return ;
  printf("---------------------------------------------------------------------------------------|\n");
  printf("%d ptr = %-15p | next = %-15p | unit = %-2d | struct_name = %-9s |\n",
         i,object_rec->ptr,object_rec->next,object_rec->units,object_rec->struct_rec->struct_name);
  printf("---------------------------------------------------------------------------------------|\n");
}
void print_object_db(object_db_t *object_db){
  if(!object_db) return;
  unsigned int number = 0;
  object_db_rec_t *head = object_db->head;
  printf("Printint OBJECT DATABASE\n");
  for (;head;head = head->next){
    print_object_rec(head, number++);
  }
}
