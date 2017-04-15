/********************************************************************
 * File_Name: guest.c
 * Description: This module will take input to guess the number.
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

int main(int argc, char const *argv[])
{
    int number;
    int qid;
    key_t key;
    struct my_msgbuf qbuf;

    srand(time(NULL));

    // create unique key via call to ftok()
    key = ftok(".", 'g');

    // open the queue, create if necessary
    if((qid = msgget(key, IPC_CREAT|0660)) == -1) 
    {
        fprintf(stderr, "fail to open or create queue.");
        exit(1);
    }

    printf("The host have chosen a random number between %d and %d, ",
        NUMBER_LOWER_BOUND, NUMBER_UPPER_BOUND);
    printf("(both included)\n");

    // main loop.
    while (1)
    {
        // reading a number from stdin.
        printf("\nInput number to guess it:\t");
        scanf("%d", &number);
        if (number < NUMBER_LOWER_BOUND || number > NUMBER_UPPER_BOUND)
        {
            printf("The range of the number is from %d to %d, ",
                NUMBER_LOWER_BOUND, NUMBER_UPPER_BOUND);
            printf("(both included)\n");
            continue;
        }

        // sending GUESS.
        printf("(Sending the message ...)\n");
        qbuf.type = GUESS;
        sprintf(qbuf.text, "%d", number);
        if((msgsnd(qid, (struct msgbuf *)&qbuf, strlen(qbuf.text)+1, 0)) ==-1)
        {
            fprintf(stderr, "fail to send massage.");
            exit(1);
        }

        // read RESPONSE from message queue.
        printf("(Reading a message ...)\n");
        qbuf.type = RESPONSE;
        msgrcv(qid, (struct my_msgbuf *)&qbuf, MSG_LEN_LIMIT, RESPONSE, 0);
        printf("[HOST]: %s\n", qbuf.text);
        if (strcmp(qbuf.text, "You are right.") == 0)
            break;
    }

    printf("\nEnd Of Session.\n\n");
    return 0;
}