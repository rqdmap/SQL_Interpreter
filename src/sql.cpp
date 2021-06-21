#include "sql.h"

#define INT 1
#define CHAR 2

List dbs, temp;
DATABASE *current_database;

char buf[100000];

/************************************************************
 * 辅助功能函数
 * *********************************************************/
char* itoa(int __x){
    int n = 0, x = __x;
    while(x){n++; x /= 10;}
    char *res = (char*)malloc(n + 1);
    res[n] = 0;
    x = __x; while(x){res[--n] = x % 10 + '0'; x /= 10;}
    return res;
}

void pprt(int num, char c){while(num--) printf("%c", c);}

int max(int a, int b){return a > b? a: b;}
int min(int a, int b){return a < b? a: b;}

/************************************************************
 * 全局启动/终止函数
 * *********************************************************/
void start(){
    read();
    current_database = NULL;
    puts("Hello");
}

void end(){
    write();
    puts("Bye");
}

/************************************************************
 * 数据库系统级别操作，对数据库进行增删改查
 * *********************************************************/
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

    //Update FS
    std::string cmd = "rm -r "; cmd += "DBMS/"; cmd += databaseName;
    if(system(cmd.c_str())) return 0;

    //Update List infomation
    DATABASE *now = (DATABASE*)elem->val;
    if(current_database == now) current_database = NULL;
    now->drop_all_table();
    dbs.erase(elem);
    return 1;
}

/*
 Example:
    +--------------------+
    | Database           |
    +--------------------+
    | information_schema |
    | db                 |
    | mysql              |
    | performance_schema |
    | sys                |
    | temp               |
    | wxapp              |
    +--------------------+
 */
void show_databases(){
    int width = 0;
    ListElement *elem = dbs.front;
    while(elem != NULL){
        width = max(width, (int)strlen(((DATABASE*)(elem->val))->name) + 6);
        elem = elem->next;
    }

    printf("+"); pprt(width - 2, '-'); printf("+\n");
    printf("| Database"); pprt(width - 12, ' '); printf(" |\n");
    printf("+"); pprt(width - 2, '-'); printf("+\n");
    
    elem = dbs.front;
    while(elem != NULL){
        DATABASE* now = (DATABASE*)elem->val;
        int space = width - 4 - strlen(now->name);
        printf("| %s", now->name); pprt(space, ' '); printf(" |\n");
        elem = elem->next;
    }
    printf("+"); pprt(width - 2, '-'); printf("+\n");
}

/************************************************************
 * 数据库级别操作，对表单进行增删改查
 * *********************************************************/
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
/*
 Example:
    +--------------+
    | Tables_in_db |
    +--------------+
    | classinfo    |
    | course       |
    | sc           |
    | schedule     |
    | stu          |
    | teaching     |
    | temp         |
    +--------------+
 */
void DATABASE::show_tables(){
    int width = 0;
    ListElement *elem = table.front;
    while(elem != NULL){
        width = max(width, (int)strlen(((TABLE*)(elem->val))->name) + 6);
        elem = elem->next;
    }
    std::string title = "Tables_in_"; title += name;
    width = max(width, (int)title.size() + 4);
    
    printf("+"); pprt(width - 2, '-'); printf("+\n");
    int space = width - 4 - title.size();
    printf("| %s", title.c_str()); pprt(space, ' '); printf(" |\n");
    printf("+"); pprt(width - 2, '-'); printf("+\n");
    
    elem = table.front;
    while(elem != NULL){
        TABLE* now = (TABLE*)elem->val;
        int space = width - 4 - strlen(now->name);
        printf("| %s", now->name); pprt(space, ' '); printf(" |\n");
        elem = elem->next;
    }
    printf("+"); pprt(width - 2, '-'); printf("+\n");
}

int DATABASE::desc_table(const char *tableName){
    ListElement *res = find_table(tableName);
    if(res == NULL) return 0;

    ((TABLE*)(res->val))->desc();
    return 1;
}

int DATABASE::drop_table(const char *tableName){
    ListElement *elem = find_table(tableName);
    if(elem == NULL) return 0;

    std::string fileName = "rm DBMS/"; fileName += name; fileName += "/"; fileName += tableName;     
    if(system(fileName.c_str())) return 0;

    TABLE *now = (TABLE*)elem->val;
    free(now->belong_to_db);
    free(now->name);
    now->field.clear();
    now->turpleEntry.clear();

    table.erase(elem);
    return 1;
}

void DATABASE::drop_all_table(){
    ListElement *elem = table.front;
    //首先回收table内部开辟的空间
    while(elem != NULL){
        TABLE *now = (TABLE*)elem->val;
        free(now->name);
        free(now->belong_to_db);
        now->field.clear();
        now->turpleEntry.clear();

        elem = elem->next;
    }
    //再回收table本身的空间
    table.clear();
}

/************************************************************
 * 表单级别操作，对表单进行具体的修改
 * *********************************************************/
/*
 * .每一张数据表的结构：
 * - 属性域的数量
 * - List 属性域
 * - 元组的数量
 * - List Turple
 */
void TABLE::read(){
    std::string fileName = "DBMS/"; fileName += belong_to_db; fileName += "/"; fileName += name; 
    FILE *in = fopen(fileName.c_str(), "rb");
    fscanf(in, "%d", &fieldNumber);
    for(int i = 0; i < fieldNumber; i++){
        FIELD *now = new FIELD();
        fscanf(in, "%s %d %d", buf, &now->bytes, &now->type);
        now->name = strdup(buf);
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

/*
Example: 
    +-------+-------------+
    | Field | Type        |
    +-------+-------------+
    | sno   | char(50)    |
    | cname | char(50)    |
    | grade | double      |
    | flag  | smallint(6) |
    +-------+-------------+
 */
void TABLE::desc(){
    int w1 = 9, w2 = 8;
    ListElement* elem = field.front;
    while(elem != NULL){
        FIELD *now = (FIELD*)elem->val;
        w1 = max(w1, (int)strlen(now->name) + 6);

        if(now->type == INT) w2 = max(w2, (int)strlen("INT") + 6);
        else if(now->type == CHAR){
            std::string str = "char("; str += itoa(now->bytes); str += ")";
            w2 = max(w2, (int)str.size() + 6);
        }
        else{
            //If current table is complete, codes below will NEVER execute!
            DEBUG; puts("Bad type!");
        }
        elem = elem->next;
    }
    printf("+"); pprt(w1 - 2, '-'); printf("+"); pprt(w2 - 2, '-'); printf("+\n");
    printf("| Field"); pprt(w1 - 9, ' '); printf(" | Type"); pprt(w2 - 8, ' '); printf(" |\n");
    printf("+"); pprt(w1 - 2, '-'); printf("+"); pprt(w2 - 2, '-'); printf("+\n");
    elem = field.front;
    while(elem != NULL){
        FIELD *now = (FIELD*)elem->val;
        printf("| %s", now->name); pprt(w1 - 4 - strlen(now->name), ' ');
        printf(" | "); 
        if(now->type == INT){
            printf("int"); pprt(w2 - 4 - 3, ' ');
        }
        else if(now->type == CHAR){
            std::string str = "char("; str += itoa(now->bytes); str += ")";
            printf("%s", str.c_str()); pprt(w2 - 4 - str.size(), ' ');
        }
        printf(" |\n");
        elem = elem->next;
    }
    printf("+"); pprt(w1 - 2, '-'); printf("+"); pprt(w2 - 2, '-'); printf("+\n");
}




/************************************************************
 * 拆包，解析temp链表中的内容
 * *********************************************************/
/*
 * create_table结构：
 * - tableName
 * - fieldName
 * - fieldType(1 byte) fieldInfomation
 */
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

    // DEBUG; printf("%d\n", current_database->table.length);
    // int cnt = 0;
    // elem = current_database->table.front;
    // while(elem != NULL) {cnt++; elem = elem->next;}
    // printf("%d\n", cnt);
    return 1;
}
