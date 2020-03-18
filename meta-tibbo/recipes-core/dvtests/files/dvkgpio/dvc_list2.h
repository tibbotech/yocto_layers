/*

 Two-way cycled linked list
 Based on Linux kernel list declarations
 See https://github.com/torvalds/linux/blob/master/include/linux/list.h
 and https://kernelnewbies.org/FAQ/LinkedLists

*/

#ifndef DVC_LIST2_H
#define DVC_LIST2_H

#include <string.h>
#include <stddef.h>

// two-sides list

typedef struct list2_t {
 struct list2_t *n;		// next
 struct list2_t *p;		// prev
} list2_t;

// helper: initial decl
#define LIST2_I(name) {  &(name), &(name)  }
// helper: definition
#define LIST2_D(name) list2_t name = LIST2_I(name)

// init list
static inline void list2_i( list2_t *_l) {
 _l->n = _l;  _l->p = _l;
}

// internal: insert _e between _p and _n
static inline void __list2_a( list2_t *_e, list2_t *_p, list2_t *_n) {
 _n->p = _e;
 _e->n = _n;
 _e->p = _p;
 _p->n = _e;
 return;  }

// external: add after (_head)
static inline void list2_a( list2_t *_e, list2_t *_head) {
 __list2_a( _e, _head, _head->n);
 return;  }

// external: add to tail
static inline void list2_a_tail( list2_t *_e, list2_t *_head) {
 __list2_a( _e, _head->p, _head);
 return;  }

// internal: delete
static inline void __list2_d( list2_t *_p, list2_t *_n) {
 _n->p = _p;
 _p->n = _n;
 return;  }

// external: del
static inline void list2_d( list2_t *_e) {
 __list2_d( _e->p, _e->n);
 _e->n = _e->p = NULL;
 return;  }

// external: replace element
static inline void list2_r( list2_t *_old, list2_t *_new) {
 _new->n = _old->n;
 _new->n->p = _new;
 _new->p = _old->p;
 _new->p->n = _new;
 return;  }

static inline int list2_is_last( const list2_t *_e, const list2_t *_head) {
 return( _e->n == _head);  }

static inline int list2_is_null( const list2_t *_head) {
 return( _head->n == _head);  }

// ptr - pointer to a member
// type - type of the container
// member - the name of the member within the struct
#define CONTAINER_OF(ptr, type, member) ({                   \
     const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
     (type *)( (char *)__mptr - offsetof(type,member) );})

// get list entry
#define LIST2_E(ptr, type, member) CONTAINER_OF(ptr, type, member)
// last entry, (ptr) is the list head
#define LIST2_EL(ptr, type, member) LIST2_E((ptr)->p, type, member)
// first entry, (ptr) is the list head
#define LIST2_EF(ptr, type, member) LIST2_E((ptr)->n, type, member)
// next entry, after (pos)
#define LIST2_EN(pos, member) LIST2_E((pos)->member.n, typeof(*(pos)), member)

// (pos) is the pointer to list2_t - cursor
#define LIST2_EACH(pos, head) for (pos = (head)->n; pos != (head); pos = pos->n)
// it safe from removal
#define LIST2_EACH_SAFE(pos, sp, head) \
    for (pos = (head)->n, sp = pos->n; pos != (head); pos = sp, sp = pos->n)

#endif // DVC_LIST2_H
