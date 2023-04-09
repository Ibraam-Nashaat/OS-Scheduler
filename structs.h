struct ProcessStruct{
int id,
    arrivalTime,
    priority,
    runningTime;
};

struct QNode {
    struct ProcessStruct* data;
    struct QNode* next;
};

struct Queue {
    struct QNode *front, *rear;
};


//Priority Queue
struct PQNode {
    struct ProcessStruct* data;
    // Lower values indicate higher priority
    int priority;
    struct PQNode* next;
};
struct PQueue {
    struct PQNode *head;
};