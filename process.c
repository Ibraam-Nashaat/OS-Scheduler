#include "headers.h"
bool SRTNInteruptionFlag=0;
void SRTNInteruptionHandler(int signum){
    SRTNInteruptionFlag=1;
}
int main(int argc, char *argv[])
{
    int remainingTime, previousTime, currentTime, quantum,currQuantum;
    initClk();
    remainingTime = atoi(argv[1]);
    quantum=currQuantum=atoi(argv[2]);
    previousTime=getClk();
    signal(SIGRTMIN+4,SRTNInteruptionHandler);

    while (remainingTime > 0)
    {
        currentTime = getClk();
        if(SRTNInteruptionFlag){
            SRTNInteruptionFlag=0;
            previousTime=currentTime=getClk();
        }
        if (currentTime != previousTime )
            {
                remainingTime--;
                if(currQuantum!=-1)currQuantum--;
                if(currQuantum==0) {
                    kill(getppid(),SIGRTMIN+1);
                    kill(getpid(),SIGSTOP);
                    currQuantum=quantum;
                }
                currentTime = getClk();
            }
        previousTime = currentTime;
    }
    
    kill(getppid(), SIGUSR2);
    destroyClk(false);
    return 0;
}