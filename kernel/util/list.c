#include <moonos/util/list.h>

void list_init(list_head_t* list) { list->next = list->prev = list; }

static void list_insert(list_head_t* node, list_head_t* prev,
                        list_head_t* next) {
    node->prev = prev;
    node->next = next;
    prev->next = node;
    next->prev = node;
}

void list_add_tail(list_head_t* node, list_head_t* list) {
    list_insert(node, list->prev, list);
}

void list_add(list_head_t* node, list_head_t* list) {
    list_insert(node, list, list->next);
}

void list_del(list_head_t* node) {
    list_head_t* prev = node->prev;
    list_head_t* next = node->next;

    prev->next = next;
    next->prev = prev;
}

int list_empty(const list_head_t* list) { return list->next == list; }