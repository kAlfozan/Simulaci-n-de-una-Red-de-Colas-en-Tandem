#include <stdlib.h>
#include "list.h"

void initlist(List *ilist) {
    ilist->head = 0;
}
void insertfront(List *ilist, double val) {
    Listitem *newitem;
    newitem = (Listitem *)malloc(sizeof(Listitem));
    newitem->next = ilist->head;
    newitem->data = val;
    ilist->head = newitem;
}
void insertback(List *ilist, double val) {
    Listitem *ptr;
    Listitem *newitem;

    newitem = (Listitem *)malloc(sizeof(Listitem));
    newitem->data = val;
    newitem->next = 0;

    if (!ilist->head) {
        ilist->head = newitem;
        return;
    }

    ptr = ilist->head;
    while (ptr->next){
        ptr = ptr->next;
    }
    ptr->next = newitem;
}
int length(List ilist){      /* returns list length */
    Listitem *ptr;
    int count = 1;

    if (!ilist.head)
        return 0;
    ptr = ilist.head;
    while (ptr->next)  {
        ptr = ptr->next;
        count++;
    }   return count;
}
void destroy(List *ilist) {          /* deletes list */
    Listitem *ptr1,*ptr2;

    if (!ilist->head)
        return;  /* nothing to destroy */

    ptr1 = ilist->head;        /* destroy one by one */
    while (ptr1)  {
        ptr2 = ptr1;
        ptr1 = ptr1->next;
        free(ptr2);
    }
    ilist->head = 0;
}
void setitem(List *ilist, int n, double val) {
    /* modifies a value*/
    /* assume length is at least n long */
    Listitem *ptr;
    int count = 0;

    if (!ilist->head)
        return;
    ptr = ilist->head;
    for (count = 0; count < n; count++) {
        if (ptr)
            ptr = ptr->next;
        else
            return;
    }
    if (ptr)
        ptr->data = val;
}
double getitem(List ilist, int n) {
    /* returns a list value,
     * assume length is at least n long */
    Listitem *ptr;
    int count = 0;

    if (!ilist.head)
        return 0;
    ptr = ilist.head;
    if (n==0)
        return ptr->data;
    while (ptr->next) {
        ptr = ptr->next;
        count++;
        if (n == count)
            return (ptr->data);
    }
    return 0;
}
void delete_from_begin(List *ilist){
    Listitem *ptr, *ptr2;
    if (!ilist->head)
        return;
    ptr = ilist ->head;
    ptr2 = ptr ->next;
    free(ptr);
    ilist->head = ptr2;
}
void delete_from_position(List *ilist, int pos){
    Listitem *ptr, *ptr2;
    if (!ilist->head)
        return;
    ptr = ilist ->head;
    for (int i = 0; i <= pos-1; ++i) {
        ptr2 = ptr;
        ptr=ptr->next;
    }
    ptr2->next = ptr->next;
    free(ptr);
}

