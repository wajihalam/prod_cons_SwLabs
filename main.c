#include <stdio.h>
#include <stdint.h>
#include <string.h>

//DEFINES
#define BUFFERSIZE 9                 //Size of buffer
char buffer[BUFFERSIZE];             //Define buffer of a specific size

//Word sequence to move into different direction
const char word_seq [5] = "+++M";
const char exit_seq [5] = "+++EXIT";
const char move_dir_n [1] = "N";
const char move_dir_s [1] = "S";
const char move_dir_e [1] = "E";
const char move_dir_w [1] = "W";

//Function to convert array to integer
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

//Function to get user input from command line. The navigation direction.
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

void main()
{
    printf("You can give following commands.\n"
           "\t1. Input \"+++MNnn\" to move North of nn steps\n"
           "\t2. Input \"+++MSnn\" to move South of nn steps\n"
           "\t3. Input \"+++MWnn\" to move West of nn steps\n"
           "\t4. Input \"+++MNnn\" to move North of nn steps\n"
           "\t5. Input \"+++EXIT\" to EXIT the program\n"
           "where nn is a number in between 0 to 99\n\n"
           "Now, please give your command \n");

    get_user_input(&buffer); //Getting user input

    const char* buffer_test = buffer;
    int steps = charArrayToInt(buffer); //Calculating steps from nn

    //Comparing with input sequence and generating output accordingly.
    if ((strncmp(buffer, word_seq, 4) == 0)
    && ((strncmp((buffer_test+4), move_dir_n, 1) == 0) || (strncmp((buffer_test+4), move_dir_s, 1) == 0) || (strncmp((buffer_test+4), move_dir_e, 1) == 0) || (strncmp((buffer_test+4), move_dir_w, 1) == 0) ))
    {
        if(strncmp((buffer_test+4), move_dir_n, 1) == 0)
        {
            printf("Moved North %d steps\n", steps);
        }
        else if(strncmp((buffer_test+4), move_dir_s, 1) == 0)
        {
            printf("Moved South %d steps\n", steps);
        }
        else if(strncmp((buffer_test+4), move_dir_e, 1) == 0)
        {
            printf("Moved East %d steps\n", steps);
        }
        else if(strncmp((buffer_test+4), move_dir_w, 1) == 0)
        {
            printf("Moved West %d steps\n", steps);
        }
    }
    else if(strncmp((buffer), exit_seq, 4) == 0)
    {
        printf("Program Terminated");
    }
    else
    {
        printf("Invalid Command");
    }
}



