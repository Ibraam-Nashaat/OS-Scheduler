#include "math.h"
#include "headers.h"
#include "scheduling_algorithms.h"

int numberOfProcesses = 0;
int totalRunningTime = 0;



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
        pushProcessToHPF(message.process);
        break;
    case 2:
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
        break;
    case 3:
        checkIfProcessArrivedAtSameTimeOfQuantumFininshed(message.process);
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

    fprintf(perfFile,"Std WTA = %.2f\n", stdDev);

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

    logFile = fopen("scheduler.log", "w");
    fprintf(logFile, "#At time x process y state arr w total z remain y wait k\n");

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
    fclose(logFile);

    generatePerfFile();

    // Destroy the clock and exit
    destroyClk(false);
    return 0;
}