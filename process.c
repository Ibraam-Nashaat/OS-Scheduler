#include "headers.h"


int main(int agrc, char * argv[])
{
    int remainingTime,previousTime,currentTime;
    initClk();
    previousTime=getClk();
    remainingTime=atoi(argv[1]);
    
    //TODO it needs to get the remaining time from somewhere
    while (remainingTime > 0)
    {
        currentTime=getClk();
        if(previousTime!=currentTime){
            remainingTime--;
        printf("previous time is %d, current time is %d and remaining time is %d\n",
                previousTime,currentTime,remainingTime);
        }
        previousTime=currentTime;
    }
    
    destroyClk(false);
    
    return 0;
}