#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_PROCESSES 10
#define MAX_LEVELS 3

typedef struct process
{
    int id;
    int arrival_time;
    int burst_time;
    int time_slice;
    int io_time;
    int io_completion_time;
    int turnaround_time;
    int waiting_time;
    int completed;
    int first_run;
} process_t;


typedef struct queue
{
    process_t processes[MAX_PROCESSES];
    int num_processes;
    int quantum;
} queue_t;
 queue_t queues[MAX_LEVELS];



void mlfq(process_t *processes, int num_processes);
int main()
{
    // Create an array of processes
    process_t processes[] = {
        {1, 0, 10,0, 3, 0, 0, 0},
        {2, 2, 6,0,0, 0, 0, 0},
        {3, 0, 8,0, 4, 0, 0},
        {4, 4, 15,0, 1, 3, 0, 0},
        {5, 3, 7,0, 0, 0, 0, 0}};
    int num_processes = sizeof(processes) / sizeof(processes[0]);

    // Run the MLFQ scheduling algorithm
    mlfq(processes, num_processes);

    // Print the results for each process
    printf("Process\tArrival Time\tBurst Time\tTurnaround Time\tWaiting Time\n");
    for (int i = 0; i < num_processes; i++)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].arrival_time, processes[i].burst_time, processes[i].turnaround_time, processes[i].waiting_time);
    }

    return 0;
}

// this function is to check if there's a job that it's time came in higher queues
// recevies the current level;
int checkHigherPriority(int t,int level){
    if (level==0){
        for(int j=0; j<queues[level].num_processes; j++){
            if(queues[level].processes[j].arrival_time<=t)return j;
        }
        return -1;
    }
    for(int i=0; i<level;i++){
        for(int j=0; j<queues[level].num_processes; j++){
            if(queues[level].processes[j].arrival_time<=t)return j;
        }
    }
    return -1;
}

 void mlfq(process_t *processes, int num_processes)
{

    int time = 0;
    int level = 0;
    int current_process = 0;
    int completed_processes = 0;
    // int time_slice = 0;

    // Initialize all queues to be empty
    for (int i = 0; i < MAX_LEVELS; i++)
    {
        queues[i].num_processes = 0;
        for (int j = 0; j < MAX_PROCESSES; j++)
        {
            queues[i].processes[j].burst_time = -1;
            queues[i].processes[j].time_slice=0;
            queues[i].processes[j].first_run=-1;

        }
    }

    // Set the quantum for each queue
    queues[0].quantum = 1; // Set the quantum for the highest priority queue to 1
    queues[1].quantum = 3; // Set the quantum for the next priority queue to 4
    queues[2].quantum = 6; // Set the quantum for the next priority queue to 8
    // Set the quantum for the remaining queues as desired

    // Add all processes to the high priority queue

    for (int i = 0; i < num_processes; i++)
    {
        queues[0].processes[queues[0].num_processes++] = processes[i];
    }

    // While there are still processes to complete
    while (completed_processes < num_processes)
    {
        // Increment time
        ///time++;
                // printf("Queue %d Starts Executing...\n",level+1);

       

        // If the current queue is not empty
        if (queues[level].num_processes > 0)
        {
             /* // loop in queue 1 to check if there's a process that arrived at -time-
                int flag=0;
             //   queue_t cur_queue=queues[level];
                for(int i=0; i<queues[level].num_processes;i++){
                    if(queues[level].processes[i].arrival_time <=time){ // check for any process that arrived at this time
                      current_process=i;   // if yes , get the index of it , if no go to the next priority level
                            flag=1; // to indicate that there's a valid process in this queue
                            break;
                    }
                } 
                if(!flag){
                    level++;
                    continue;
                } */

                current_process=checkHigherPriority(time,0);
                if(current_process==-1){
                    level++;
                    continue;
                } 
           if(queues[level].processes[current_process].first_run==-1) queues[level].processes[current_process].first_run=time;
            // Increment the waiting time for all processes in the current queue
            for (int i = 0; i < queues[level].num_processes; i++)
            {
                if (i != current_process)
                {
                    queues[level].processes[i].waiting_time++;
                }
            }
            // first we check if we're going to execute it normally  or does it has io so we move it the last of the queue
            if(queues[level].processes[current_process].io_time==0)  // if true , this means that it's cpu-intensive process
            {
                      if(level) // if we are not on higher priority
                      {
                        for(int i=0; i<queues[level].quantum && checkHigherPriority(time,level); i++){
                            queues[level].processes[current_process].burst_time-=1; // decrease the burst time by quantum                
                             queues[level].processes[current_process].time_slice+=1;
                             time+=1;
                           printf("Process %d Executing....\n",queues[level].processes[current_process].id);

                                       // If the current process has completed
                            if (queues[level].processes[current_process].burst_time == 0)
                                {
                                    // Set the turnaround time and mark the process as completed
                                    queues[level].processes[current_process].turnaround_time = time-queues[level].processes[current_process].arrival_time;
                                    queues[level].processes[current_process].completed = 1;
                                    completed_processes++;
                                printf("Process %d Completed Execution with Turn Around Time of :%d ....\n",queues[level].processes[current_process].id,queues[level].processes[current_process].turnaround_time);

                                    // Remove the completed process from the queue
                                    for (int i = current_process; i < queues[level].num_processes - 1; i++)
                                    {
                                        queues[level].processes[i] = queues[level].processes[i + 1];
                                    }
                                    queues[level].num_processes--;
                                    current_process--;
                                }
                                // If the time slice has been exceeded but not finished(so it's gonna be moved to the next queue)
                                else if (queues[level].processes[current_process].time_slice == queues[level].quantum)
                                {
                                    // Move the current process to the next queue
                                    queues[level + 1 ].processes[queues[level + 1].num_processes++] = queues[level].processes[current_process];
                                    printf("Process %d  Moved to Queue %d\n",queues[level].processes[current_process].id,level+2);
                                    // Remove the process from the current queue
                                    for (int i = current_process; i < queues[level].num_processes - 1; i++)
                                    {
                                        queues[level].processes[i] = queues[level].processes[i + 1];
                                    }
                                    queues[level].num_processes--;
                                //  current_process--;
                                }
                                //current_process++;

                                // If the current process has exceeded the number of processes in the current queue
                                if (current_process == queues[level].num_processes)
                                {
                                    // Reset the current process to the beginning of the queue
                                    current_process = 0;
                                    }
                                
                    
                        }
                    } else{
                             queues[level].processes[current_process].burst_time-=1; // decrease the burst time by quantum                
                              queues[level].processes[current_process].time_slice+=1;                                   
                             time+=1;
                             printf("Process %d Executing....\n",queues[level].processes[current_process].id);

                                  // If the current process has completed
                    if (queues[level].processes[current_process].burst_time == 0)
                    {
                        // Set the turnaround time and mark the process as completed
                        queues[level].processes[current_process].turnaround_time = time-queues[level].processes[current_process].arrival_time;
                        queues[level].processes[current_process].completed = 1;
                        completed_processes++;
                    printf("Process %d Completed Execution with Turn Around Time of :%d ....\n",queues[level].processes[current_process].id,queues[level].processes[current_process].turnaround_time);

                        // Remove the completed process from the queue
                        for (int i = current_process; i < queues[level].num_processes - 1; i++)
                        {
                            queues[level].processes[i] = queues[level].processes[i + 1];
                        }
                        queues[level].num_processes--;
                        current_process--;
                    }
                    // If the time slice has been exceeded but not finished(so it's gonna be moved to the next queue)
                    else if (queues[level].processes[current_process].time_slice == queues[level].quantum)
                    {
                        // Move the current process to the next queue
                        queues[level + 1].processes[queues[level + 1].num_processes++] = queues[level].processes[current_process];
                        printf("Process %d  Moved to Queue %d\n",queues[level].processes[current_process].id,level+2);
                        // Remove the process from the current queue
                        for (int i = current_process; i < queues[level].num_processes - 1; i++)
                        {
                            queues[level].processes[i] = queues[level].processes[i + 1];
                        }
                        queues[level].num_processes--;
                      //  current_process--;
                    }
                    //current_process++;

                    // If the current process has exceeded the number of processes in the current queue
                    if (current_process == queues[level].num_processes)
                    {
                        // Reset the current process to the beginning of the queue
                        current_process = 0;
                        }
                    }
                
                }
                
                else { // it's not cpu intensive process 
                    if(! queues[level].processes[current_process].io_completion_time){ // if io completion wasn't set ,set it
                        queues[level].processes[current_process].io_completion_time=time+ queues[level].processes[current_process].io_time;
                            // move it to the last of the queue
                                //increasing time slice one by one 
                                queues[level].processes[current_process].time_slice++;
                                time++;
                                process_t temp=queues[level].processes[current_process];
                                for(int j=current_process; j<num_processes; j++){
                                    queues[level].processes[(j)%num_processes]=queues[level].processes[(j+1)%num_processes];
                                        }
                                queues[level].processes[num_processes-1]=temp; // move it to the last element 
                                printf("Process %d Moved to the end of Queue %d \t\t\t Total Time Elapsed: %d\n",queues[level].processes[num_processes-1].id,level+1,time);
                                        // queues[level].processes[current_process].waiting_time++;
                                }
                                else { // it's already set , so check if it completed or not
                                        if(queues[level].processes[current_process].io_completion_time<=time){
                                            // execute noramlly
                                            queues[level].processes[current_process].burst_time-=queues[level].quantum;
                                            queues[level].processes[current_process].time_slice+=queues[level].quantum;
                                            // check if took the full allotment
                                                // If the time slice has been exceeded but not finished(so it's gonna be moved to the next queue)
                                            if (queues[level].processes[current_process].time_slice == queues[level].quantum)
                                            {
                                                // Move the current process to the next queue
                                                queues[level + 1].processes[queues[level + 1].num_processes++] = queues[level].processes[current_process];
                                                printf("Process %d  Moved to Queue %d\n",queues[level].processes[current_process].id,level+2);
                                                // Remove the process from the current queue
                                                for (int i = current_process; i < queues[level].num_processes - 1; i++)
                                                {
                                                    queues[level].processes[i] = queues[level].processes[i + 1];
                                                }
                                                queues[level].num_processes--;
                                                current_process--;
                                            }
                
                      }
                    } 
              
               }
        
        }
            
          
    else
    {
        level++;
        current_process = 0;

        // If all levels have been completed, move back to the first level
        if (level == MAX_LEVELS)
        {
            level = 0;
        }
    }

    // printf("Queue %d Execution Done...\n",level+1);
}
}

  /* else // not completed then move it again to the end of the queue
                {
              
                       queues[level].processes[current_process].time_slice++;
                        time++;
                        if( queues[level].processes[current_process].io_time){ // if the time hits the start of i/o of the process then deschedule it and put it back to the end of the queue
                                //shift it to the end 
                                process_t temp=queues[level].processes[current_process];
                                for(int j=current_process; j<num_processes; j++){
                                queues[level].processes[(j)%num_processes]=queues[level].processes[(j+1)%num_processes];
                                }
                                queues[level].processes[num_processes-1]=temp; // move it to the last element 
                                // queues[level].processes[current_process].waiting_time++;
                        
            }
                } */

 /*  for(int i=0; i<queues[level].quantum ;i++){ //increasing time slice one by one 
                queues[level].processes[current_process].time_slice++;
                time++;
                int ioFlag=0;
                if( queues[level].processes[current_process].io_time){ // if the time hits the start of i/o of the process then deschedule it and put it back to the end of the queue
                        //shift it to the end 
                        process_t temp=queues[level].processes[current_process];
                        for(int j=current_process; j<num_processes; j++){
                         queues[level].processes[(j)%num_processes]=queues[level].processes[(j+1)%num_processes];
                        }
                        queues[level].processes[num_processes-1]=temp; // move it to the last element 
                        ioFlag=1;
                        // queues[level].processes[current_process].waiting_time++;
                }
                if(ioFlag)break;
            } */

       /*      // If the current process has an I/O operation to complete
            if (queues[level].processes[current_process].io_completion_time > 0)
            {
                // Decrement the I/O completion time
                queues[level].processes[current_process].io_completion_time--;

                // If the I/O operation has completed
                if (queues[level].processes[current_process].io_completion_time == 0)
                {
                    // Reset the I/O time of the process
                    queues[level].processes[current_process].io_time = 0;
                }
            } */
            
            
        // If the current queue is empty, move to the next queue
