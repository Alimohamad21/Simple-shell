#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int count_args(char *sentence);
char** generate_args_array(char *sentence,int args_count);
void announce_termination();
int is_waiting=0;
int main() {
    char input[200];
    char input_copy[200];
    char **args;
    int args_count;
    int last_argument_index;
    int containsAmpersand;
while(1){
    if(!is_waiting)
    {
    printf("Shell >>  ");
    gets(input);
    strcpy(input_copy, input); // strtok modifies original input so we copy the original input into another string for further processing on the input
    args_count = count_args(input); // count number of input words
    args=generate_args_array(input_copy,args_count); // generate array of strings with size args_count
    if(!strcmp(args[0],"exit"))  // process exit function
    {
    exit(0);
    }
    
    pid_t pid;
    signal(SIGCHLD,announce_termination); // for future printing to file when child is terminated
    last_argument_index=args_count-1;
    containsAmpersand=strcmp(args[last_argument_index],"&")==0?1:0;  // check for &
    if(containsAmpersand)                 // if there is an &, we shouldn't wait for child to terminate, else we should
        {is_waiting=0;
        args[last_argument_index]=NULL;   // to not send & to execvp, (it sends & to the url if we do so)
        }
    else is_waiting=1;
    args[last_argument_index+1]=NULL;   // excevp works alternatingly, so had to implement this fix (doesn't make any sense)
    pid=fork();
    if(pid==-1)   // code for child forking failure
    {   
      continue;
    }
    else if(pid==0)   // if child, execute the command 
    {   
        execvp(args[0],args);
        printf("%s is an invalid command or wrong parameter format",args[0]);
        is_waiting=0;
    }
    else if(pid>0){  // if parent, if user didn't enter & we should wait for child
        if(is_waiting){
        is_waiting=0;
        waitpid(pid, NULL, 0);
        }
    }
}
}
}

int count_args(char *sentence) {    // returns number of words in a sentece, we use it to count args
    int count = 0;
    char *token = strtok(sentence, " ");
    while (token) {
        count++;
        token = strtok(NULL, " ");
    }
    return count;
}
char** generate_args_array(char *sentence,int args_count){   // generates an array of strings used as args
    char ** args = malloc(args_count * sizeof(char*));
    for (int i =0 ; i < args_count; ++i)
        args[i] = malloc(20 * sizeof(char));
    char *token = strtok(sentence, " ");
    int i = 0;
    while (token) {
   
        strcpy(args[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    return args;
}
void announce_termination(){     // write to file that child process has been terminated when it terminates
FILE *fp=fopen("terminations.text","a");
if(!fp){
printf("Error opening file");
exit(0);
}
fprintf(fp,"Process has been terminated\n");
fclose(fp);
}
