int algorithmFlag = 1;
int childProcessPID;
int selectedAlgorithm;
int quantum;
int currQuantum;
int previous_id;
bool isRunning;
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
    //runningProcess->lastStartedTime = getClk();
    if(runningProcess->pid != -1) // if the process started before , send SIGCONT to make it continue its execution
    {
        printf("Process with id %d and pid %d continued, clk %d\n", runningProcess->id,runningProcess->pid, getClk());
        kill(runningProcess->pid,SIGCONT);
        previous_id = runningProcess->id;
        return;
    }

    int pid = fork();
    if (pid == -1) {
        perror("Error in execl");
        exit(-1);
    }
    if(pid==0) // make new process
    {
        childProcessPID=getpid();
        printf("process %d at time %d\n", runningProcess->id, getClk());
        fflush(stdout);
        char remainigTimeChar[13];
        sprintf(remainigTimeChar, "%d", currProcess->remainingTime);
        char* argv []={"./process.out",remainigTimeChar,NULL}; // send remaining time as an argument
        int execlResult = execvp(argv[0], argv);
    }    
    runningProcess->pid = pid;
    previous_id = runningProcess->id;
}

/*
This function deletes the process and make isRunning=false to execute another process. 
*/
void terminateProcess(int sigNum)
{
    printf("process %d finished At=%d\n", runningProcess->id, getClk());
    free(runningProcess);
    runningProcess = NULL;
    isRunning=false;
    fflush(stdout);
}
/*
This function blocks the process, put it at the end of the queue and make isRunning=false to begin another process. 
*/
void blockProcess()
{
    kill(runningProcess->pid,SIGSTOP);
    if(selectedAlgorithm == 3)
        {            
            enqueue(queue,runningProcess);
        }
        else
        {
            push(priorityQueue,runningProcess,runningProcess->remainingTime);
        }
    
    printf("id = %d Blocked pid = %d remainingtime = %d current time = %d \n", runningProcess->id, runningProcess->pid, runningProcess->remainingTime, getClk());
    
    isRunning=false;
    fflush(stdout);
}

/*
This function decrement the currQuantum and remainingTime of the process and do the required checks to either block the process or make it continue.
*/
void processMadeOneClk(int sigNum)
{
    runningProcess->remainingTime--;
    if(selectedAlgorithm==3){
        currQuantum--;
        if (!currQuantum && runningProcess->remainingTime){ // in case RR
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
    }
}