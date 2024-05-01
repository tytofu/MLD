#include "tofu_mld.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

/*Application Structures*/

typedef struct emp_ {
  char emp_name[30];
  unsigned int emp_id;
  unsigned int age;
  struct emp_ *mgr;
  float salary;
} emp_t;

typedef struct student_ {
  char stud_name[32];
  unsigned int rollno;
  unsigned int age;
  float aggregate;
  struct student_ *best_colleage;
}student_t;


int main () {
  /**/
  struct_db_t * struct_db = calloc(1, sizeof(struct_db_t));
  struct_db->count = 0;
  static field_info_t emp_fields[] =  {
   FIELD_INFO(emp_t,emp_name,CHAR,NULL),
   FIELD_INFO(emp_t,emp_id,UINT32,NULL),
   FIELD_INFO(emp_t, age, UINT32, NULL),
   FIELD_INFO(emp_t, mgr, OBJ_PTR, emp_t),
   FIELD_INFO(emp_t, salary, FLOAT, NULL)
  };
  REG_STRUCT(struct_db,emp_t , emp_fields)
  static field_info_t student_fields[] = {
    FIELD_INFO(student_t, stud_name, CHAR, NULL),
    FIELD_INFO(student_t, rollno, UINT32, NULL),
    FIELD_INFO(student_t, age, UINT32, NULL),
    FIELD_INFO(student_t, aggregate, FLOAT, NULL),
    FIELD_INFO(student_t, best_colleage, OBJ_PTR, student_t),
  };
  REG_STRUCT(struct_db,student_t,student_fields)
  print_structure_db(struct_db);
  printf("student_t is %p\n",struct_db_look_up(struct_db, "student_t"));
  /*初始化一个对象数据库*/
  object_db_t *object_db = calloc(1, sizeof(object_db_t));
  object_db->count = 0;
  object_db->struct_db = struct_db;//需要进行初始化,第二个错误
  student_t *student_s1 = xcalloc(object_db, "student_t", 1);
  student_t *student_s2 = xcalloc(object_db, "student_t", 1);
  emp_t *emp_e1 = xcalloc(object_db, "emp_t", 1);
  print_object_db(object_db);
}
