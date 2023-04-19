#include "headers.h"
int remainingTime, previousTime, currentTime, quantum;
void resumeProcessHandler(int signum){
    previousTime=currentTime=getClk();
}
int main(int argc, char *argv[])
{
    initClk();
    remainingTime = atoi(argv[1]);
    previousTime=getClk();
    signal(SIGCONT,resumeProcessHandler);

    while (remainingTime > 0)
    {
        currentTime = getClk();
        if (currentTime != previousTime )
            {
                remainingTime--;
                kill(getppid(), SIGRTMIN+1);
                currentTime = getClk();
            }
        previousTime = currentTime;
    }
    
    kill(getppid(), SIGUSR2);
    destroyClk(false);
    return 0;
}