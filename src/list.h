/*
 * @Author: your name
 * @Date: 2021-06-21 04:28:28
 * @LastEditTime: 2021-07-01 02:12:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /report/home/rqdmap/codes/SQL_Interpreter/src/list.h
 */
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
    void copy(List *from); //将from链表拥有所有元素信息拷贝到当前链表
    void pop_front();
    void pop_back();
    void clear();
    void save(const char *file, int append); //向文件file写入字节流，append非0则以附加形式缀到文件尾
};