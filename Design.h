/*
    - Plan:
        1. Process Generator:
            1-  We will use message queue to send the the arrived processes to the schedular
            2-  In the main loop that consumes the process queue that is generated after reading the file,
                the process generator should send the process to the schedular when its time arrives by putting
                the process in the message queue,then it should send a signal to the schedular to allow it to read the message queue.
                It should down a semaphore to make sure that the schedular received the process (when the schedular receives the semaphore, it
                will up the semaphore again).
                NB: In step 2, if we didn't add the semaphore, it should work fine as if multiple signals were sent on the same signal handler,
                    the second signal would be blocked till the first signal finishes. But we added semaphore to ensure synchronization.
            3-  When the process generator finished its queue of processes,it will send a signal to the schedular to change a flag(this flag informes
                the schedular that process generator has finished working and is waiting till the schedular finishes all the ready and running processes).
                -There is 2 ways to make change the schedular flag:
                    1)make a special processID of ID=-1, and send it to the schedular(so that when the schedular sees it, it would change its flag)
                    2)Send a signal to the schedular, such that the schedular signal handler change the flag(I think this way is better)
            4-  The clock resources should be cleared after making sure that the schedular has terminated(waitpid till the schedular terminates)
 
        2. Schedular:
            1-  It should create the queue or priority queue according to the specified algorithm by the user.
            2-  When it receives the process from the message queue(based on a signal from the process generator), it should add it to the approprate queue or priority queue of the algo.

        3. The Schedular Algo(ex:HPF,RR):
            1-  In a while loop,it will dequeue the process from the queue or priority queue
            2-  It forks a child process, then using execvp the child will be switched to a running instance of the process.
            3-  In the while loop,after the dequeue of the process, it will down the semaphore(to be blocked till the process finishes executing)
            4-  When the process finishes,It will send a signal to the schedular algo indicating that it finished.
            5-  The sent signal will up the semaphore to allow the schedular algo to dequeue a new process
            6-  while(!queue.empty() || flag), where the flag indicates the process generator has emptied its queue.


    - Signals:
        1. From process generator to schedular: 1-One signal to inform the schedular that a process was put in the message queue.
                                            2-One signal to inform the schedular that there is no more processes will be sent to it by changing the schedular flag.
        2. From the process to the schedular algo:When the process finishes executing, it sends a signal to the schedular algo to inform it. 
    
    - IPC:
        1. Message Queues:
            1-  One message queue from process generator to schedular.
            

        2. Semaphores:
            1-  Between process generator and schedular:The process generator will be blocked at down,till the schedular makes it up after receiving the 
                                                        the process from the message queue and add it to the appropriate data structure of the the schedular algo
            2-  Between schedular algo and itself:The schedular algo will be blocked in down till the process finishes executing.The signal sent
                                                  from the process after finishing executing will make the semaphore up again to allow the schedular algo to
                                                  be unblocked to dequeue a new process.
    */