
struct ProcessStruct * runningProcess;
struct ProcessStruct * previousProcess = NULL;
struct PQueue *processQueueSRTN; // pointer for PQueue
bool isRunning = false;
bool isBlocked = false;
const int QUANTUM_ONE = 1;

void runProcessSRTN(struct ProcessStruct *currProcess, bool sameProcess)
{
    runningProcess=currProcess;
    isRunning=true;
    isBlocked = false;
    int pid = fork();
    if (pid == -1) {
        perror("Error in execl");
        exit(-1);
    }
    if(pid==0)
    {
        childProcessPID=getpid();
        if(!sameProcess)
            printf("id =  %d started pid = %d current time = %d \n", currProcess->id, getpid(), getClk());
        char remainigTimeChar[13];
        char quantumChar[13];
        sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
        sprintf(quantumChar, "%d", -1 * QUANTUM_ONE);
        char* argv []={"./process.out",remainigTimeChar, quantumChar,NULL};
        int execlResult = execvp(argv[0], argv);
    }
    runningProcess->remainingTime -= QUANTUM_ONE;
    previousProcess = runningProcess;
}
//terminate process ->make isRunning false and free process
void terminateProcessSRTN(int sigNum)
{
    isRunning = false;
    isBlocked = false;
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    //signal(SIGUSR2, terminateProcess);
    fflush(stdout);
    previousProcess = NULL;
}

void BlockProcessSRTN(int sigNum)
{
    isRunning=false;
    isBlocked = true;
    //printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, getpid(), runningProcess->remainingTime, getClk());
    push(processQueueSRTN,runningProcess,runningProcess->remainingTime);
    fflush(stdout);
}

 
void SRTN(struct PQueue *pQueue)
{
    struct ProcessStruct *readyProcess;  //pointer for processes in Pqueue 
    processQueueSRTN = pQueue;
    
    while (algorithmFlag||!isEmptyPQ(processQueueSRTN) || isRunning || isBlocked) { // while queue isn't empty or runung
        if (isEmptyPQ(processQueueSRTN)) // queue is empty doesn't make thing
            continue;
        if (!isRunning && algorithmBlockingFlag) {          // isn't running and in Pqueue --> run it  
            readyProcess = peek(processQueueSRTN);
            pop(processQueueSRTN);
            runningProcess=readyProcess;
            bool sameProcess = true;
            if(previousProcess != NULL && runningProcess != NULL && runningProcess->id != previousProcess->id)
                {
                    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", previousProcess->id, getpid(), previousProcess->remainingTime, getClk());
                    sameProcess = false;
                }
            if(previousProcess == NULL)
                sameProcess = false;    
            runProcessSRTN(readyProcess, sameProcess);
        }
    }
};