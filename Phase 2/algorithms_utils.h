#include "defines.h"
int algorithmFlag = 1;
int algorithmBlockingFlag = 1; // for handling processes that arrive at the same time
int selectedAlgorithm, quantum, memoryPolicy;
bool isRunning;
struct msgBuff message;
struct PQueue *processRunningPriorityQueue;
struct Queue *processRunningQueue, *processWaitingQueue;
struct ProcessStruct *runningProcess = NULL;
struct sortedLinkedList *memoryHoles, *memoryUsed;

// Run a process with a given quantum time
// currProcess: a pointer to the process structure
// quantum: the quantum time for RR algorithm
void runProcess(struct ProcessStruct *currProcess, int quantum)
{
    runningProcess = currProcess;
    isRunning = true;
    // runningProcess->lastStartedTime = getClk();
    if (runningProcess->pid != -1) // if the process started before, send CONTINUE_PROCESS signal to make it continue its execution
    {
        printf("Process with id %d and pid %d continued, clk %d\n", runningProcess->id, runningProcess->pid, getClk());
        kill(runningProcess->pid, CONTINUE_PROCESS);
        return;
    }
    int pid = fork();
    if (pid == -1)
    {
        perror("Error in fork");
        exit(-1);
    }
    if (pid == 0) // make new process
    {
        printf("process %d started at time %d \n", runningProcess->id, getClk());
        fflush(stdout);
        char remainigTimeChar[13];
        sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
        char quantumChar[13];
        sprintf(quantumChar, "%d", quantum);
        char *argv[] = {"./process.out", remainigTimeChar, quantumChar, NULL}; // send remaining time and quantum as arguments
        int execlResult = execvp(argv[0], argv);
        if (execlResult == -1)
        {
            perror("Error in execvp");
            exit(-1);
        }
    }
    runningProcess->pid = pid;
    runningProcess->startTime = getClk();
    runningProcess->lastStopedTime = getClk();
}
void deAllocateProcessMemory(struct ProcessStruct *process)
{
    struct memoryNode *memoryRemovedNode;
    struct sortedLinkedListNode *llNode = find(memoryUsed->head, process->pid);
    if (!isEmptyLL(memoryUsed))
    {
        memoryRemovedNode = removeLinkedListNode(llNode, memoryUsed);
    }
    if (memoryRemovedNode == NULL)
    {
        printf("null\n");
        return;
    }
    memoryRemovedNode->pid = -1;

    insert(memoryHoles, memoryRemovedNode, memoryRemovedNode->startLocation);
    mergeHoles(memoryHoles);
    printMemory(memoryHoles,"Memory Holes after Deallocation");
    printMemory(memoryUsed,"Memory Used after Deallocation");
}

bool allocateProcessInMemory(struct ProcessStruct *Process)
{
    struct memoryNode *memNode = createMemoryNode(memoryHoles->head->data->startLocation, Process->memSize, Process->pid);
    struct sortedLinkedListNode *currNode = memoryHoles->head;

    fflush(stdout);
    while (currNode != NULL && memNode->size > currNode->data->size)
    {
        currNode = currNode->next;
    }
    if (currNode == NULL)
    {
        printf("NO memory available\n");
        printMemory(memoryHoles,"Memory Holes after allocation");
        printMemory(memoryUsed,"Memory Used after allocation");
        return false;
    }
    if (memNode->size == currNode->data->size)
    {
        // struct sortedLinkedListNode* holedNode=removeLinkedListNode(currNode,memoryHoles);
        insert(memoryUsed, memNode, Process->priority);
    }
    else
    {
        struct sortedLinkedListNode *secCurrNode = splitNode(currNode, Process->memSize);
        // struct sortedLinkedListNode* holedNode=removeLinkedListNode(secCurrNode,memoryHoles);

        insert(memoryUsed, memNode, Process->priority);
    }

    printMemory(memoryHoles,"Memory Holes after allocation");
    printMemory(memoryUsed,"Memory Used after allocation");
    return true;
}
// This function terminates the process and frees the memory
// sigNum: the signal number for termination
void terminateProcess(int sigNum)
{
    deAllocateProcessMemory(runningProcess);
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    runningProcess = NULL;
    isRunning = false;
    fflush(stdout);
    struct ProcessStruct *waitingProcess = peekQueue(processWaitingQueue);
    if (waitingProcess!=NULL && allocateProcessInMemory(waitingProcess))
    {
        waitingProcess = dequeue(processWaitingQueue);
        if (selectedAlgorithm == 1)
            push(processRunningPriorityQueue, waitingProcess, waitingProcess->priority);
        else if (selectedAlgorithm == 2)
            push(processRunningPriorityQueue, waitingProcess, waitingProcess->remainingTime);
        else if (selectedAlgorithm == 3)
            enqueue(processRunningQueue, waitingProcess);
        
    }
}

// This function blocks the process, puts it at the end of the queue or the priority queue depending on the algorithm, and makes isRunning=false to begin another process.
void blockProcess()
{
    if (selectedAlgorithm == 3)
    {
        // For round robin algorithm, enqueue the process at the end of the queue
        // queue: a pointer to the queue structure
        // runningProcess: a pointer to the process structure
        enqueue(processRunningQueue, runningProcess);
    }
    else
    {
        // For shortest remaining time next algorithm, update the last stopped time and the remaining time of the process, and push it to the priority queue
        // priorityQueue: a pointer to the priority queue structure
        // runningProcess: a pointer to the process structure
        runningProcess->lastStopedTime = getClk();
        kill(runningProcess->pid, INTERRUPT_SIGNAL);
        kill(runningProcess->pid, SIGSTOP);
        push(processRunningPriorityQueue, runningProcess, runningProcess->remainingTime);
    }
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    isRunning = false;
    fflush(stdout);
}