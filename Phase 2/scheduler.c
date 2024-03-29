#include "math.h"
#include "headers.h"
#include "testing_functions.h"
#include "logFiles.h"
#include "firstFit.h"
#include "buddyAllocation.h"
#include "scheduling_algorithms.h"

int numberOfProcesses = 0;
int totalRunningTime = 0;

void pushProcessToWaitingQueue(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime, process.memSize);
        enqueue(waitingProcessesQueue, newProcess);
    }
}

void pushProcessToSRTN(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime, process.memSize);
        push(readyProcessesPriorityQueue, newProcess, newProcess->remainingTime);
    }
}

void pushProcessToHPF(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime, process.memSize);
        push(readyProcessesPriorityQueue, newProcess, newProcess->priority);
    }
}

void pushProcessToRR(struct ProcessStruct process)
{
    if (process.id != -1)
    {
        struct ProcessStruct *newProcess = create_process(process.id, process.arrivalTime, process.priority,
                                                          process.runningTime, process.memSize);
        enqueue(readyProcessesQueue, newProcess);
    }
}
bool tryAllocatingMemory(struct ProcessStruct process)
{
    if (memoryPolicy == 1)
    {
        return allocateProcessMemoryFirstFit(&process);
    }
    else
    {
        return allocateProcessMemoryBuddy(&process);
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

    numberOfProcesses++;
    totalRunningTime += message.process.runningTime;

    switch (selectedAlgorithm)
    {
    case 1:
        if (tryAllocatingMemory(message.process))
            pushProcessToHPF(message.process);
        else
        {
            printf("Pushing process %d to waiting queue\n", message.process.id);
            pushProcessToWaitingQueue(message.process);
        }
        break;
    case 2:
        if (tryAllocatingMemory(message.process))
        {
            pushProcessToSRTN(message.process);
            if (isRunning)
            {
                int tempRunningRime = runningProcess->remainingTime;
                int processRemainingtime = runningProcess->remainingTime - (getClk() - runningProcess->lastStartingTime);
                if (message.process.runningTime < processRemainingtime)
                    blockProcess();
                else
                    runningProcess->remainingTime = tempRunningRime;
            }
        }
        else
        {
            printf("Pushing process %d to waiting queue\n", message.process.id);
            pushProcessToWaitingQueue(message.process);
        }
        break;
    case 3:
        if (tryAllocatingMemory(message.process))
            pushProcessToRR(message.process);
        else
        {
            printf("Pushing process %d to waiting queue\n", message.process.id);
            pushProcessToWaitingQueue(message.process);
        }
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

void generatePerfFile()
{
    FILE *perfFile;
    perfFile = fopen("scheduler.perf", "w");

    fprintf(perfFile, "CPU utilization = %d%%\n", (totalRunningTime * 100 / getClk()));

    fprintf(perfFile, "Avg WTA = %.2f\n", (sumWeightedTAT / (float)numberOfProcesses));

    fprintf(perfFile, "Avg Waiting = %.2f\n", (totalWaitingTime / (float)numberOfProcesses));

    float meanWTA = sumWeightedTAT / (float)numberOfProcesses;
    float stdDev = sqrt(abs(((sumWeightedTAT) - (numberOfProcesses * pow(meanWTA, 2)))) / (float)numberOfProcesses);

    fprintf(perfFile, "Std WTA = %.2f\n", stdDev);

    fclose(perfFile);
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
    memoryPolicy = atoi(argv[3]);

    schedulerLogFile = fopen("scheduler.log", "w");
    fprintf(schedulerLogFile, "#At time x process y state arr w total z remain y wait k\n");

    memoryLogFile = fopen("memory.log", "w");
    fprintf(memoryLogFile, "#At time x allocated y bytes for process z from I to j\n");

    switch (memoryPolicy)
    {
    case FIRST_FIT_POLICY:
        memoryHoles = createSortedLinkedList();
        memoryUsed = createSortedLinkedList();
        struct memoryNode *memoryNode = createMemoryNode(0, 1024, -1);
        insert(memoryHoles, memoryNode, 0);
        break;

    case BUDDY_POLICY:
        buddyMemoryNode = createTreeNode(1024);
        break;

    default:
        printf("Invalid policy selected.\n");
        break;
    }

    waitingProcessesQueue = createQueue(); // Queue containing processes that can't be allocated

    // Switch on the selected algorithm and allocate the appropriate data structure
    switch (selectedAlgorithm)
    {
    case HPF_ALGORITHM:
        readyProcessesPriorityQueue = createPriorityQueue();
        HPF(readyProcessesPriorityQueue);
        break;
    case SRTN_ALGORITHM:
        readyProcessesPriorityQueue = createPriorityQueue();
        SRTN(readyProcessesPriorityQueue);
        break;
    case RR_ALGORITHM:
        readyProcessesQueue = createQueue();
        quantum = atoi(argv[2]);
        RR(readyProcessesQueue, quantum);
        break;
    default:
        printf("Invalid algorithm selected.\n");
        break;
    }

    fclose(schedulerLogFile);
    fclose(memoryLogFile);

    generatePerfFile();

    // Destroy the clock and exit
    destroyClk(false);
    return 0;
}