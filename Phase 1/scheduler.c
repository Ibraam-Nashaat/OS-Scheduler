#include "headers.h"
#include "scheduling_algorithms.h"

void pushProcessToSRTN(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        push(priorityQueue, newProcess, newProcess->remainingTime);
    }
}

void pushProcessToHPF(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        push(priorityQueue, newProcess, newProcess->priority);
    }
}

void pushProcessToRR(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime);
        enqueue(queue, newProcess);
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
        pushProcessToHPF(message.process);
        break;
    case 2:
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
        break;
    case 3:
        pushProcessToRR(message.process);
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
    printf("Selected Algorithm: %d\n", selectedAlgorithm);
    fflush(stdout);

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