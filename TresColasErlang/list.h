#ifndef TRESCOLASERLANG_LIST_H
#define TRESCOLASERLANG_LIST_H

struct listitem {
    double data;
    struct listitem *next;
};

typedef struct listitem Listitem;

struct list {
    Listitem *head;
};


typedef struct list List;


void initlist (List *);              /* initialize an empty list */
void insertfront(List * , double val);  /* insert val at front */
void insertback(List  *, double val);   /* insert val at back */
int length(List);                    /* returns list length */
void destroy(List *);                /* deletes list */
void setitem(List *, int n, double val);/* modifies item at n to val*/
double getitem(List, int n);            /* returns value at n*/
void delete_from_begin(List *);
void delete_from_position(List *, int pos);

#endif //TRESCOLASERLANG_LIST_H
