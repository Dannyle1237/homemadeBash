#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

int main(){
    char *test = (char*) malloc(40);
    while(1){
        fgets(test, 40, stdin);
        printf("fsdfs");
    }

}