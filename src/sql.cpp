#include "sql.h"

#define INT 1
#define CHAR 2

#define BELOW 1
#define EQU 2

#define AND 1
#define OR 2

List dbs, temp, cons;
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

void pprt(int num, char c){assert(num >= 0); while(num--) printf("%c", c);}

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
        now->name = (char*)malloc(strlen(buf) + 1);
        memset(now->name, 0, strlen(buf) + 1); strcpy(now->name, buf);
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

ListElement* find_database(const char* databaseName, int bytes){
    ListElement* res = dbs.front;
    while(res != NULL){
        DATABASE* now = (DATABASE*)res->val;
        if(!strcmp(now->name, databaseName)) return res;
        res = res->next;
    }
    return NULL;
}

int create_database(const char* databaseName, int bytes){
    if(find_database(databaseName, bytes)) return 0;
    
    //Update FS
    std::string cmd = "mkdir DBMS/"; cmd +=databaseName;
    if(system(cmd.c_str())) return 0;
    cmd = "touch DBMS/"; cmd += databaseName; cmd += "/.tableinfo";
    if(system(cmd.c_str())) return 0;

    //Update List infomation
    DATABASE *now = new DATABASE();
    now->name = (char*)malloc(strlen(databaseName) + 1);
    memset(now->name, 0, strlen(databaseName) + 1);
    strcpy(now->name, databaseName);
    dbs.push_back(now, sizeof(DATABASE));
    return 1;
}

int use_database(const char* databaseName, int bytes){
    ListElement *elem = find_database(databaseName, bytes);
    if(elem == NULL) return 0;
    current_database = (DATABASE*)(elem->val);
    return 1;
}

int drop_database(const char* databaseName, int bytes){
    ListElement *elem = find_database(databaseName, bytes);
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
    int width = 4 + 8;
    ListElement *elem = dbs.front;
    while(elem != NULL){
        width = max(width, (int)strlen(((DATABASE*)(elem->val))->name) + 6);
        elem = elem->next;
    }
    printf("%d\n", width);

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
        now->name = (char*)malloc(strlen(buf) + 1);
        memset(now->name, 0, strlen(buf) + 1);
        strcpy(now->name, buf);

        now->belong_to_db = (char*)malloc(strlen(name) + 1);
        memset(now->belong_to_db, 0, strlen(name) + 1);
        strcpy(now->belong_to_db, name);
        
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

ListElement* DATABASE::find_table(const char* tableName, int bytes){

    ListElement *res = table.front;
    while(res != NULL){
        TABLE *now = (TABLE*)res->val;
        if(!strncmp(now->name, tableName, bytes)) return res;
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

int DATABASE::desc_table(const char *tableName, int bytes){
    ListElement *res = find_table(tableName, bytes);
    if(res == NULL) return 0;

    ((TABLE*)(res->val))->desc();
    return 1;
}

int DATABASE::drop_table(const char *tableName, int bytes){
    ListElement *elem = find_table(tableName, bytes);
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
    fread(&fieldNumber, 4, 1, in);
    for(int i = 0; i < fieldNumber; i++){
        FIELD *now = new FIELD();
        int bytes; fread(&bytes, 4, 1, in);
        fread(buf, bytes, 1, in); buf[bytes] = 0;
        fread(&now->bytes, 4, 1, in);
        fread(&now->type, 4, 1, in);

        now->name = (char*)malloc(strlen(buf) + 1);
        memset(now->name, 0, strlen(buf) + 1);
        strcpy(now->name, buf);

        field.push_back(now, sizeof(FIELD));
    }
    fread(&turpleNumber, 4, 1, in);
    for(int i = 0; i < turpleNumber; i++){
        List *now = new List();

        for(int j = 0; j < fieldNumber; j++){
            void *p; int bytes;
            fread(&bytes, 4, 1, in);
            p = malloc(bytes);
            fread(p, bytes, 1, in);
            now->push_back(p, bytes);
        }
        turpleEntry.push_back(now, sizeof(List));
    }
}

void TABLE::write(){
    std::string fileName = "DBMS/"; fileName += belong_to_db; fileName += "/"; fileName += name; 
    FILE *out = fopen(fileName.c_str(), "wb");
    fwrite(&fieldNumber, 4, 1, out);
    ListElement *elem = field.front;
    while(elem != NULL){
        FIELD *now = (FIELD*)elem->val;
        int bytes = strlen(now->name);
        fwrite(&bytes, 4, 1, out);
        fwrite(now->name, bytes, 1, out);
        fwrite(&now->bytes, 4, 1, out);
        fwrite(&now->type, 4, 1, out);
        elem = elem->next;
    }
    fwrite(&turpleNumber, 4, 1, out);
    elem = turpleEntry.front;
    while(elem != NULL){
        List *now = (List*)elem->val;
        ListElement *temp = now->front;
        while(temp != NULL){
            fwrite(&temp->bytes, 4, 1, out);
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

/*
    根据cons检查所有的元组，返回一个满足条件的候选List
    不做类型检查，输入应当合法。
*/
List* TABLE::check_all_turple(){
    if(cons.length == 0){
        return &turpleEntry;
    }
    List *ok = new List;
    
    ListElement *turpleElem = turpleEntry.front;
    ListElement *elem;
    while(turpleElem != NULL){
        
        //建立__cons链表，链表元素为所有cons的ListElem指针,避免每次都进行cons的复制
        List *__cons = new List;
        elem = cons.front;
        while(elem != NULL){
            __cons->push_back(&elem, sizeof(void*));
            elem = elem->next;
        }

        List *turple = (List*)turpleElem->val;

        elem = __cons->front; 
        while(elem != NULL){
            ListElement *cond_elem = (*(ListElement**)elem->val);
            //提取该位置前两个elem，并将自身改造为0、1的常值1
            if(cond_elem->bytes == 1){
                if(elem->pre == NULL || elem->pre->pre == NULL){
                    puts("前方元素不足两个!");
                    return NULL;
                }
                int val1 = check_turple(turple, elem->pre->pre);
                if(val1 == -1) return NULL;
                int val2 = check_turple(turple, elem->pre);
                if(val2 == -1) return NULL;
                
                if(((char*)cond_elem->val)[0] == AND) val1 &= val2;
                else if(((char*)cond_elem->val)[0] == OR) val1 |= val2;

                __cons->erase(elem->pre); __cons->erase(elem->pre);
                elem->bytes = 2; elem->val = realloc(elem->val, 2); ((char*)elem->val)[0] = (char)val1;
            }
            elem = elem->next;
        }
        assert(__cons->length == 1);
        elem = __cons->front;

        if(elem->bytes == 2){
            //栈顶为真值1
            if(((char*)elem->val)[0]) ok->push_back(turple, sizeof(List));
        }
        else{
            int res = check_turple(turple, elem);
            if(res == -1) return NULL;
            if(res) ok->push_back(turple, sizeof(List));
        }

        turpleElem = turpleElem->next;
    }

    return ok;
}

/*
    return 0/1 indicating false/true, -1 invalid
*/
int TABLE::check_turple(List *turple, ListElement *elem){
    assert(elem->bytes == 2 || elem->bytes == 8);

    if(elem->bytes == 2) return ((char*)elem->val)[0];

    ListElement* conElem = *(ListElement**)elem->val;
    CONDITION *con = (CONDITION*)conElem->val;
    char *p, *q;
    ListElement *value_elem = turple->front;
    ListElement *field_elem = field.front;

    // DEBUG; show_con(conElem);

    assert(turple->length == field.length);
    while(value_elem != NULL){
        FIELD* f = (FIELD*)field_elem->val;
        if(!strcmp(f->name, (char*)con->lval)){
            if(value_elem->bytes == 0){
                printf("Field %s not initialize yet!\n", f->name);
                return -1;
            }
            if(f->type == INT){
                int val = *(int*)(value_elem->val);
                q = (char*)con->rval;
                int num = 0; for(int i = 0; i < con->rbytes; i++){num *= 10; num += q[i] - '0';}

                // DEBUG; printf("%d %d %d\n", val, num, con->cmp);
                if(con->cmp == BELOW) return val < num;
                else if(con->cmp == EQU) return val == num;
            }
            else if(f->type == CHAR){
                p = (char*)value_elem->val; p++; p[strlen(p) - 1] = 0;
                q = (char*)con->rval; q++; q[strlen(q) - 1] = 0;

                int res;
                if(con->cmp == BELOW) res = strcmp(p, q) < 0;
                else if(con->cmp == EQU) res = strcmp(p, q) == 0;

                p[strlen(p)] = '"';
                q[strlen(q)] = '"';

                return res;
            }
            else{
                puts("Bad Type.");
                return -1;
            }
        }
        value_elem = value_elem->next;
        field_elem = field_elem->next;
    }
    return -1;
}



/************************************************************
 * 拆包，解析temp链表中的内容
 * *********************************************************/
/*
 * create_table结构：
 * - tableName
 * - (fieldName, fieldType(1 byte) fieldBytes)+
 */
int temp_create_table(){
    if(current_database == NULL) return 0;

    ListElement* elem = temp.front; 
    char *p = (char*)elem->val;
    if(current_database->find_table(p, elem->bytes) != NULL) return 0;

    TABLE *now = new TABLE();
    now->name = (char*)malloc(strlen(p) + 1);
    memset(now->name, 0, strlen(p) + 1);
    strcpy(now->name, p);

    now->belong_to_db = (char*)malloc(strlen(current_database->name) + 1);
    memset(now->belong_to_db, 0, strlen(current_database->name) + 1);
    strcpy(now->belong_to_db, current_database->name);

    temp.pop_front();
    
    assert(temp.length % 2 == 0);
    while(temp.length != 0){
        elem = temp.front; p = (char*)elem->val;
        FIELD *f = new FIELD();
        f->name = (char*)malloc(strlen(p) + 1);
        memset(f->name, 0, strlen(p) + 1); strcpy(f->name, p); 

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

    int ambigious = 0;
    for(ListElement* i = now->field.front; i != NULL; i = i->next){
        for(ListElement *j = i->next; j != NULL; j = j->next){
            if(!strcmp(((FIELD*)i->val)->name, ((FIELD*)j->val)->name)){
                ambigious = 1;
                break;
            }
        }
    }
    if(ambigious) return 0;

    current_database->table.push_back(now, sizeof(TABLE));
    return 1;
}


/*
    将elem指向的内容转换为type要求的格式插入到to中
*/
void __insert(List *to, ListElement *elem, int type){
    if(type == INT){
        int num = 0;
        char *p = (char*)elem->val;
        for(int i = 0; i < elem->bytes; i++){
            num *= 10;
            num += p[i] - '0';
        }
        to->push_back(&num, 4);
    }
    else if(type == CHAR){
        to->push_back(elem->val, elem->bytes);
    }
    else{
        puts("Bad Type");
        return ;
    }
}

/*
 * 如果执行insert语句向不存在的属性域插入，不会成功，但也不会报错！
 * temp_insert结构：
 * - tableName
 * - If (next val is 0?) List fieldName...., end with another 0
 * - List values...
 */
int temp_insert(){
    if(current_database == NULL) return 0;
    
    ListElement *elem = temp.front;
    char *p = (char*)elem->val;
    ListElement *tableElem = current_database->find_table(p, elem->bytes);
    if(tableElem == NULL) return 0;

    TABLE *table = (TABLE*)tableElem->val;;
    temp.pop_front();
    elem = temp.front; p = (char*)elem->val;
    
    List *temp_field, *temp_value;
    temp_field = new List; temp_value = new List;
    //指定Field插入，解析出Field信息并存入temp_field
    if(elem->bytes == 1 && p[0] == 0){
        temp.pop_front();
        while(1){
            elem = temp.front; p = (char*)elem->val;
            //终止
            if(elem->bytes == 1 && p[0] == 0){
                temp.pop_front();
                break;
            }
            temp_field->push_back(p, elem->bytes);
            temp.pop_front();
        }
    }
    //读取剩余的value信息
    while(temp.front != NULL){
        elem = temp.front; p = (char*)elem->val;
        temp_value->push_back(p, elem->bytes);
        temp.pop_front();
    }
    List *turple = new List;
    ListElement *temp_value_elem, *temp_field_elem, *field_elem;
    if(temp_field->length == 0){
        if(temp_value->length != table->field.length) return 0;
        field_elem = table->field.front;
        temp_value_elem = temp_value->front;

        while(temp_value_elem != NULL){
            __insert(turple, temp_value_elem, ((FIELD*)field_elem->val)->type);

            field_elem = field_elem->next;
            temp_value_elem = temp_value_elem->next;
        }
    }
    else{
        if(temp_value->length != temp_field->length) return 0;

        //按顺序针对每一个Field，检查是否出现在insert列表中
        field_elem = table->field.front;
        while(field_elem != NULL){
            temp_value_elem = temp_value->front;
            temp_field_elem = temp_field->front;
            int ok = 0;
            while(temp_field_elem != NULL){
                FIELD* f = (FIELD*)field_elem->val;
                if(!strcmp((char*)(temp_field_elem->val), f->name)){
                    ok = 1;
                   __insert(turple, temp_value_elem, f->type);
                   break;
                }
                temp_value_elem = temp_value_elem->next;
                temp_field_elem = temp_field_elem->next;
            }
            if(!ok){
                void *p;
                turple->push_back(p, 0);
            }
            field_elem = field_elem->next;
        }
    }

    // DEBUG;
    // elem = turple->front;
    // field_elem = table->field.front;
    // printf("Turple Length: %d\n", turple->length);
    // while(elem != NULL){
    //     FIELD* f = (FIELD*)field_elem->val;
    //     if(f->type == INT) printf("%d %d\n", elem->bytes, *(int*)elem->val);
    //     else if(f->type == CHAR) printf("%d %s\n", elem->bytes, (char*)elem->val);
    //     elem = elem->next;
    //     field_elem = field_elem->next;
    // }

    table->turpleEntry.push_back(turple, sizeof(List));
    table->turpleNumber++;
    return 1;
}

/*
 显示某一条condition
*/
void show_con(ListElement* elem){
    if(elem->bytes == 1){
        printf("CMP: %d\n", ((char*)elem->val)[0]);
    }
    else if(elem->bytes == 2){
        printf("constValue: %d\n", ((char*)elem->val)[0]);
    }
    else{
        CONDITION* cond = (CONDITION*)elem->val;
        char* p;
        p = (char*)cond->lval; printf("CONDITION: %s ", p);
        printf("%d ", cond->cmp);
        p = (char*)cond->rval; printf("%s\n", p);
    }
}

/*
 显示cons中所有的条件
*/
void show_cons(){
    ListElement* elem = cons.front;
    while(elem != NULL){
        show_con(elem);
        elem = elem->next;
    }
}

void clear_cons(){
    ListElement* elem = cons.front;
    while(elem != NULL){
        if(elem->bytes != 1){
            CONDITION* con = (CONDITION*)elem->val;
            free(con->lval); free(con->rval);   
        }
        elem = elem->next;
    }
    cons.clear();
}

/*
  利用temp以及cons的信息，查询元组信息 
    temp结构:
      - List FieldName, TableName
    cons结构:
      - CONDITION和逻辑运算符的后缀表达

  具体实现：
    - 通过temp更新所有Field的select标记，之后根据select进行输出显示
    - 遍历所有的元组，根据cons决定该元组是否满足条件，如满足条件加入bufList中
    - 根据select标记显示bufList
*/
int temp_select(){
    if(current_database == NULL) return 0;

    ListElement *elem; char *p, *q;
    elem = temp.back; p = (char*)elem->val;
    elem = current_database->find_table(p, elem->bytes);
    if(elem == NULL) return 0;
    TABLE *table = (TABLE*)elem->val;


    //更新所有select标记
    temp.pop_back();
    ListElement *fieldElem = table->field.front;
    int all = 0; if(temp.length == 0) all = 1;
    whlie(fieldElem != NULL){
        FIELD *f = (FIELD*)fieldElem->val;
        if(all) f->selected = 1;
        else{
            f->selected = 0;

            elem = temp.front;
            while(elem != NULL){
                p = (char*)elem->val;
                q = ((FIELD*)fieldElem->val)->name;
                if(!strcmp(p, q)){
                    f->selected = 1;
                    break;
                }
                elem = elem->next;
            }
        }
        
        fieldElem = fieldElem->next;
    }

    //获取ok元组
    List *res = table->check_all_turple();
    if(res == NULL) return 0;

    //处理出长度对齐信息
    int *w = (int*)malloc(table->fieldNumber);
    memset(w, 0, 4 * table->fieldNumber);
    elem = table->field.front;
    for(int i = 0; i < table->fieldNumber; i++){
        FIELD *f = (FIELD*)elem->val;
        if(f->selected) w[i] = 4 + strlen(f->name);
        elem = elem->next;
    }

    elem = res->front;
    while(elem != NULL){
        List *turple = (List*)elem->val;
        ListElement *val = turple->front;
        ListElement *field_elem = table->field.front;
        for(int i = 0; i < table->fieldNumber; i++){
            FIELD* f= (FIELD*)field_elem->val;
            if(f->selected == 1){
                if(val->bytes == 0) w[i] = max(w[i], 6 + 4);
                if(f->type == INT) w[i] = max(w[i], strlen(itoa(*(int*)val->val)) + 6);
                else if(f->type == CHAR) w[i] = max(w[i], val->bytes + 6);
                else{
                    puts("Bad type.");
                    return 0;
                }
            }

            val = val->next;
            field_elem = field_elem->next;
        }

        elem = elem->next;
    }

    //正式开始打印
    // DEBUG; for(int i = 0; i < table->fieldNumber; i++) printf("%d " ,w[i]); puts("");
    printf("+");
    elem = table->field.front;
    for(int i = 0; i < table->fieldNumber; i++){
        FIELD *f = (FIELD*)elem->val;
        if(f->selected == 1){
            pprt(w[i] - 2, '-'); printf("+");
        }
        elem = elem->next;
    }
    puts("");

    printf("|");
    elem = table->field.front;
    for(int i = 0; i < table->fieldNumber; i++){
        FIELD *f = (FIELD*)elem->val;
        if(f->selected == 1){
            printf(" %s", f->name);
            pprt(w[i] - 4 - strlen(f->name), ' ');
            printf(" |");
        }
        elem = elem->next;
    }
    puts("");

    printf("+");
    elem = table->field.front;
    for(int i = 0; i < table->fieldNumber; i++){
        FIELD *f = (FIELD*)elem->val;
        if(f->selected == 1){
            pprt(w[i] - 2, '-'); printf("+");
        }
        elem = elem->next;
    }
    puts("");

    elem = res->front;
    while(elem != NULL){
        List *turple = (List*)elem->val;
        ListElement *val = turple->front;
        ListElement *field_elem = table->field.front;
        printf("|");
        for(int i = 0; i < table->fieldNumber; i++){
            FIELD* f= (FIELD*)field_elem->val;
            if(f->selected == 1){
                if(val->bytes == 0){
                    printf(" %s", "(null)");
                    pprt(w[i] - 4 - 6, ' ');
                    printf(" |");
                }
                else if(f->type == INT){
                    printf(" %s", itoa(*(int*)val->val));
                    pprt(w[i] - 4 - strlen(itoa(*(int*)val->val)), ' ');
                    printf(" |");
                }
                else{
                    printf(" %s", (char*)val->val);
                    pprt(w[i] - 4 - strlen((char*)val->val), ' ');
                    printf(" |");
                }
            }

            val = val->next;
            field_elem = field_elem->next;
        }
        puts("");
        elem = elem->next;
    }

    printf("+");
    elem = table->field.front;
    for(int i = 0; i < table->fieldNumber; i++){
        FIELD *f = (FIELD*)elem->val;
        if(f->selected == 1){
            pprt(w[i] - 2, '-'); printf("+");
        }
        elem = elem->next;
    }
    puts("");

    free(w);
    return 1;
}