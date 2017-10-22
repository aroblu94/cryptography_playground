#include "utils.h"

/***********************/
/** UTILITY FUNCTIONS **/
/***********************/

/*
 * New Queue and DQueue
 */
Queue * new_queue() {
	Queue *q = calloc(1, sizeof(Queue));
	q->len = 0;
	return q;
}

DQueue * new_dqueue() {
	DQueue *q = calloc(1, sizeof(DQueue));
	q->len = 0;
	return q;
}

/*
 * Append to queue
 */
Queue * append_queue(Queue *nl, uint8_t info) {
	Node *nd = calloc(1, sizeof(Node));
	nd->info = info;
	if (nl->len == 0) {
		nl->head = nl->tail = nd;
	}
	else {
		nl->tail->next = nd;
		nl->tail = nd;
	}
	nl->len += 1;
	return nl;
}

DQueue * append_dqueue(DQueue *nl, uint8_t first, uint8_t second) {
	DNode *nd = calloc(1, sizeof(DNode));
	nd->first = first;
	nd->second = second;
	if (nl->len == 0) {
		nl->head = nl->tail = nd;
	}
	else {
		nl->tail->next = nd;
		nl->tail = nd;
	}
	nl->len += 1;
	return nl;
}

/*
 * Pop from queue
 */
Node * pop_queue(Queue *nl) {
	Node *ret;
	Node *next;
	ret = nl->head;
	next = nl->head->next;
	free(nl->head);
	nl->head = next;
	nl->len -= 1;
	return ret;
}

DNode * pop_dqueue(DQueue *nl) {
	DNode *ret;
	DNode *next;
	ret = nl->head;
	next = nl->head->next;
	free(nl->head);
	nl->head = next;
	nl->len -= 1;
	return ret;
}

/*
 * Delete queue
 */
void del_queue(Queue *nl) {
	if(nl->len > 0) {
		do {
			pop_queue(nl);
		} while(nl->len > 0);
	}
	free(nl);
}

void del_dqueue(DQueue *nl) {
	if(nl->len > 0) {
		do {
			pop_dqueue(nl);
		} while(nl->len > 0);
	}
	free(nl);
}

/*
 * Is in queue
 */
int is_in_queue(Queue *nl, uint8_t v) {
	Node *n = nl->head;
	while(n != NULL) {
		if(n->info == v)
			return 1;
		n = n->next;
	}
	return 0;
}

int is_in_dqueue(DQueue *nl, uint8_t v) {
	DNode *n = nl->head;
	while(n != NULL) {
		if(n->first == v)
			return 1;
		if(n->second == v)
			return 1;
		n = n->next;
	}
	return 0;
}