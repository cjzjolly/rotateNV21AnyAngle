/**防止重复引用**/
#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H
#endif

#define new(Class) (Class*)malloc(sizeof(Class))
#define findMethodByObject(Object, Method, ...) Object->Method(Object, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LinkedListItem ListItem;
typedef struct LinkedListHead ListHead;

struct LinkedListItem {
    void *content;
    ListItem *prev,
            *next;
};

struct LinkedListHead {
    ListItem *headItem;
    ListItem *lastItem;
    int length;

    void (*removeItem)(ListHead *head, ListItem *item);

    void (*removeItemByPosition)(ListHead *head, int pos);

    void (*add)(ListHead *head, void *content);

    void *(*get)(ListHead *head, int pos);

    void (*insert)(ListHead *head, void *content, int pos);
};

void removeItem(ListHead *head, ListItem *item);
void removeItemByPosition(ListHead *head, int pos);
void add(ListHead *head, void *content);
void *get(ListHead *head, int pos);
void insert(ListHead *head, void *content, int pos);

#ifdef __cplusplus
}
#endif

#include "LinkedList.c"