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
void list_splice(list_head_t* from, list_head_t* to);
void __list_splice(list_head_t* first, list_head_t* last, list_head_t* prev);
void list_splice_tail(list_head_t* from, list_head_t* to);
int list_empty(const list_head_t* list);

static inline void list_add_after(struct list_head *node,
			struct list_head *ptr)
{
	list_add(node, ptr);
}

static inline void list_add_before(struct list_head *node,
			struct list_head *ptr)
{
	list_add_tail(node, ptr);
}

#ifdef __cplusplus
}
#endif

#endif  // __LIST_H__