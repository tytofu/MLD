#include "tofu_mld.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  if (struct_db->count == 0) {
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
    printf("| %-11s  | size = %-3d    | #flds = %-3d|\n",
           struct_rec->struct_name, struct_rec->ds_size, struct_rec->n_fields);
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
    // move_ptr +=
    // 1;//遇到的第一个错误(将move_ptr当成某个数组中指向数据元素类型的指针了,move_ptr
    // + 1以为是move_ptr下一个指向的对象)
    // move_ptr下一个对象是申请的动态内存,因此地址百分之99不可能是连续的.
    move_ptr = move_ptr->next;
  }
}

struct_db_rec_t *struct_db_look_up(struct_db_t *struct_db, char *struct_name) {
  struct_db_rec_t *move_ptr = struct_db->head;
  if (struct_db == NULL && struct_name == NULL && struct_db->count == 0) {
    printf("error!");
    return NULL;
  }
  while (move_ptr != NULL) {
    if (strcmp(move_ptr->struct_name, struct_name) == 0) {
      return move_ptr;
    }
    move_ptr = move_ptr->next;
  }
  return NULL;
}
/*phase 1: Function Implementation End*/

/*phase 2: Function Implementation Start*/
object_db_rec_t *object_db_look_up(object_db_t *object_db, void *ptr) {
  assert(object_db != NULL && ptr != NULL);
  if (object_db->count == 0)
    return NULL;
  object_db_rec_t *head = object_db->head;
  for (; head; head = head->next) {
    if (ptr == head->ptr)
      return head;
  }
  return NULL;
}
static void add_object_to_object_db(object_db_t *object_db, void *ptr,
                                    int units, struct_db_rec_t *struct_db_rec,mld_boolean_t is_root) {
  /*检测对象数据库中是否已有ptr的对象记录*/
  object_db_rec_t *obj_rec = object_db_look_up(object_db, ptr);
  /*如果能找到,则不添加,使用断言报错*/
  assert(
      !obj_rec); // 如果可以找到,则obj_rec的值一定不是NULL,则需要使用!运算符触发断言
  // 如果没找到,则添加一条对象记录
  obj_rec = calloc(1, sizeof(object_db_rec_t));
  obj_rec->next = NULL;
  obj_rec->ptr = ptr;
  obj_rec->units = units;
  obj_rec->struct_rec = struct_db_rec;
  obj_rec->is_root = is_root;
  if (object_db->count == 0) {
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
  struct_db_rec_t *struct_db_rec =
      struct_db_look_up(object_db->struct_db, struct_name);
  // 对象数据库中的struct_db我并没有进行初始化就开始使用,导致段错误,应该在app.c中创建对象数据库开始就将之前
  // 创建的结构体数据库的值赋值给对象数据库中的struct_db成员.
  assert(struct_db_rec);
  void *ptr = calloc(units, struct_db_rec->ds_size);
  add_object_to_object_db(object_db, ptr, units, struct_db_rec,MLD_FALSE);
  return ptr;
}
void print_object_rec(object_db_rec_t *object_rec, int i) {
  if (!object_rec)
    return;
  printf("---------------------------------------------------------------------"
         "------------------------------------|\n");
  printf("%d ptr = %-15p | next = %-15p | unit = %-2d | struct_name = %-9s | is_root = %-5s |\n",
         i, object_rec->ptr, object_rec->next, object_rec->units,
         object_rec->struct_rec->struct_name,object_rec->is_root ? "TRUE" : "FALSE");
  printf("---------------------------------------------------------------------"
         "------------------------------------|\n");
}
void print_object_db(object_db_t *object_db) {
  if (!object_db)
    return;
  unsigned int number = 0;
  object_db_rec_t *head = object_db->head;
  printf("Printint OBJECT DATABASE\n");
  for (; head; head = head->next) {
    print_object_rec(head, number++);
  }
}
void mld_dump_object_rec_detail(object_db_rec_t *object_db_rec,const char *struct_ptr_name) {//这个是重点
  assert(object_db_rec);
  void *ptr = object_db_rec->ptr; // 申请动态内存的基地址
  field_info_t *fields =
      object_db_rec->struct_rec
          ->fields; // 该指针指向对象记录中的结构体记录中的第一个结构体类型成员
  // 如果object_db_rec记录的units是大于1的,那么就相当于申请了某个结构体类型的数组
  // 要根据units的个数输出数组中每个结构体成员的值
  unsigned int object_index = 0;
  // 找到当前对象记录在对象数据库中的索引
  //  data_type_t data_type;//初始化枚举变量
  while (object_index < object_db_rec->units) {
    ptr += object_index * object_db_rec->struct_rec->ds_size;
    for (int i = 0; i < object_db_rec->struct_rec->n_fields; i++) {
      switch (fields[i].dtype) {
      case UINT8: {
        printf("%s[%d]->%s = %hhd\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname,
               *((unsigned char *)(ptr + fields[i].offset)));
        break;
      }
      case UINT32: {
        printf("%s[%d]->%s = %u\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname,
               *((unsigned int *)(ptr + fields[i].offset)));
        break;
      }
      case INT32: {
        printf("%s[%d]->%s = %d\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname,
               *((int *)(ptr + fields[i].offset)));
      }
      case CHAR: {
        printf("%s[%d]->%s = %s\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname,
               (char *)(ptr + fields[i].offset));
        break;
      }
      case FLOAT: {
        printf("%s[%d]->%s = %f\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname,
               *((float *)(ptr + fields[i].offset)));
        break;
      }
      case DOUBLE: {
        printf("%s[%d]->%s = %f\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname,
               *((double *)(ptr + fields[i].offset)));
        break;
      }
      case OBJ_PTR:
      case OBJ_STRUCT: {
        printf("%s[%d]->%s = %s\n", struct_ptr_name,
               object_index, object_db_rec->struct_rec->fields[i].fname, "nil");
        break;
      }
      }
    }
    object_index++;
  }
}

// 获取对象记录的上一条对象记录的地址
static object_db_rec_t *pre_object_db_rec_fn(object_db_t *object_db,
                                             object_db_rec_t *object_db_rec) {
  object_db_rec_t *head = object_db->head;
  while (1) {
    if (head->next == object_db_rec) {
      return head;
    }
    head = head->next;
  }
  return head;
}
void xfree(object_db_t *object_db, void *ptr) {
  // 通过ptr与object_db找到ptr所在的object_db_rec的地址
  object_db_rec_t *object_db_rec = object_db_look_up(object_db, ptr);
  // 将object_db中的链表头指针赋值给head
  object_db_rec_t *head = object_db->head;
  // 1.删除对象记录
  // 如果此对象记录是链表中的第一条记录,也就是头指针指向此对象记录
  if (head == object_db_rec) { //这里可以做一个优化,创建一个虚拟头节点
    head = object_db_rec->next; // 直接指向此对象记录的下一个对象记录删除此对象
    object_db->count--;
  } else {
    // 获取此对象记录的前一条对象记录的地址
    object_db_rec_t *pre_object_db_rec =
        pre_object_db_rec_fn(object_db, object_db_rec);
    // 将前一条对象记录的地址直接指向此对象记录的后一条对象记录地址,后一条记录可由
    // pre_object_db_rec->next获取,这个值也可能是NULL,因为此对象记录可能是最后一条
    pre_object_db_rec = object_db_rec->next;
    object_db->count--;
  }
  // 2.释放ptr所在的堆空间
  free(ptr);
  // 3. 释放ptr所在的对象记录的堆空间
  free(object_db_rec);
}
/*The global object of the application which is not created by xcalloc show be registered with
 * MLD using below API*/
void mld_register_global_as_root(object_db_t *object_db, void *objptr, char *struct_name, unsigned int units){
  //通过结构体名称获取结构体记录地址
  struct_db_rec_t *struct_db_rec = struct_db_look_up(object_db->struct_db, struct_name);
  //创建一个新的对象记录添加到对象数据库中
  add_object_to_object_db(object_db, objptr, units, struct_db_rec, MLD_TRUE);
}

/*Application might create an object using xcalloc,but at the same time the object can be root
 * object.Use this API to override the object flags for the object already preet in object db*/
void mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr) {
  //通过obj_ptr找到已分配的对象记录地址
  object_db_rec_t *object_db_rec = object_db_look_up(object_db, obj_ptr);
  //设置此对象记录中的is_root为MLD_TRUE
  object_db_rec->is_root = MLD_TRUE;
}
