int algorithmFlag = 1;
int childProcessPID;
int selectedAlgorithm;
int quantum;
int currQuantum;
int previous_id;
bool isRunning;
bool isBlocked;
struct msgBuff message;
struct PQueue *priorityQueue;
struct Queue *queue;
struct ProcessStruct * runningProcess = NULL;
struct ProcessStruct * previousProcess = NULL;

#define QUANTUM_ONE 1;

/*

At time 1 process 1 started arr 1 total 6 remain 6 wait 0
At time 3 process 1 stopped arr 1 total 6 remain 4 wait 0
At time 3 process 2 started arr 3 total 3 remain 3 wait 0
At time 6 process 2 finished arr 3 total 3 remain 0 wait 0 TA 3 WTA 1
At time 6 process 1 resumed arr 1 total 6 remain 4 wai t 3
At time 10 process 1 finished arr 1 total 6 remain 0 wait 3 TA 10 WTA 1.67

*/

void runProcess(struct ProcessStruct *currProcess)
{
    runningProcess=currProcess;
    isRunning = true;
    isBlocked = false;
    //runningProcess->lastStartedTime = getClk();
    if(runningProcess->pid != -1)
    {
      //  if(runningProcess->id != previous_id)
        printf("Process with id %d continued, clk %d\n", runningProcess->id, getClk());
        kill(runningProcess->pid,SIGCONT);
        previous_id = runningProcess->id;
        return;
    }

    int pid = fork();
    if (pid == -1) {
        perror("Error in execl");
        exit(-1);
    }
    if(pid==0)
    {
        childProcessPID=getpid();
        printf("process %d at time %d\n", runningProcess->id, getClk());
        char remainigTimeChar[13];
        sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
        char* argv []={"./process.out",remainigTimeChar,NULL};
        int execlResult = execvp(argv[0], argv);
    }    
    runningProcess->pid = pid;
    previous_id = runningProcess->id;
}

//terminate process ->make isRunning false and free process

void terminateProcess(int sigNum)
{
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    runningProcess = NULL;
    isRunning=false;
    isBlocked=false;
    signal(SIGALRM, terminateProcess);
    //printf("algo flad %d , isemptyQ %d , isRunning %d\n", algorithmFlag, !isEmptyPQ(priorityQueue), isRunning);
    fflush(stdout);
}

void blockProcess()
{
    if(selectedAlgorithm == 3)
        {            
            enqueue(queue,runningProcess);
        }
        else
        {
            push(priorityQueue,runningProcess,runningProcess->remainingTime);
        }
    
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    
    isBlocked=true;    
    isRunning=false;
    fflush(stdout);
}

void processMadeOneClk(int sigNum)
{
    signal(SIGUSR2,processMadeOneClk);
    runningProcess->remainingTime--;
    currQuantum--;

    //printf("remaining time %d\n",runningProcess->remainingTime);
    if(!runningProcess->remainingTime){
        // nothing to do
        kill(runningProcess->pid,SIGCONT);
    }
    else if (!currQuantum){
        if(!isEmptyQueue(queue))
            {
                blockProcess();
            }
        else
            {
                currQuantum=quantum;
                kill(runningProcess->pid,SIGCONT);
            }        
    } 
    else if(selectedAlgorithm == 2 && !isEmptyPQ(priorityQueue) && peek(priorityQueue)->runningTime < runningProcess->remainingTime)
        blockProcess();
    else
    {
        kill(runningProcess->pid,SIGCONT);
    }    
}