#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Defining structures
 */
typedef struct node {
    uint8_t info;
    struct node *next;
} Node;

typedef struct dnode {
    uint8_t first;
    uint8_t second;
    struct dnode *next;
} DNode;

typedef struct queue {
	Node *head;
	Node *tail;
	int len;
} Queue;

typedef struct dqueue {
	DNode *head;
	DNode *tail;
	int len;
} DQueue;


Queue * new_queue();
DQueue * new_dqueue();

Queue * append_queue(Queue *nl, uint8_t info);
DQueue * append_dqueue(DQueue *nl, uint8_t first, uint8_t second);

Node * pop_queue(Queue *nl);
DNode * pop_dqueue(DQueue *nl);

void del_queue(Queue *nl);
void del_dqueue(DQueue *nl);

int is_in_queue(Queue *nl, uint8_t v);
int is_in_dqueue(DQueue *nl, uint8_t v);