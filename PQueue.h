#define NULL ((void *)0)



struct PQNode* newPQNode(struct ProcessStruct* process ,int priority)
{
    struct PQNode * temp=(struct PQNode*)malloc(sizeof(struct PQNode));
    temp->data=process;
    temp->next=NULL;
    temp->priority=priority;
    return temp;
}

struct PQueue* createPriorityQueue()
{
    struct PQueue* pq=(struct PQueue*)malloc(sizeof(struct PQueue));
   //Intialize queue
    pq->head=NULL;
    pq->tail=NULL;
    return pq;
};
bool isEmptyPQ(struct PQueue * pq) {
    return (pq->head == NULL);
}


// enQueue method-> add node to Queue
void push(struct PQueue* pq,struct ProcessStruct* process, int priority)
{
    struct PQNode* PQnodehead= pq->head;
struct PQNode* nNode=newPQNode(process,priority);
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
    else if (priority > PQnodehead->priority)// if not try to rearrange itself 
    {
         while ((PQnodehead->next != NULL) && (PQnodehead->next->priority <= priority))
            PQnodehead = PQnodehead->next;
       nNode->next = PQnodehead->next;
        PQnodehead->next = nNode;
    }
}
}

//deQueue method-> remove node from queue and store its data in variable
struct ProcessStruct* pop (struct PQueue* pq)
{
    struct ProcessStruct* process=NULL;
    if(pq->head==NULL)
    {
        return process;
    }
struct PQNode* temp=pq->head;
pq->head= pq->head->next;
process =temp->data;
free(temp);
return process;
}
struct ProcessStruct* peek(struct PQueue* pq) {
    if (isEmptyPQ(pq))
        return NULL;
    return pq->head->data;
}