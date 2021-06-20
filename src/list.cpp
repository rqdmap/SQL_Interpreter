#include "list.h"

List::List(){
    length = 0;
    front = back = NULL;
}

/*******************************************
 * Push_back listElement
 * ****************************************/
void List::push_back(void *val, int L){
    ListElement *now = (ListElement*)malloc(sizeof(ListElement));
    now->val = malloc(L);
    now->bytes = L;
    memcpy(now->val, val, L);
    now->pre = now->next = NULL;

    if(back == NULL){
        assert(back == NULL);
        front = back = now;
        length = 1;
    }
    else{
        now->pre = back;
        back->next = now;
        back = now;
        length++;
    }
}
/*******************************************
 * Erase certain Element p
 * NOT CHECK IF p IS NULL!
 * ****************************************/
void List::erase(ListElement *p){
    //Only p
    if(p->pre == NULL && p->next == NULL){
        length = 0;
        front = back = NULL;
    }
    //p is front
    else if(p->pre == NULL && p->next != NULL){
        front = p->next;
        front->pre = NULL;
        length--;
    }
    //p is back
    else if(p->pre != NULL && p->next == NULL){
        back = p->pre;
        back->next = NULL;
        length--;
    }
    else{
        p->pre->next = p->next;
        p->next->pre = p->pre;
        length--;
    }
    free(p->val);
    free(p);
}

void List::clear(){
    while(front != NULL) erase(front);
}

/*******************************************
 * Copy ALL_INFOMATION from List
 * ****************************************/
void List::copy(List *from){
    clear();

    ListElement* elem = from->front;
    while(elem != NULL){
        void *p = malloc(elem->bytes);
        memcpy(p, elem->val, elem->bytes);
        push_back(p, elem->bytes);
        elem = elem->next;
    }
}

/*******************************************
 * Store infomation in files
 * ****************************************/
void List::save(const char *file, int append){
    FILE *out;
    if(append) out = fopen(file, "ab+");
    else out = fopen(file, "wb+");
    if(out == NULL){
        puts("Open save file failed\n"); 
        return ;
    }

    ListElement *now = front;
    while(now != NULL){
        fwrite(now->val, now->bytes, 1, out);
        now = now->next;
    }
}

void List::pop_front(){
    if(front != NULL) erase(front);
}

void List::pop_back(){
    if(back != NULL) erase(back);
}
