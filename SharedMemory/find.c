/********************************************************************
 * File_Name: find.c
 * Description: This module search a number in a file by 2 processes.
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <unistd.h>  

#define NUMBER_SIZE    65536
#define PROCESS_SYNC   (NUMBER_SIZE + 1)
#define C_PROCESS_DATA (NUMBER_SIZE + 2)
#define P_PROCESS_DATA (NUMBER_SIZE + 3)

int main(int argc, char const *argv[])
{
    key_t key;
    int   shmid;
    int   target;
    int   i;
    int   count;
    int  *mem;

    // create unique key via call to ftok()
    key = ftok(".", 'z');

    if((shmid = shmget(key, (NUMBER_SIZE + 3) * sizeof(int), 
        IPC_CREAT | 0666)) == -1) 
    {
        fprintf(stderr, "shm failure.");
        exit(1);
    }

    FILE *f;
    f = fopen(argv[1], "r");
    target = atoi(argv[2]);
    mem = (int *)shmat(shmid, 0, 0);
    mem[PROCESS_SYNC] = 0;

    i = 0;
    while (fscanf(f, "%d", mem + i++) == 1)
        ;

    count = i;

    // find target by 2 process
    pid_t childpid;
    if((childpid = fork()) < 0) // error occured
    {
        fprintf(stderr, "fork failure.");
        exit(1);
    }
    else if(childpid == 0) // child process
    {
        mem[C_PROCESS_DATA] = -1;
        for (int i = 0; i < count / 2; ++i)
        {
            if (target == mem[i])
            {
                mem[C_PROCESS_DATA] = i;
                break;
            }
        }

        mem[PROCESS_SYNC] += 1;

    }
    else // parent process
    {
        mem[P_PROCESS_DATA] = -1;
        for (int i = count / 2; i < count; ++i)
        {
            if (target == mem[i])
            {
                mem[P_PROCESS_DATA] = i;
                break;
            }
        }

        mem[PROCESS_SYNC] += 1;
    }

    while (mem[PROCESS_SYNC] < 2)
        ; // wait for snyc.

    if (childpid > 0)
    {
        printf("Child Process:\n");
        if (mem[C_PROCESS_DATA] == -1)
            printf("\tNot Found.\n");
        else
            printf("\tFound at No. %d\n", mem[C_PROCESS_DATA]);


        printf("Parent Process:\n");
        if (mem[P_PROCESS_DATA] == -1)
            printf("\tNot Found.\n");
        else
            printf("\tFound at No. %d\n", mem[P_PROCESS_DATA]);
    }

    shmctl(shmid, IPC_RMID, 0);

    return 0;
}