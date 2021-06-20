
/**********************************************
Supported funtion:
1. create database
2. use database
3. create table, typeOptions: [int | varchar | double]
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
};

struct TABLE{
    char *belong_to_db;
    char *name;
    int fieldNumber, turpleNumber;
    List field, turpleEntry;

    void read();
    void write();
};

struct DATABASE{
    char *name;
    List table;

    ListElement* find_table(const char* tableName);

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

int temp_create_table();

extern List dbs, temp;
extern DATABASE *current_database;