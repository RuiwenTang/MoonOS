#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

struct list_head {
    struct list_head* next;
    struct list_head* prev;
};

typedef struct list_head list_head_t;

void list_init(list_head_t* list);
void list_add_tail(list_head_t* node, list_head_t* list);
void list_add(list_head_t* node, list_head_t* list);
void list_del(list_head_t* node);
int list_empty(const list_head_t* list);

#ifdef __cplusplus
}
#endif

#endif  // __LIST_H__