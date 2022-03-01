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
pid_t execvpFunc(char *tokens[])
{
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
  return pid;
}

//creating a linked list to keep track of our history of commands
struct historyNode {
  char command[MAX_COMMAND_SIZE];
  struct historyNode *next;
};

//Function implement for us to easily run a command when the user inputs !n. 
int rerunCommand(struct historyNode *head, struct historyNode *current, char* command_string, int count, pid_t pidhistory[15], int pid_index)
{
  int gettingCommand = 0;
  while(gettingCommand == 0){
    //store input in history and move current command pointer up to keep command history updated

    strcpy(current->command, command_string);
    current->next = NULL;
    current->next = (struct historyNode*) malloc(sizeof(struct historyNode));
    current = current->next;


    //Once we reach out 15 command limit, we must only remove the first command and add on the new one. 
    if(count == 15){
      struct historyNode* temp = head;
      head = head->next;
      //free(temp);
    }
    else{
      count++;
    }

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
    //check for an empty string before everything to avoid segfaulting
    if(token[0] == '\0'){
      continue;
    }

    if(strcasecmp(token[0],"quit") == 0){
      exit(0);
    }
    else if(strcmp(token[0], "cd") == 0){
      chdir(token[1]);
    }
    //We iterate through our linked list and print out the history of the commands  
    else if(strcmp(token[0], "history") == 0){
      struct historyNode *temp = head;
      printf("\n");
      for(int i = 0; temp->next != NULL; i++){
        printf("%d:%s",i, temp->command);
        temp = temp->next;
      }
    }
    else if(*token[0] == '!'){
      struct historyNode *temp = head;
      //We use memmove to delete the ! in our and only have the int left over 
      memmove(token[0], token[0] + 1, strlen(token[0]));
      if(atoi(token[0]) > count){
      }
      else{
        for(int i = 0; i < atoi(token[0]); i++){
          temp = temp->next;
        }
        count = rerunCommand(head, current, temp->command, count, pidhistory, pid_index);
      }
    }
    else if(strcmp(token[0], "pidhistory") == 0){
      int index = pid_index;
      for(int i = 0; i < 15; i++){
        printf("%d\n", pidhistory[index++]);
        if(index > 14){
          index = 0;
        }
      }
    }
    else{
      pid_t temp = execvpFunc(token);
      if(temp != -1){
        pidhistory[pid_index++] = temp;
      }
      else{
        printf("\n%s: Command is not found", token[0]);
        return 0;
      }
    }
    free( head_ptr );
    gettingCommand = 1;
    return count;
  }
  return count;
}

int main()
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  //Creating a linked list to store our history of commands in. 
  struct historyNode *head = NULL;
  struct historyNode *current;
  //head = (struct historyNode*) malloc(sizeof(struct historyNode));
  //struct historyNode *current = head;

  //using count to keep track of which command number we are on, but we leave 15 as the max. 
  int count = 0;

  int pid_index = 0;
  pid_t pidhistory[15];
  for(int i = 0; i < 15; i++){
    pidhistory[i] = -1;
  }

  while( 1 )
  {
    // Print out the msh prompt
    printf ("\nmsh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    //store input in history and move current command pointer up to keep command history updated
    

    if( head == NULL )
    {
      head = (struct historyNode*) malloc(sizeof(struct historyNode));
      head -> next = NULL;
      current = head;
    }
    else
    {
      current->next = (struct historyNode*) malloc(sizeof(struct historyNode));
      current = current->next;
      current -> next = NULL;
    }
    
    if( current )
    {  
      strncpy(current->command, command_string, 255);
    }
    else
    {
      perror("Problem allocating linked list node");
    }


    //Once we reach out 15 command limit, we must only remove the first command and add on the new one. 
    if(count == 15){
      struct historyNode* temp = head;
      head = head->next;
      free(temp); 
    }
    else{
      count++;
    } 

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

    if(token[0] == '\0'){
      continue;
    }

    if(strcasecmp(token[0],"quit") == 0 || strcasecmp(token[0],"exit") == 0){
      exit(0);
    }
    else if(strcmp(token[0], "cd") == 0){
      chdir(token[1]);
    }
    //We iterate through our linked list (head and current) in order to find out history of commands
    else if(strcmp(token[0], "history") == 0){
      struct historyNode *temp = head;
      printf("\n");
      int i = 0;
      while( temp )
      {
        printf("%d:%s",i++, temp->command);
        temp = temp->next;
      }
    }
    else if(*token[0] == '!'){
      struct historyNode *temp = head;
      memmove(token[0], token[0] + 1, strlen(token[0]));
      if(atoi(token[0]) > count){
        printf("Command not found in history");
      }
      else{
        for(int i = 0; i < atoi(token[0]); i++){
          temp = temp->next;
        }
      }
      //We must pass our pointers into the rerun command in order to ensure our history is still getting updated
      count = rerunCommand(head, current, temp->command, count, pidhistory, pid_index);
    }
    else if(strcmp(token[0], "pidhistory") == 0){
      int index = pid_index;
      for(int i = 0; i < 15; i++){
        if(pidhistory[i] != -1){
          printf("%d:%d\n", i, pidhistory[i]);
        }
      }
    }
    else{
      pid_t temp = execvpFunc(token);
      if(temp != -1){
        if(pid_index > 14){
          for(int i = 0; i < 14 - 1; i++){
            pidhistory[i] = pidhistory[i + 1];
          }
          pid_index--;
        }
        pidhistory[pid_index++] = temp;
      }
      else{
        printf("\n%s: Command is not found", token[0]);
        return 0;
      }
    }
    free( head_ptr );
  }
  return 0;
  // e2520ca2-76f3-11ec-90d6-0242ac120003
}

