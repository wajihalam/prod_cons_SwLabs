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

#define BUFFERSIZE 10
char buffer[BUFFERSIZE];

const char initial_seq [5] = "+++M";
const char exit_seq [5] = "EXIT";
const char move_dir_north [1] = "N";
const char move_dir_south [1] = "S";
const char move_dir_east [1] = "E";
const char move_dir_west [1] = "W";

int charArrayToInt(char *arr) {
    int i, value, r, flag;

    flag = 1;
    i = value = 0;

    for( i = 5 ; i<7 ; ++i){
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

void get_user_input(char *input) {
    int length;
    *input = '\0';
    do {
        if (fgets(buffer, BUFFERSIZE, stdin) == NULL) {
            break;
        }
        length = strlen(buffer)-1;
        buffer[length] = '\0';
    } while (length == 0);
}

const char* buffer_test = buffer;

void* producer(void* args) {
    while (1) {
        // Add to the buffer
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        printf("Enter your command\n");
        get_user_input(buffer);
        const char* buffer_test = buffer;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
        sleep(1);
    }
}

void* consumer(void* args) {
    while (1) {
        // Remove from the buffer
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        int steps = charArrayToInt(buffer);
        if ((strncmp(buffer, initial_seq, 4) == 0)
            && ((strncmp((buffer_test+4), move_dir_north, 1) == 0) || (strncmp((buffer_test+4), move_dir_south, 1) == 0) || (strncmp((buffer_test+4), move_dir_east, 1) == 0) || (strncmp((buffer_test+4), move_dir_west, 1) == 0) ))
        {
            if(strncmp((buffer_test+4), move_dir_north, 1) == 0)
            {

                printf("Moved North %d steps\n", steps);
            }
            else if(strncmp((buffer_test+4), move_dir_south, 1) == 0)
            {
                printf("Moved South %d steps\n", steps);
            }
            else if(strncmp((buffer_test+4), move_dir_east, 1) == 0)
            {
                printf("Moved East %d steps\n", steps);
            }
            else if(strncmp((buffer_test+4), move_dir_west, 1) == 0)
            {
                printf("Moved West %d steps\n", steps);
            }
        }
        else if(strncmp((buffer), exit_seq, 4) == 0)
        {
            printf("Exited\n");
            pthread_kill(&producer, SIGINT);
            pthread_kill(&consumer, SIGINT);

            exit(0);
        }
        else
        {
            printf("Invalid Command\n");
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
