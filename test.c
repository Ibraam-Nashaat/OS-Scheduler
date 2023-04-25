#define NULL ((void *)0)

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
    struct sortedLinkedListNode* current=LL->head;
    struct sortedLinkedListNode* previous=NULL;
    if(current!=NULL && current->next==NULL){
        free(current);
        LL->head=NULL;
        LL->tail==NULL;
        return current;
    }
    if(current!=NULL && current==node){
        LL->head=current->next;
        free(current);
        return current;
    }
    while(current!=NULL && current!=node){
        previous=current;
        current=current->next;
    }
    if(current==NULL) return NULL;
    previous->next=current->next;
    free(current);
    return current;
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
  struct sortedLinkedListNode* holedNode=removeLinkedListNode(secCurrNode,memoryHoles);
    
    insert (memoryUsed,memNode,Process->priority);
}

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


    return 0;
}
