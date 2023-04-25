#include <stdio.h>
#include <stdlib.h>
#define NULL ((void *)0)
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef short bool;
struct ProcessStruct
{
    int id,
        arrivalTime,
        priority,
        runningTime,
        remainingTime,
        startTime,
        waitingTime,
        lastStopedTime,
        pid,
        memSize;
};

struct memoryNode{
    int startLocation,
        endLocation,
        size,
        pid;
};


struct sortedLinkedListNode
{
    struct memoryNode* data; // data of Node
    struct sortedLinkedListNode *next;        // pointer for next Node
    int priority;
};

struct sortedLinkedList
{
    struct sortedLinkedListNode *head, *tail;
};
struct sortedLinkedListNode*newSortedLLNode(struct memoryNode * memoryNode, int priority)
{
    struct sortedLinkedListNode*temp = (struct sortedLinkedListNode*)malloc(sizeof(struct sortedLinkedListNode));
    temp->data = memoryNode;
    temp->next = NULL;
    temp->priority = priority;
    return temp;
}

struct memoryNode* createMemoryNode(int startLocation,int size,int pid){
    struct memoryNode *memoryNode=(struct memoryNode *)malloc(sizeof(struct memoryNode));
    memoryNode->startLocation=startLocation;
    memoryNode->endLocation=startLocation+size-1;
    memoryNode->size=size;
    memoryNode->pid=pid;
};
struct sortedLinkedList *createSortedLinkedList()
{
    struct sortedLinkedList *LL = (struct sortedLinkedList *)malloc(sizeof(struct sortedLinkedList));
    // Intialize queue
    LL->head = NULL;
    LL->tail = NULL;
    return LL;
};
struct ProcessStruct *runningProcess = NULL;
struct sortedLinkedList* memoryHoles, *memoryUsed;

// enQueue method-> add node to Queue
void insert(struct sortedLinkedList *LL, struct memoryNode * memoryNode, int priority)
{
    struct sortedLinkedListNode*LLnodeHead = LL->head;
    struct sortedLinkedListNode*nNode = newSortedLLNode(memoryNode, priority);
    // codition if queue is empty
    if (LL->head == NULL)
    {
        LL->tail = LL->head = nNode;
        return;
    }
    else
    {
        if (priority <= LLnodeHead->priority) // LOW priority first
        {
            nNode->next = LLnodeHead;
            LL->head = nNode;
        }
        else if (priority > LLnodeHead->priority) // if not try to rearrange itself
        {
            while ((LLnodeHead->next != NULL) && (LLnodeHead->next->priority <= priority))
                LLnodeHead = LLnodeHead->next;
            nNode->next = LLnodeHead->next;
            LLnodeHead->next = nNode;
        }
    }
}

struct sortedLinkedListNode* removeLinkedListNode(struct sortedLinkedListNode* node,struct sortedLinkedList* LL)
{
    // If either the head or node to be deleted is NULL, return
    if (LL == NULL || node == NULL) {
        return NULL;
    }
    
      if (LL->head == node) {
        LL->head = node->next;
        node->next = NULL;
        return node;
    }
    
    // Traverse the linked list to find the node before the node to be deleted
    struct sortedLinkedListNode* prev = LL->head;
    while (prev->next != NULL && prev->next != node) {
        prev = prev->next;
    }
    
    // If the node to be deleted was not found, return NULL
    if (prev->next == NULL) {
        return NULL;
    }
    
    // Update the previous node's next pointer to skip over the deleted node
    prev->next = node->next;
    node->next = NULL;
    return node;
}

//find node by its pid
struct sortedLinkedListNode* find(struct sortedLinkedListNode* headList, int pid) {
    struct sortedLinkedListNode* curr = headList;
    while (curr != NULL && curr->data->pid != pid)
    {
        curr = curr->next;
    }

    return curr;
}
//split node to two nodes sequential
struct sortedLinkedListNode* splitNode(struct sortedLinkedListNode* nodeToSplit,int memSize) {
    // Create a new node with part the data of the original node
    struct sortedLinkedListNode* newNode = (struct sortedLinkedListNode*) malloc(sizeof(struct sortedLinkedListNode));
    //alocate memoryNode -->data of LLNode
    newNode->data = (struct memoryNode*) malloc(sizeof(struct memoryNode));
    newNode->data->startLocation = nodeToSplit->data->startLocation;
    newNode->data->pid=nodeToSplit->data->pid;
    newNode->priority=nodeToSplit->priority;
    newNode->data->size=memSize;
    newNode->data->endLocation=newNode->data->startLocation+memSize;
    //Modify nodeToSplit data
    nodeToSplit->data->size=nodeToSplit->data->size-memSize;
    nodeToSplit->data->startLocation=newNode->data->endLocation;

    // Adjust the pointers to insert the new node between the original node and the next node
    newNode->next = nodeToSplit;
    // Return a pointer to the new node created by the split
    return newNode;
}
void reAllocateProcessMemory(struct ProcessStruct *Process){
struct memoryNode * memNode=createMemoryNode(memoryHoles->head->data->startLocation,Process->memSize,Process->pid);
struct sortedLinkedListNode* currNode=memoryHoles->head;
while(memNode->size >currNode->data->size && currNode!=NULL )
{
currNode=currNode->next;
}
if(currNode ==NULL)
{
    printf("NO memory available\n");
    return ;
}
if(memNode->size==currNode->data->size)
{
   struct sortedLinkedListNode* holedNode=removeLinkedListNode(currNode,memoryHoles);
   insert (memoryUsed,memNode,Process->priority);
}
else{
  struct sortedLinkedListNode* secCurrNode=splitNode(currNode,Process->memSize);
 // struct sortedLinkedListNode* holedNode=removeLinkedListNode(secCurrNode,memoryHoles);

    insert (memoryUsed,memNode,Process->priority);
}

}
bool isEmptyLL(struct sortedLinkedList *LL)
{
    return (LL->head == NULL);
}

void deAllocateProcessMemory(struct ProcessStruct *process )
{
  struct sortedLinkedListNode* rmNode;
  struct sortedLinkedListNode* llNode= find(memoryUsed->head,process->pid);
if(!isEmptyLL(memoryUsed)){  
  rmNode=removeLinkedListNode(llNode,memoryUsed);}
if(rmNode==NULL)
{
    printf("null");
}
  llNode->data->pid=-1;
 // memoryHoles->head->data->size=memoryHoles->head->data->size -rmNode->data->size;
  //memoryHoles->head->data->startLocation=memoryHoles->head->data->startLocation+rmNode->data->size;
  insert(memoryHoles,llNode->data,llNode->data->startLocation);
}

int main()
{
  memoryHoles=createSortedLinkedList();
  memoryUsed=createSortedLinkedList();
  struct memoryNode * memoryNode=createMemoryNode(0,100,-1);
  insert(memoryHoles,memoryNode,0);
struct ProcessStruct* process = (struct ProcessStruct*) malloc(sizeof(struct ProcessStruct));
 process->pid = 1;
    process->memSize = 50;
    process->priority = 1;
 reAllocateProcessMemory(process);
 if (memoryHoles->head->data->size == 50) {
        printf("Memory allocated to process\n");
    } else {
        printf("Error allocating memory\n");
    }
    struct ProcessStruct* process2 = (struct ProcessStruct*) malloc(sizeof(struct ProcessStruct));
    process2->pid = 2;
    process2->memSize = 10;
    process2->priority = 2;

    // Call the reAllocateProcessMemory function again
    reAllocateProcessMemory(process2);
     if (memoryHoles->head->data->size == 40) {
        printf("Memory split and allocated to process\n");
    } else {
        printf("Error splitting and allocating memory\n");
    }

struct sortedLinkedListNode* testNode=memoryHoles->head;
while(testNode !=NULL)
{
    printf("memoryHoles \n");
     printf("%d\n",testNode->data->size);
  //  printf("%d\n",testNode->data->startLocation);

     testNode=testNode->next;
}
struct sortedLinkedListNode* testNode1=memoryUsed->head;

  printf("memoryUsed \n");
while(testNode1 !=NULL)
{

     printf("%d\n",testNode1->data->size);
//    printf("%d\n",testNode1->data->startLocation);
     testNode1=testNode1->next;


}

printf("reintialize ");
fflush(stdout);
testNode=memoryHoles->head;
testNode1=memoryUsed->head;
printf("deallcotion ");
fflush(stdout);
//deAllocateProcessMemory(process);
deAllocateProcessMemory(process2);
printf("deallcotion ");
fflush(stdout);
printf("memoryHoles \n");

while(testNode !=NULL)
{
     printf("%d\n",testNode->data->size);
   // printf("%d\n",testNode->data->startLocation);

     testNode=testNode->next;
}
       printf("memoryUsed \n");

while(testNode1 !=NULL)
{

     printf("%d\n",testNode1->data->size);
//    printf("%d\n",testNode1->data->startLocation);
     testNode1=testNode1->next;
}
    return 0;
}
