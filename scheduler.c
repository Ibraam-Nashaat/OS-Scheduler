#include "headers.h"

int flag = 1;
int selectedAlgorithm;

struct msgBuff message;
struct PQueue *priorityQueue;
struct Queue *queue;

struct ProcessStruct *create_process(int id, int arrivalTime, int priority, int runningTime)
{
    struct ProcessStruct *process = (struct ProcessStruct *)malloc(sizeof(struct ProcessStruct));
    process->id = id;
    process->arrivalTime = arrivalTime;
    process->priority = priority;
    process->runningTime = runningTime;
    return process;
}

void pushProcessToSRTN(struct ProcessStruct process)
{
    printf("Added Process to SRTN");
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        push(priorityQueue, newProcess, newProcess->runningTime);
    }
}

void pushProcessToHPF(struct ProcessStruct process)
{
    printf("Added Process to HPF");
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        push(priorityQueue, newProcess, newProcess->priority);
    }
}

void pushProcessToRR(struct ProcessStruct process)
{
    printf("Added Process to RR");
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        enQueue(queue, newProcess);
    }
}

void getProcess(int signum)
{
    printf("\nANAAA ZHEEEEEE2TTTTTT");
    int rec_val = msgrcv(messageQueueID, &message, sizeof(message.process), 5, !IPC_NOWAIT);

    printf("Scheduler Received Process with ID=%d", message.process->id);

    if (rec_val == -1)
    {
        perror("Error in receiving from msg queue");
    }

    switch (selectedAlgorithm)
    {
    case 1:
        pushProcessToHPF(*message.process);
        break;
    case 2:
        pushProcessToSRTN(*message.process);
        break;
    case 3:
        pushProcessToRR(*message.process);
        break;
    }

    // Process has been pushed to the queue
    // Up the semaphore to allow process generator to continue
    up(processGeneratorAndSchedulerSemID);

    // check if that process was the terminating one (id = -1)
    if (message.process->id == -1)
    {
        flag = 0;
    }
}

void createMessageQueue(){
    messageQueueID = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    if (messageQueueID== -1)
    {
        perror("Error in creating message queue");
        exit(-1);
    }
}

void createSemaphore(){
    processGeneratorAndSchedulerSemID= semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (processGeneratorAndSchedulerSemID== -1)
    {
        perror("Error in creating semaphore");
        exit(-1);
    }
    

    union Semun semun;
    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(processGeneratorAndSchedulerSemID, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, getProcess);
    initClk();
    createSemaphore();
    createMessageQueue();

    selectedAlgorithm = atoi(argv[1]);
    printf("GA3FAR EL 3OMDA %d", selectedAlgorithm);

    switch (selectedAlgorithm) {
        case 1:
            // Allocate the priority queue
            priorityQueue = createPriorityQueue();
            //while(1){};
            break;
        case 2:
            // Allocate the priority queue
            priorityQueue = createPriorityQueue();
            //while(1){};
            break;
        case 3:
            // Allocate the queue
            queue = createQueue();
            //while(1){};
            break;
    }

    while(1){};

    //sleep(20);

    destroyClk(false);
}
