#define NULL ((void *)0)

// create queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    // Intialize queue
    q->head = NULL;
    q->tail = NULL;
    q->size=0;
    return q;
};
// intailiaze new node
struct Node *newQueueNode(struct ProcessStruct *process)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->data = process;
    temp->next = NULL;
    return temp;
}

// isEmptyPQ methodknown by head pointer
bool isEmptyQueue(struct Queue *q)
{
    if (q->head == NULL)
    {
        return true;
    }
    else
        return false;
}

// enQueue method-> add node to Queue
void enqueue(struct Queue *q, struct ProcessStruct *process)
{
    struct Node *nNode = newQueueNode(process);
    q->size++;
    // codition if queue is empty
    if (q->head == NULL)
    {
        q->tail = q->head = nNode;
        return;
    }
    else
    {
        // if queue isn't empty make Next Pointer of tail implies to newNode
        q->tail->next = nNode;
        // move tail pointer to newNode
        q->tail = nNode;
    }
}
// deQueue method-> remove node from queue and store its data in variable
struct ProcessStruct *dequeue(struct Queue *q)
{
    struct ProcessStruct *process = NULL;
    if (q->head == NULL)
    {
        return process;
    }
    struct Node *temp = q->head;
    q->head = q->head->next;
    process = temp->data;
    q->size--;
    free(temp);
    return process;
}
struct ProcessStruct *peekQueue(struct Queue *q)
{
    if (isEmptyQueue(q))
        return NULL;
    return q->head->data;
}
struct ProcessStruct *peekQueueTail(struct Queue *q)
{
    if (isEmptyQueue(q))
        return NULL;
    return q->tail->data;
}
int queueSize(struct Queue *q){
    return q->size;
}
