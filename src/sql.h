
/**********************************************
Supported funtion:
1. create database
2. use database
3. create table, typeOptions: [int | varchar]
4. show tables & desc & show databases
5. insert
6. delete
7. update
8. select
9. drop table
10. drop database
11. exit
**********************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "list.h"

struct FIELD{
    char *name;
    int bytes;
    int type;

    //仅仅在执行select操作时使用
    int selected;
};

struct TABLE{
    char *belong_to_db;
    char *name;
    int fieldNumber, turpleNumber;
    List field, turpleEntry;

    void desc();

    void read();
    void write();
};

struct DATABASE{
    char *name;
    List table;

    ListElement* find_table(const char* tableName);
    void show_tables();
    int desc_table(const char *tableName);
    int drop_table(const char *tableName);
    void drop_all_table();

    void read();
    void write();
};

void start();
void end();

void read();
void write();

int create_database(const char* databaseName);
int use_database(const char* databaseName);
int drop_database(const char* databaseName);
void show_databases();

int temp_create_table();

//如果执行insert语句向不存在的属性域插入，不会成功，但也不会报错！
int temp_insert();

extern List dbs, temp;
extern DATABASE *current_database;