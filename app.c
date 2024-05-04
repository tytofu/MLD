#include "tofu_mld.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  //-----------------student_s1------------------------------
  student_t *student_s1 = xcalloc(object_db, "student_t", 2);
  student_s1[0].best_colleage = NULL;
  student_s1[0].age = 20;
  student_s1[0].rollno = 22;
  student_s1[0].aggregate = 98.21;
  strncpy(student_s1[0].stud_name, "tofu", strlen("tofu"));
  student_s1[1].best_colleage = NULL;
  student_s1[1].age = 21;
  student_s1[1].rollno = 23;
  student_s1[1].aggregate = 100.1;
  strncpy(student_s1[1].stud_name, "tytofu", strlen("tytofu"));
  student_t *student_s2 = xcalloc(object_db, "student_t", 1);
  student_s2->best_colleage = NULL;
  student_s2->age = 21;
  student_s2->rollno = 23;
  student_s2->aggregate = 99.10;
  strncpy(student_s2->stud_name, "wangshuai", strlen("wangshuai"));
  emp_t *emp_e1 = xcalloc(object_db, "emp_t", 1);
  emp_e1->age = 22;
  emp_e1->mgr = NULL;
  emp_e1->emp_id = 1;
  emp_e1->salary = 24;
  strncpy(emp_e1->emp_name, "huamengjun", strlen("huamengjun"));
  print_object_db(object_db);
  //输出此结构体堆对象中各个成员的信息.完成这个项目之后,可以对这个函数进行重写,改变参数为ptr,对用户
  //使用更加友好
  mld_dump_object_rec_detail(object_db_look_up(object_db, student_s1),"student_s1");
  mld_dump_object_rec_detail(object_db_look_up(object_db, student_s2),"student_s2");
  mld_dump_object_rec_detail(object_db_look_up(object_db, emp_e1),"emp_e1");
  printf("%d\n",object_db->count);
  xfree(object_db, student_s1);
  xfree(object_db, student_s2);
  xfree(object_db,emp_e1);
  printf("%d\n",object_db->count);
  // mld_dump_object_rec_detail(object_db->head);
}
