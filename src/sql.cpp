#include "sql.h"

#define INT 1
#define CHAR 2

List dbs, temp;
DATABASE *current_database;

char buf[100000];

/***************************************************
 * 全局启动/终止函数
 * ************************************************/
void start(){
    read();
    current_database = NULL;
    puts("Hello");
}

void end(){
    write();
    puts("Bye");
}

/***************************************************
 * 数据库系统级别操作，对数据库进行增删改查
 * ************************************************/
void read(){
    std::string fileName = "DBMS/.dbinfo";
    FILE *in = fopen(fileName.c_str(), "rb");
    while(~fscanf(in, "%s", buf)){
        DATABASE *now = new DATABASE();
        now->name = strdup(buf);
        now->read();
        dbs.push_back(now, sizeof(DATABASE));
    }
}

void write(){
    std::string fileName = "DBMS/.dbinfo";
    FILE *out = fopen(fileName.c_str(), "wb");
    ListElement *elem = dbs.front;
    while(elem != NULL){
        DATABASE *now = (DATABASE*)elem->val;
        fprintf(out, "%s\n", now->name);
        now->write();
        elem = elem->next;
    }
}

ListElement* find_database(const char* databaseName){
    ListElement* res = dbs.front;
    while(res != NULL){
        DATABASE* now = (DATABASE*)res->val;
        if(!strcmp(now->name, databaseName)) return res;
        res = res->next;
    }
    return NULL;
}

int create_database(const char* databaseName){
    if(find_database(databaseName)) return 0;
    
    //Update FS
    std::string cmd = "mkdir DBMS/"; cmd +=databaseName;
    if(system(cmd.c_str())) return 0;
    cmd = "touch DBMS/"; cmd += databaseName; cmd += "/.tableinfo";
    if(system(cmd.c_str())) return 0;

    //Update List infomation
    DATABASE *now = new DATABASE();
    now->name = strdup(databaseName);
    dbs.push_back(now, sizeof(DATABASE));
    return 1;
}

int use_database(const char* databaseName){
    ListElement *elem = find_database(databaseName);
    if(elem == NULL) return 0;
    current_database = (DATABASE*)(elem->val);
    return 1;
}

int drop_database(const char* databaseName){
    ListElement *elem = find_database(databaseName);
    if(elem == NULL) return 0;
    if(current_database == elem->val) current_database = NULL;
    dbs.erase(elem);
    return 1;
}


/***************************************************
 * 数据库级别操作，对表单进行增删改查
 * ************************************************/

void DATABASE::read(){
    std::string fileName = "DBMS/"; fileName += name; fileName += "/.tableinfo";
    FILE *in = fopen(fileName.c_str(), "rb");

    while(~fscanf(in, "%s", buf)){
        TABLE *now = new TABLE();
        now->name = strdup(buf);
        now->belong_to_db = strdup(name);
        now->read();
        table.push_back(now, sizeof(TABLE));
    }
}

void DATABASE::write(){
    std::string fileName = "DBMS/"; fileName += name; fileName += "/.tableinfo";
    FILE *out = fopen(fileName.c_str(), "wb");
    ListElement *elem = table.front;
    while(elem != NULL){
        TABLE *now = (TABLE*)elem->val;
        fprintf(out, "%s\n", now->name);
        now->write();
        elem = elem->next;
    }
}

ListElement* DATABASE::find_table(const char* tableName){

    ListElement *res = table.front;
    while(res != NULL){
        TABLE *now = (TABLE*)res->val;
        if(!strcmp(now->name, tableName)) return res;
        res = res->next;
    }
    return NULL;
}

/***************************************************
 * 表单级别操作，对表单进行具体的修改
 * ************************************************/
/**************************************************
 * 每一张数据表的结构：
 * - 属性域的数量
 * - List 属性域
 * - 元组的数量
 * - List Turple
 * ************************************************/
void TABLE::read(){
    std::string fileName = "DBMS/"; fileName += belong_to_db; fileName += "/"; fileName += name; 
    FILE *in = fopen(fileName.c_str(), "rb");
    fscanf(in, "%d", &fieldNumber);
    for(int i = 0; i < fieldNumber; i++){
        FIELD *now = new FIELD();
        fscanf(in, "%s %d %d", now->name, &now->bytes, &now->type);
        field.push_back(now, sizeof(FIELD));
    }
    fscanf(in, "%d", &turpleNumber);
    for(int i = 0; i < turpleNumber; i++){
        List *now = new List();

        ListElement *fieldElem;
        for(int j = 0; j < fieldNumber; j++){
            FIELD *f = (FIELD*)fieldElem->val;
            void *p = malloc(f->bytes);
            fread(p, f->bytes, 1, in);
            now->push_back(p, f->bytes);

            fieldElem = fieldElem->next;
        }
        turpleEntry.push_back(now, sizeof(List));
    }
}

void TABLE::write(){
    std::string fileName = "DBMS/"; fileName += belong_to_db; fileName += "/"; fileName += name; 
    FILE *out = fopen(fileName.c_str(), "wb");
    fprintf(out, "%d\n", fieldNumber);
    ListElement *elem = field.front;
    while(elem != NULL){
        FIELD *now = (FIELD*)elem->val;
        fprintf(out, "%s %d %d\n", now->name, now->bytes, now->type);
        elem = elem->next;
    }
    fprintf(out, "%d", turpleNumber);
    elem = turpleEntry.front;
    while(elem != NULL){
        List *now = (List*)elem->val;
        ListElement *temp = now->front;
        while(temp != NULL){
            fwrite(temp->val, temp->bytes, 1, out);
            temp = temp->next;
        }
        elem = elem->next;
    }
}


/***************************************************
 * 拆包，解析temp链表中的内容
 * ************************************************/
/***************************************************
 * create_table结构：
 * - tableName
 * - fieldName
 * - fieldType(1 byte) fieldInfomation
 * ************************************************/
int temp_create_table(){
    if(current_database == NULL) return 0;

    ListElement* elem = temp.front; 
    char *p = (char*)elem->val;
    if(current_database->find_table(p) != NULL) return 0;

    TABLE *now = new TABLE();
    now->name = strdup(p);
    now->belong_to_db = strdup(current_database->name);
    temp.pop_front();
    
    assert(temp.length % 2 == 0);
    while(temp.length != 0){
        elem = temp.front; p = (char*)elem->val;
        FIELD *f = new FIELD();
        f->name = strdup(p);
        temp.pop_front();
        
        elem = temp.front; p = (char*)elem->val;
        f->type = p[0];
        if(f->type == INT) f->bytes = 4;
        else if(f->type == CHAR) sscanf(p + 1, "%d", &f->bytes);
        else{
            printf("Bad type %d try to create.", f->type);
            return 0;
        }
        temp.pop_front();
        now->field.push_back(f, sizeof(FIELD));   
    }
    now->fieldNumber = now->field.length;

    current_database->table.push_back(now, sizeof(TABLE));
    printf("%s %d %d\n", now->name, now->field.length, now->turpleEntry.length);
    elem = now->field.front;
    whlie(elem != NULL){
        FIELD *now = (FIELD*)elem->val;
        printf("%s %d %d\n", now->name, now->type, now->bytes);
        elem = elem->next;
    }
    return 1;
}