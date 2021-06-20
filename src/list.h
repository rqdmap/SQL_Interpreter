#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>
#include "rqdmap.h"

struct ListElement{
    int bytes;
    void *val;
    ListElement *pre, *next;
};

struct List{
    ListElement *front, *back;
    int length;

    List();

    void push_back(void *val, int L);
    void erase(ListElement*);
    void copy(List *);
    void pop_front();
    void pop_back();

    void clear();

    void save(const char *file, int append);
};