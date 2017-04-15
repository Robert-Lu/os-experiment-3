#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>  

#define MAX_SIZE 65536


union semun 
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

int binary_semaphore_allocation(key_t key, int sem_flags);
int binary_semaphore_deallocate(int semid);
int binary_semaphore_initialize(int semid, int value);
int binary_semaphore_wait(int semid);
int binary_semaphore_post(int semid);

int main(int argc, char const *argv[])
{
    key_t key_child;
    key_t key_parent;
    int   sem_child;
    int   sem_parent;

    // create unique key via call to ftok()
    key_child = ftok(".", 'c');
    key_parent = ftok(".", 'p');

    if ((sem_child = binary_semaphore_allocation(
            key_child, IPC_CREAT | 0666)) == -1 ||
        (sem_parent = binary_semaphore_allocation(
            key_parent, IPC_CREAT | 0666)) == -1) 
    {
        fprintf(stderr, "sem failure.");
        exit(1);
    }

    binary_semaphore_initialize(sem_child, 1);
    binary_semaphore_initialize(sem_parent, 0);

    pid_t childpid;
    if((childpid = fork()) < 0) // error occured
    {
      perror("Fork Failed");
      exit(1);
    }
    else if(childpid == 0) // child process
    {
        // do first
        binary_semaphore_wait(sem_child);

        FILE *f;
        f = fopen(argv[1], "r");
        int num;

        for (int i = 0; i < 10; ++i)
        {
            if (fscanf(f, "%d", &num) != 1)
                break;
            printf("%d\n", num);

            binary_semaphore_post(sem_parent);
            binary_semaphore_wait(sem_child);
        }

        fclose(f);
    }
    else // parent process
    {
        // after first element has been output
        binary_semaphore_wait(sem_parent);

        FILE *f;
        f = fopen(argv[2], "r");
        int num;

        for (int i = 10; i < 20; ++i)
        {

            if (fscanf(f, "%d", &num) != 1)
                break;
            printf("%d\n", num);
            binary_semaphore_post(sem_child);
            binary_semaphore_wait(sem_parent);
        }

        fclose(f);
    }

    binary_semaphore_deallocate(sem_child);
    binary_semaphore_deallocate(sem_parent);

    return 0;
}

// Obtain a binary semaphore’s ID, allocating if necessary.
int binary_semaphore_allocation (key_t key, int sem_flags)
{
    return semget (key, 1, sem_flags);
}

// Deallocate a binary semaphore. All users must have finished their
//     use. Returns -1 on failure. 
int binary_semaphore_deallocate (int semid)
{
    union semun ignored_argument;
    return semctl (semid, 1, IPC_RMID, ignored_argument);
}

// Initialize a binary semaphore with a value.
int binary_semaphore_initialize (int semid, int value)
{
    union semun argument;
    unsigned short values[1];
    values[0] = value;
    argument.array = values;
    return semctl (semid, 0, SETALL, argument);
}

// Wait on a binary semaphore. Block until the semaphore value is positive, 
//     then decrement it by 1.
int binary_semaphore_wait (int semid)
{
    struct sembuf operations[1];
    // Use the first (and only) semaphore.
    operations[0].sem_num = 0;
    // Decrement by 1.
    operations[0].sem_op = -1;
    // Permit undo’ing.
    operations[0].sem_flg = SEM_UNDO;
    return semop (semid, operations, 1);
}

// Post to a binary semaphore: increment its value by 1.
//     This returns immediately.
int binary_semaphore_post (int semid)
{
    struct sembuf operations[1];
    // Use the first (and only) semaphore.
    operations[0].sem_num = 0;
    // Increment by 1.
    operations[0].sem_op = 1;
    // Permit undo’ing.
    operations[0].sem_flg = SEM_UNDO;
    return semop (semid, operations, 1);
}
