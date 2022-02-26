// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/*
Name: Danny Le
ID: 1001794802
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                              // so we need to define what delimits our tokens.
                              // In this case  white space
                              // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments

//This function allows us to call the execvp command, which allows us to quickly check all commands.
int execvpFunc(char *tokens[]){
  pid_t pid = fork( );
  if( pid == 0 )
  {
    char *arguments[10];
    int i;
    //Code bugged whenever the for loop runs for i<10, so we only run until the command line has a NULL
    for(i = 0; tokens[i] != NULL; i++){
      arguments[i] = ( char * ) malloc( strlen( tokens[i] ) );
      strncpy( arguments[i], tokens[i], strlen( tokens[i] ) );
    }
    arguments[i] = NULL;

    int ret = execvp( arguments[0], &arguments[0] );  
    if( ret == -1 )
    {
      return -1;
    }
  }
  else
  {
    int status;
    wait( & status );
  }
  return 0;
}

int main()
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  int commandNum = 0; 
  char *history[15];

  for(int i = 0; i < 15; i++){
    history[i] = (char*) malloc( MAX_COMMAND_SIZE );
  }
  while( 1 )
  {
    //Use the commandNum to keep track of the order of commands and store it into the string array history


    // Print out the msh prompt
    printf ("\nmsh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                            
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                            
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }


    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    /*int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }
    */
    strcpy(history[commandNum], command_string);

    if(token[0] == '\0'){
      continue;
    }

    if(strcasecmp(token[0],"quit") == 0){
      exit(0);
    }
    else if(strcmp(token[0], "cd") == 0){
      chdir(token[1]);
    }
    else if(strcmp(token[0], "history") == 0){
      for(int i = 0; strcmp(history[i], "") != 0; i++){
        printf("%d:%s",i, history[i]);
      }
    }
    else if(execvpFunc(token) == 0){
    }
    else{
      printf("\n%s: Command is not found", token[0]);
      return 0;
    }
    free( head_ptr );
    /*printf("\n0%s", history[0]);
    printf("\n1%s", history[1]);
    printf("\n2%s", history[2]);
    printf("\n3%s", history[3]);
    printf("\n4%s", history[4]);
    printf("\n%d", commandNum);*/
    commandNum ++;
  }
  return 0;
  // e2520ca2-76f3-11ec-90d6-0242ac120003
}

