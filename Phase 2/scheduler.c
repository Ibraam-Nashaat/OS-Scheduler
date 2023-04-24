#include "headers.h"
#include "scheduling_algorithms.h"

void pushProcessToWaitingQueue(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime,process.memSize);
        enqueue(processWaitingQueue, newProcess);
    }
}

void pushProcessToSRTN(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime,process.memSize);
        push(priorityQueue, newProcess, newProcess->remainingTime);
    }
}

void pushProcessToHPF(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime,process.memSize);
        push(priorityQueue, newProcess, newProcess->priority);
    }
}

void pushProcessToRR(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime,process.memSize);
        enqueue(queue, newProcess);
    }
}
bool firstFitMemoryAllocation(struct ProcessStruct* process, struct sortedLinkedListNode* memoryList){

    
    struct sortedLinkedListNode* curr = memoryList;
    while (curr != NULL) {
        if (curr->data->size >= process->memSize) {
            // Allocate the memory by updating the block's PID and splitting the block if necessary
            curr->data->pid = process->pid;
            if (curr->data->size > process->memSize) {
                struct sortedLinkedListNode* newNode = (struct sortedLinkedListNode*) malloc(sizeof(struct sortedLinkedListNode));
                newNode->data->pid = -1;
                newNode->data->size = curr->data->size - process->memSize;
                newNode->next = curr->next;
                curr->data->size = process->memSize;
                curr->next = newNode;
            }
            return true;
        }
        curr = curr->next;
    }
    // If no block is found, return false
    return false;
  //replace by algorithm implementation
}
bool tryAllocatingMemory(struct ProcessStruct process){
    if(memoryPolicy==1) //FirstFit
        return firstFirstMemoryAllocation(process);
    else {
        return 1;       //change after implementing buddy memory allocation
    }
}
void getProcess(int signum)
{
    int rec_val = msgrcv(messageQueueID, &message, sizeof(message.process), 5, !IPC_NOWAIT);

    printf("Scheduler Received Process with ID=%d\n", message.process.id);
    fflush(stdout);

    if (rec_val == -1)
    {
        perror("Error in receiving from msg queue\n");
    }

    switch (selectedAlgorithm)
    {
    case 1:
        if(tryAllocatingMemory(message.process)) pushProcessToHPF(message.process);
        else pushProcessToWaitingQueue(message.process);
        break;
    case 2:
        if(tryAllocatingMemory(message.process)) {
            pushProcessToSRTN(message.process);
            if (isRunning)
            {
                int tempRunningRime = runningProcess->remainingTime;
                runningProcess->remainingTime = runningProcess->remainingTime - (getClk() - runningProcess->lastStopedTime);
                if (message.process.runningTime < runningProcess->remainingTime)
                    blockProcess();
                else
                    runningProcess->remainingTime = tempRunningRime;
            }
        }
        else pushProcessToWaitingQueue(message.process);
        break;
    case 3:
        if(tryAllocatingMemory(message.process)) pushProcessToRR(message.process);
        else pushProcessToWaitingQueue(message.process);
        break;
    }

    // Process has been pushed to the queue
    // Up the semaphore to allow process generator to continue
    up(processGeneratorAndSchedulerSemID);

    // check if that process was the terminating one (id = -1)
    if (message.process.id == -1)
    {
        algorithmFlag = 0;
    }
}
void changeAlgorithmFlag(int sigNum)
{
    algorithmFlag = 0;
}
void changeBlockingFlag(int signum)
{
    algorithmBlockingFlag = !algorithmBlockingFlag;
}

int main(int argc, char *argv[])
{
    // Register the signal handlers
    signal(GET_PROCESS, getProcess);
    signal(CHANGE_ALGORITHM, changeAlgorithmFlag);
    signal(PROCESS_FINISHED_SIGNAL, terminateProcess);
    signal(QUANTUM_FINISHED, quantumFinished);
    signal(CHANGE_BLOCKING, changeBlockingFlag);

    // Initialize the clock
    initClk();

    // Create the semaphore and the message queue
    createSemaphore();
    createMessageQueue();

    // Get the selected algorithm from the command line argument
    selectedAlgorithm = atoi(argv[1]);
    memoryPolicy=atoi(argv[3]);
    printf("Selected Algorithm: %d\n", selectedAlgorithm);
    fflush(stdout);
    if(memoryPolicy==1){
        memoryHoles=createSortedLinkedList();
        memoryUsed=createSortedLinkedList();
        struct memoryNode * memoryNode=createMemoryNode(0,1024,-1);
        insert(memoryHoles,memoryNode,0);
    }

    processWaitingQueue=createQueue(); //Queue containing processes that can't be allocated

    // Switch on the selected algorithm and allocate the appropriate data structure
    switch (selectedAlgorithm)
    {
    case HPF_ALGORITHM:
        priorityQueue = createPriorityQueue();
        HPF(priorityQueue);
        break;
    case SRTN_ALGORITHM:
        priorityQueue = createPriorityQueue();
        SRTN(priorityQueue);
        break;
    case RR_ALGORITHM:
        queue = createQueue();
        quantum = atoi(argv[2]);
        RR(queue, quantum);
        break;
    default:
        printf("Invalid algorithm selected.\n");
        break;
    }

    // Destroy the clock and exit
    destroyClk(false);
    return 0;
}