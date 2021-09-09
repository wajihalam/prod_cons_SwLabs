#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdint.h>

#define THREAD_NUM 2

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

#define BUFFERSIZE 9                        //Size of buffer
char buffer[BUFFERSIZE];                    //Define buffer of a specific size
int consumer_wake_time;
const char* buffer_test = buffer;

/*** Word sequence to move into different direction ***/
const char initial_seq [5] = "+++M";
const char exit_seq [5] = "+++EXIT";
const char move_dir_north [1] = "N";
const char move_dir_south [1] = "S";
const char move_dir_east [1] = "E";
const char move_dir_west [1] = "W";

/*** Function to convert array to integer ***/
int charArrayToInt(char *arr, int start, int end) {
    int i, value, r, flag;

    flag = 1;
    i = value = 0;

    for( i = start ; i<end ; ++i){
        // if arr contain negative number
        if( i==0 && arr[i]=='-' ){
            flag = -1;
            continue;
        }

        r = arr[i] - '0';
        value = value * 10 + r;
    }

    value = value * flag;

    return value;

}

/*** Function to get user input from command line. The navigation direction. ***/
void get_user_input(char *input) {
    int length;
    *input = '\0';

    while(buffer[(sizeof(buffer)  - 2)] != '\n')
    {
        printf("Enter your command\n");


        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }
        if (buffer[(sizeof(buffer)  - 2)] != '\n')
        {
           printf("Invalid command: Input the command again\n");
        }
    }
    length = strlen(buffer)-1;
    buffer[length] = '\0';
}

void* producer(void* args) {
    while (1) {
        // Add to the buffer
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);

        get_user_input(buffer);
        const char* buffer_test = buffer;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
        sleep(consumer_wake_time);
    }
}

void* consumer(void* args) {
    while (1) {
        sleep(consumer_wake_time);
        // Remove from the buffer
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        //int steps = charArrayToInt(buffer);
        //Comparing with input sequence and generating output accordingly.
        if ((strncmp(buffer, initial_seq, 4) == 0)
            && ((strncmp((buffer_test+4), move_dir_north, 1) == 0) || (strncmp((buffer_test+4), move_dir_south, 1) == 0) || (strncmp((buffer_test+4), move_dir_east, 1) == 0) || (strncmp((buffer_test+4), move_dir_west, 1) == 0) )
                )
        {
            int buffer5 = charArrayToInt(buffer, 5, 6);
            int buffer6 = charArrayToInt(buffer, 6, 7);
            if((buffer5)>=0 && (buffer5)<=9)
            {
                if((buffer6)>=0 && (buffer6)<=9)
                {
                    int steps = charArrayToInt(buffer, 5, 7);      //Calculating steps from nn
                    if(strncmp((buffer_test+4), move_dir_north, 1) == 0)
                    {

                        printf("Moved North %d steps\n\n", steps);
                    }
                    else if(strncmp((buffer_test+4), move_dir_south, 1) == 0)
                    {
                        printf("Moved South %d steps\n\n", steps);
                    }
                    else if(strncmp((buffer_test+4), move_dir_east, 1) == 0)
                    {
                        printf("Moved East %d steps\n\n", steps);
                    }
                    else if(strncmp((buffer_test+4), move_dir_west, 1) == 0)
                    {
                        printf("Moved West %d steps\n\n", steps);
                    }
                }
                else
                {
                    printf("Wrong input commands, enter again\n\n");
                }
            }
            else
            {
                printf("Wrong input command, enter again\n\n");
            }
        }
        else if(strncmp((buffer), exit_seq, 4) == 0)
        {
            printf("Exited\n\n");
            pthread_kill(&producer, SIGINT);
            pthread_kill(&consumer, SIGINT);

            exit(0);
        }
        else
        {
            printf("Invalid Command, Enter again\n");
        }
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);
    }
}

int main(int argc, char* argv[]) {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);

    printf("Enter the time interval in seconds after which consumer will wake up: \n");
    //scanf("%d", &consumer_wake_time);
    //fflush(stdin);

    char *p, s[100];

    while (fgets(s, sizeof(s), stdin)) {
        consumer_wake_time = strtol(s, &p, 10);
        if (p == s || *p != '\n') {
            printf("You entered wrong input. Please enter an integer value for consumer wake up time: \n");
        } else break;
    }
    printf("The consumer will wake up in: %ld seconds\n\n", consumer_wake_time);
    fflush(stdin);

    printf("You can give following commands.\n"
           "\t1. Input \"+++MNnn\" to move North of nn steps\n"
           "\t2. Input \"+++MSnn\" to move South of nn steps\n"
           "\t3. Input \"+++MWnn\" to move West of nn steps\n"
           "\t4. Input \"+++MNnn\" to move North of nn steps\n"
           "\t5. Input \"+++EXIT\" to EXIT the program\n"
           "where nn is a number in between 0 to 99\n\n");

    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (i %2 == 0) {
            if (pthread_create(&th[i], NULL, &producer, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}
