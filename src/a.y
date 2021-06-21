%{
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "sql.h"

int yylex (void);
void yyerror(const char *str){
    fprintf(stderr,"error:%s\n",str);
}

%}
%union{
    char *str;
    int num;
    struct {
        char *p;
        int bytes;   
    } M;
}

%token CREATE DATABASE DATABASES USE TABLE TABLES SHOW DESC INSERT INTO VALUES SELECT UPDATE DELETE DROP EXIT INT DOUBLE CHAR AND OR FROM WHERE SET
%token FIN LB RB COMMA BELOW EQU STAR QM
%token <str> ID STRING INTEGER
%type <str> create_database use_database desc_table drop_table drop_database value
%type <M> datatype
%type <num> create_table insert select

%left OR
%left AND

%%

statements:
    statement
    |statements statement 
    ;

statement: 
    create_database {
        if(create_database($1, strlen($1))) puts("create_database ok.");
        else puts("create_database fail.");
    }
    |use_database {
        if(use_database($1, strlen($1))) puts("use_database ok.");
        else puts("use_database fail.");
    }
    |create_table {
        if($1) puts("create_table ok.");
        else puts("create_table fail.");
    }
    |show_databases {
        show_databases();
    }
    |show_tables {
        if(current_database == NULL) puts("No database selected!");
        else current_database->show_tables();
    }
    |desc_table {
        if(current_database == NULL) puts("No database selected!");
        else current_database->desc_table($1, strlen($1));
    }
    |insert {
        if($1) puts("insert_into ok.");
        else puts("insert_into fail.");
    }
    |delete {printf("Delete something ok\n");}
    |update {printf("Update ok\n");}
    |select {
        if($1 == 1) puts("Select ok");
        else puts("Select fail");
    }
    |drop_table {
        if(current_database == NULL) puts("No database selected!");
        else if(current_database->drop_table($1, strlen($1)) == 0) puts("drop_table fail.");
        else puts("drop_table ok.");
    }
    |drop_database {
        if(drop_database($1, strlen($1))) puts("drop_database ok.");
        else puts("drop_database fail.");
    }
    |exit {return 0;}
    ;

create_database:
    CREATE DATABASE ID FIN {$$ = $3;}

use_database:
    USE DATABASE ID FIN {$$ = $3;}
    
create_table:
    CREATE TABLE ID LB {
        temp.clear();
        temp.push_back($3, strlen($3));
    }
    fields
    RB FIN {
        if(temp_create_table() == 0) $$ = 0;
        else $$ = 1;
    }
fields: 
    field
    |fields COMMA field
field: 
    ID{
        temp.push_back($1, strlen($1));
    }
    datatype {
        temp.push_back($3.p, $3.bytes);
    }
datatype:
    INT {
        $$.bytes = 1;
        $$.p = (char*)malloc($$.bytes);
        $$.p[0] = 1;
    }
    |CHAR LB INTEGER RB {
        $$.bytes = 1 + strlen($3);
        $$.p = (char*)malloc($$.bytes);
        $$.p[0] = 2;
        for(int i = 1; i < $$.bytes; i++) $$.p[i] = $3 [i - 1];
    }

show_tables: SHOW TABLES FIN 

show_databases: SHOW DATABASES FIN

desc_table: DESC ID FIN {$$ = $2;}

insert: 
    INSERT INTO ID {
        temp.clear();
        temp.push_back($3, strlen($3));
    }
    insert_method 
    FIN {
        if(temp_insert()) $$ = 1;
        else $$ = 0; 
    }

insert_method:
    LB {
        char *p = (char*)malloc(1); p[0] = 0;
        temp.push_back(p, 1);
    }
    cols RB {
        char *p = (char*)malloc(1); p[0] = 0;
        temp.push_back(p, 1);
    }
    insert_values 
    |insert_values 
cols:
    ID {
        temp.push_back($1, strlen($1));
    }
    |cols COMMA ID {
        temp.push_back($3, strlen($3));
    }
insert_values:
    VALUES LB values RB
values:
    value {
        temp.push_back($1, strlen($1));
    }
    |values COMMA value {
        temp.push_back($3, strlen($3));
    }
value:
    INTEGER {
        $$ = $1;
    }
    |STRING {
        $$ = $1;
    }

conditions:
    WHERE with_conditions
    |
with_conditions:
    with_conditions AND with_conditions {
        char *p = (char*)malloc(1); p[0] = 1;
        cons.push_back(p, 1);
    }
    |with_conditions OR with_conditions {
        char *p = (char*)malloc(1); p[0] = 2;
        cons.push_back(p, 1);
    }
    |LB with_conditions RB
    |ID BELOW value {
        CONDITION *now = new CONDITION;
        now->lval = $1; now->lbytes = strlen($1);
        now->rval = $3; now->rbytes = strlen($3);
        now->cmp = 1;
        cons.push_back(now, sizeof(CONDITION));
    }
    |ID EQU value {
        CONDITION *now = new CONDITION;
        now->lval = $1; now->lbytes = strlen($1);
        now->rval = $3; now->rbytes = strlen($3);
        now->cmp = 2;
        cons.push_back(now, sizeof(CONDITION));
    }

delete:
    DELETE FROM ID conditions FIN;

update:
    UPDATE ID
    SET set_values
    conditions FIN;
set_values:
    set_value
    |set_values COMMA set_value
set_value:
    ID EQU value

select:
    SELECT{
        clear_cons();
        temp.clear();
    }
    select_what
    FROM ID{
        temp.push_back($5, strlen($5));
    }
    conditions FIN{
        if(temp_select() == 1) $$ = 1;
        else $$ = 0;
    }
ids:
    ID {
        temp.push_back($1, strlen($1));
    }
    |ids COMMA ID {
        temp.push_back($3, strlen($3));
    }
select_what:
    STAR
    |ids
    
drop_table:
    DROP TABLE ID FIN {$$ = $3;}

drop_database: 
    DROP DATABASE ID FIN {$$ = $3;}

exit:
    EXIT 
%%

int main(){
    start();
    yyparse();
    end();
    return 0;
}