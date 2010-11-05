#ifndef LLIST_H
#include <stdlib.h>

typedef struct llist_n {
	int idx;
	short amp;
	struct llist_n *next;
} LList;

typedef struct llist_h {
	LList *first;
	LList *last;
	int len;
} LListH;


LListH *llist_init();

void llist_append( LListH *list, int idx, short amp );

void llist_remove_idx( LListH *list, int idx );

LListH *llist_free( LListH *list );

void llist_reinit( LListH *list );

#endif
