#include "structFlag.h"
#include "1stLevel.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>


/*
 *
 * Version:
 *     $Id: 1stLevel.c,v 1.4 2005/07/18 23:27:28 edn2065 Exp $
 *
 * Revision:
 *     $Log: 1stLevel.c,v $
 *     Revision 1.4  2005/07/18 23:27:28  edn2065
 *     Integrated RxTxThread and fixed major deadlocks
 *
 *     Revision 1.3  2005/06/27 18:49:55  edn2065
 *     added auto bps checking to amishs code
 *
 *     Revision 1.2  2005/06/01 16:25:44  edn2065
 *     Worked out compilation issues
 *
 *     Revision 1.1  2004/12/22 15:33:26  jar4773
 *     Initial revision
 *
 *
 * Author: Amish Rughoonundon
 * Company: Active vision lab
 *
 */

/*Prototypes*/
void PrintCmdReply(command *aC);

/*
 * ReadFromCam: Read a command from the serial port. 
 * 
 * Return: A reply from the camera.
 *         Negative number if there is an error.
 *
 * Param: fd:- The file descriptor for the serial port.
 *       aCommand:- The command structure to read in.
 *       time:- The time to wait until giving up reading something.
 * 
 */

int ReadFromCam(int fd, command *aCommand, int time) {
  
  int numOfBytesRead = 0; /*Number of bytes read. */
  int change = 0; /* Set if there is a change in Rxd line. */ 
  char correct = 0; /* Set if 1 byte was correctly read. */
  unsigned char *buffer = NULL; /* Pointer to the start of the buffer. */
  struct timeval timeout; /* Time out waiting for signal. */
  fd_set readFds;

  /* Set program to monitor Rxd line for any change. */
  FD_ZERO(&readFds);
  
  FD_SET(fd, &readFds);

  timeout.tv_sec = time; /* Setup time to wait in seconds. */

  timeout.tv_usec = 0;
 
  change = select(fd+1, &readFds, NULL, NULL, &timeout);
 
  //printf( "Rx: " );
  if(change > 0) {

    do {
     
      /* Set the pointer to the correct part of the buffer. */
      buffer = aCommand->reply+(sizeof(unsigned char)*numOfBytesRead);

      /* Read 1 byte into the buffer from the serial port. */
      correct = read(fd, buffer, 1);
 
      if(correct != -1) {
     
	numOfBytesRead++; 
       // printf( "%x ", aCommand->reply[numOfBytesRead-1] );
     
      }
    } while ((numOfBytesRead < 20) && 
	     (aCommand->reply[numOfBytesRead-1] != aCommand->end));
  }
 
  //printf( "\n" );
  aCommand->replyLength = numOfBytesRead;
  
  if(numOfBytesRead >= 20 || numOfBytesRead == 0) {
  
    fputs("2) Reply message does not comform to standard.(ReadFromCam)\n",logfile);
   
    PrintCmdReply(aCommand);
    fflush(logfile);

    return -2;
  }
  
  else {
    
    return numOfBytesRead;
  }
}

/*
 *
 * SendToCam: Send a command to the serial port. 
 *
 * Return: Number of bytes sent
 *         Negative number if there is an error.
 * 
 * Param fd: The file descriptor for the serial port.
 *       aCommand: The command structure to read in.
 *
 */
int SendToCam(int fd, command *aCommand) {

  int numOfBytesWritten = 0; /*Number of bytes written. */
  int length = 0; /* Length of the buffer to write. */
  int correct = 0; /* Set if at least some bytes were correctly sent. */
  unsigned char *buffer = NULL; /* Pointer to the start of the buffer. */
  int ctr;
  
  do {

    /* Sets the length of bytes to send to the length of the instruction. */   
    length = (aCommand->sendLength) - numOfBytesWritten;

    /* Sets the pointer to the the point where the last byte was written. */
    buffer = aCommand->send+(sizeof(unsigned char)*numOfBytesWritten);

    /* Write the buffer to the serial port. */

    correct = write(fd, buffer, length);
    
    /* Get the number of bytes written. */

    if( correct >= 0 ) {
      
      numOfBytesWritten = numOfBytesWritten + correct;
      /*for( ctr = 0; ctr < correct; ctr++ )
      {
        printf( "%x ", buffer[ctr] );
      }*/
      
    }
  } while( (numOfBytesWritten < length) );

  //printf( "\n" );
  return numOfBytesWritten;
 
}

/*
 * PrinCmdReply: Prints the reply part of a command
 *
 * Params: ac:- A pointer to the command.
 *
 */
void PrintCmdReply(command *aC) {
  int i;
 
  for (i=0;i<aC->replyLength;i++) {
    
    fprintf(logfile,"%x ",aC->reply[i]);
  }

  fprintf(logfile,"\n"); 
}
