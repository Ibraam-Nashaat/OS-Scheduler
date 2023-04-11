#include"stdlib.h"
#include "stddef.h"
#include"stdbool.h"
#include <stdio.h>
#define NULL ((void *)0)
struct Node
{
    int data; // data of Node
    struct Node * next;  // pointer for next Node
};
//Queue Struct
struct Queue
{
    struct Node * head ,* tail;
};
// create queue 
struct Queue* createQueue()
{
    struct Queue* q=(struct Queue*)malloc(sizeof(struct Queue));
   //Intialize queue
    q->head=NULL;
    q->tail=NULL;
    return q;
};
// intailiaze new node 
struct Node* newNode(int val )
{
    struct Node * temp=(struct Node*)malloc(sizeof(struct Node));
    temp->data=val;
    temp->next=NULL;
    return temp;
}

// IsEmpty methodknown by head pointer
bool isEmpty(struct Queue* q)
{
     if( q->head == NULL)
     {
     return true;
     }
     else return false;
}

// enQueue method-> add node to Queue
void enQueue(struct Queue* q,int val)
{
struct Node* nNode=newNode(val);
//codition if queue is empty
if(q->head==NULL)
{
    q->tail=q->head=nNode;
    return;
}
else
{
    // if queue isn't empty make Next Pointer of tail implies to newNode
    q->tail->next=nNode;
    // move tail pointer to newNode
    q->tail=nNode;
}
}
//deQueue method-> remove node from queue and store its data in variable
int deQueue(struct Queue* q)
{
    int val=-1;
    if(q->head==NULL)
    {
        printf("Queue is empty\n");
        return val;
    }
struct Node* temp=q->head;
q->head=q->head->next;
if(q->head==NULL)
{
 printf("Queue becomes empty\n");
}
val =temp->data;
free(temp);
return val;
}

