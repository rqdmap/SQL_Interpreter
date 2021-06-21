
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

struct CONDITION{
    void *lval, *rval;
    int lbytes, rbytes;
    int cmp;
};

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

    List* check_all_turple();
    int check_turple(List *turple, ListElement *conElem);
    void desc();

    void read();
    void write();
};

struct DATABASE{
    char *name;
    List table;

    ListElement* find_table(const char* tableName, int bytes);
    void show_tables();
    int desc_table(const char *tableName, int bytes);
    int drop_table(const char *tableName, int bytes);
    void drop_all_table();

    void read();
    void write();
};

void start();
void end();

void read();
void write();

int create_database(const char* databaseName, int bytes);
int use_database(const char* databaseName, int bytes);
int drop_database(const char* databaseName, int bytes);
void show_databases();

int temp_create_table();
int temp_insert();
int temp_select();

void show_con(ListElement* elem);
void show_cons();
void clear_cons();


extern List dbs, temp, cons;
extern DATABASE *current_database;
