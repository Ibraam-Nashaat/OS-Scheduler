// Handle the signal to indicate that a quantum has finished
// signum: the signal number
void quantumFinished(int signum){
  if (isEmptyQueue(queue)) // if the queue is empty, continue the running process
    kill(runningProcess->pid, SIGCONT);
  else // otherwise, block the running process
    blockProcess();
}

// Implement the RR algorithm
// q: a pointer to the queue of processes
// quant: the quantum time for RR algorithm
void RR(struct Queue *q, int quant)
{
    struct ProcessStruct *readyProcess;  // pointer for processes in queue 
    queue = q; // set the global queue variable to q
    quantum = quant; // set the global quantum variable to quant
    while (algorithmFlag || !isEmptyQueue(queue) || isRunning) { // while the algorithm flag is true or the queue is not empty or a process is running

        if (isEmptyQueue(queue)) // if the queue is empty, do nothing
            {
                continue;
            }
        if (!isRunning) {          // if no process is running and there are processes in queue, run the first one
            readyProcess = dequeue(queue); // get the first process from the queue
            runningProcess = readyProcess; // set the global running process variable to readyProcess
            runProcess(readyProcess, quantum); // run the ready process with quantum time
        }
    }
};