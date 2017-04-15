/********************************************************************
 * File_Name: host.c
 * Description: This module will randomly choose a number between
 *      1 to 10 (both included) for the guest module to guess.
 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define NUMBER_LOWER_BOUND  1
#define NUMBER_UPPER_BOUND  10
#define MSG_LEN_LIMIT       30
#define GUESS               1
#define RESPONSE            2

struct my_msgbuf
{
    // type: 1 for a guessing from guest
    //       2 for a response from host
    long type; 
    char text[MSG_LEN_LIMIT];
};

int get_random_number();

int main(int argc, char const *argv[])
{
    int number, guess, success;
    int qid;
    key_t key;
    struct my_msgbuf qbuf;
    char msg[MSG_LEN_LIMIT];

    srand(time(NULL));

    // get a random number.
    number = get_random_number();

    // create unique key via call to ftok()
    key = ftok(".", 'g');

    // open the queue, create if necessary
    if((qid = msgget(key, IPC_CREAT|0660)) == -1) 
    {
        fprintf(stderr, "fail to open or create queue.");
        exit(1);
    }

    printf("I have chosen a random number between %d and %d, ",
        NUMBER_LOWER_BOUND, NUMBER_UPPER_BOUND);
    printf("(both included)\n");
    printf("\nI can tell you here that it is %d.\n", number);

    success = 0;
    // main loop.
    while (1)
    {
        printf("\nWaiting for a guess.\n");

        // read GUESS from message queue.
        printf("(Reading a message ...)\n");
        qbuf.type = GUESS;
        msgrcv(qid, (struct my_msgbuf *)&qbuf, MSG_LEN_LIMIT, GUESS, 0);
        guess = atoi(qbuf.text);
        if (guess == number)
        {
            success = 1;
            strcpy(msg, "You are right.");
        }
        else
        {
            success = 0;
            if (guess > number)
                strcpy(msg, "Too Big.");
            else
                strcpy(msg, "Too Small.");
        }

        printf("[GUEST]: guess = %d\n", guess);
        printf("Response is: %s\n", msg);

        // sending RESPONSE.
        printf("(Sending the message ...)\n");
        qbuf.type = RESPONSE;
        strcpy(qbuf.text, msg);
        if((msgsnd(qid, (struct msgbuf *)&qbuf, strlen(qbuf.text)+1, 0)) ==-1)
        {
            fprintf(stderr, "fail to send massage.");
            exit(1);
        }

        if (success)
            break;
    }
    msgctl(qid, IPC_RMID, 0);

    printf("\nEnd Of Session.\n\n");
    return 0;
}

int get_random_number()
{
    return rand() % (NUMBER_UPPER_BOUND - NUMBER_LOWER_BOUND + 1)
        + NUMBER_LOWER_BOUND;
}