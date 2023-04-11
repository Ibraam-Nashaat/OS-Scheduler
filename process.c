#include "headers.h"


int main(int agrc, char * argv[])
{
    int remainingTime,previousTime,currentTime;
    initClk();
    previousTime=getClk();
    remainingTime=atoi(argv[1]);

    while (remainingTime > 0)
    {
        currentTime=getClk();
        if(previousTime!=currentTime){
            remainingTime--;
        }
        previousTime=currentTime;
    }
    kill(getppid(),SIGUSR2);
    
    destroyClk(false);
    
    return 0;
}