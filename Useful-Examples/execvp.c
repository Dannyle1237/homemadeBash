#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( )
{
  pid_t pid = fork( );
  if( pid == 0 )
  {

    char *arguments[4];

    arguments[0] = ( char * ) malloc( strlen( "ls" ) );
    strncpy( arguments[0], "ls", strlen( "ls" ) );
    
    // Notice you can add as many NULLs on the end as you want
    int ret = execvp( arguments[0], &arguments[0] );  
    if( ret == -1 )
    {
      perror("execl failed: ");
    }
  }
  else 
  {
    int status;
    wait( & status );
  }

  return 0;
}
