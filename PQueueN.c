#include <stdio.h>
#include <stdlib.h>
#include"stdbool.h"
#define NULL ((void *)0)

struct PQNode
{
    int data; // data of Node
    struct PQNode * next;  // pointer for next Node
    int priority;
};
//Queue Struct
struct PQueue
{
    struct PQNode * head ,* tail;
};

struct PQNode* newPQNode(int val ,int priority)
{
    struct PQNode * temp=(struct PQNode*)malloc(sizeof(struct PQNode));
    temp->data=val;
    temp->next=NULL;
    temp->priority=priority;
    return temp;
}

struct PQueue* createPQueue()
{
    struct PQueue* pq=(struct PQueue*)malloc(sizeof(struct PQueue));
   //Intialize queue
    pq->head=NULL;
    pq->tail=NULL;
    return pq;
};
bool pq_isEmpty(struct PQueue * pq) {
    return (pq->head == NULL);
}


// enQueue method-> add node to Queue
void enPQueue(struct PQueue* pq,int val, int priority)
{
    struct PQNode* PQnodehead= pq->head;
struct PQNode* nNode=newPQNode(val,priority);
//codition if queue is empty
if(pq->head==NULL)
{
    pq->tail=pq->head=nNode;
    return;
}
else
{
    if (priority <= PQnodehead->priority )// LOW priority first
    {
        nNode->next=PQnodehead;
        pq->head=nNode;

    }
    else if (priority > PQnodehead->priority)
    {
         while ((PQnodehead->next != NULL) && (PQnodehead->next->priority <= priority))
            PQnodehead = PQnodehead->next;
       nNode->next = PQnodehead->next;
        PQnodehead->next = nNode;
    }
}
}
//deQueue method-> remove node from queue and store its data in variable
int dePQueue(struct PQueue* pq)
{
    int val=-1;
    if(pq->head==NULL)
    {
        printf("PQueue is empty\n");
        return val;
    }
struct PQNode* temp=pq->head;
pq->head= pq->head->next;
if(pq->head==NULL)
{
 printf("Queue becomes empty\n");
}
val =temp->data;
free(temp);
return val;
}
