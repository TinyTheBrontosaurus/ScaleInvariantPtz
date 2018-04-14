#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "2ndLevel.h"
#include "1stLevel.h"
#include "structFlag.h"

/*
 *
 * Version:
 *     $Id: 2ndLevel.c,v 1.4 2005/08/10 02:48:03 edn2065 Exp $
 *
 * Revision:
 *     $Log: 2ndLevel.c,v $
 *     Revision 1.4  2005/08/10 02:48:03  edn2065
 *     Changed to allow warning and error free ICC compile
 *
 *     Revision 1.3  2005/06/30 14:46:28  edn2065
 *     overnight checkin
 *
 *     Revision 1.2  2005/06/27 18:49:55  edn2065
 *     added auto bps checking to amishs code
 *
 *     Revision 1.1  2005/06/01 13:39:01  edn2065
 *     Adding Ameesh s code
 *
 *     Revision 1.1  2004/12/22 15:33:37  jar4773
 *     Initial revision
 *
 *
 * Author: Amish Rughoonundon
 * Company: Active vision lab
 *
 */

/* Function prototypes not accessible to outside functions. */


//Global variables.
command canonError;

struct termios oldSetting;

speed_t baudrate;

int Initialize38400(char *serial, flag *aflag, int cameranum) {

  baudrate = B38400;
  return Initialize2(serial, aflag, cameranum);


}

/*
 *
 * Initializes the serial port, checks if either a SONY EVI-D100
 * or a CANON VC-C4 camera is connected, sets the sony_canon 
 * flag appropriately and initialises the camera.
 *
 * PARAMS: serial:- A pointer to the serial port name.
 *         aflag:- The flag structure to be used by the command level.
 *         cameranum:- Number of cameras
 *
 * Returns the file descriptor for the serial port
 * if successful else it returns a negative number.
 *
 */
int Initialize(char *serial, flag *aflag, int cameranum){
  baudrate = B9600;
  return Initialize2(serial, aflag, cameranum);

}
 
int Initialize2(char *serial, flag *aflag, int cameranum) {
  
  
  
  int error = 0;
  
  int fileDesc = 0; /* The file descriptor for the serial port. */

  command ir; //Turn off ir receive for sony camera

  /* Open log file and clear*/
  logfile = fopen("logfile.txt","w+");
  
  /* Flag is set for sony and reset for canon. */
  aflag->sony_canon = -1;

  /* If port is already open close it before assigning a file descriptor.*/
 if(fileDesc > 0) {

    close(fileDesc);
  }
  
 fileDesc = SetupConnection(serial); /* Set up the serial port. */

 //DEBUG PURPOSES  
    if(DEBUG) {
      printf("SETUPCONNECTION DONE\n");
    }

  ioctl(fileDesc,TCFLSH,2); /* Flush the input and output buffer */

 /* Check if there is a SONY or CANON camera connected. */
 if( InitializeSony(fileDesc, cameranum) == 0 ) {
   
   /* Set the flag. */
   aflag->sony_canon = 1;
   aflag->camera = cameranum;

   /* Clear the interface. */
   if((error = ClearSonyInterface(fileDesc)) < 0) return error;

   ir.send[0]= 0x80 | cameranum;
   ir.send[1]= 0x01;
   ir.send[2]= 0x06;
   ir.send[3]= 0x08;
   ir.send[4]= 0x03;
   ir.send[5]= 0xFF;
   ir.sendLength = 6;
   ir.end = 0xFF;

   SendToCam(fileDesc, &ir);

   //DEBUG PURPOSES  
   if(DEBUG) {
     printf("SONY ON\n");
   }
 }
 
 else if( InitializeCanon(fileDesc) == 0 ) {

   /* Set the flag. */
   aflag->sony_canon = 0;
   aflag->camera = cameranum;
   
   //DEBUG PURPOSES  
   if(DEBUG) {
     printf("CANON ON\n");
   }
 }

 else {
   
   fputs("3) Camera cannot be initialized or no camera is connected to serial port.(Initialize)\n",logfile);
   fflush(logfile);
   return -3;
 }
  return fileDesc;
}

/*
 *
 * This function sets up the serial serial port communication.
 * Baud rate: B9600 
 * Data size 8bit, non-parity, 1 stop bit with Hardware flow control.
 *
 * PARAMS: serial:- A pointer to the serial port name.
 *
 * Returns the file descriptor if no errors was encountered.
 *
 */
int SetupConnection(char *serial) {
  
  int fd = 0; /* The file descriptor. */
  
  struct termios termData; /* Setting serial port properties. */
  
  /* Open serial port and sets file descriptor to point to serial port. */
  if((fd = open(serial, O_RDWR | O_NOCTTY | O_NDELAY)) < 0 ) {

    fprintf(logfile,"3) File descriptor for serial port could not be opened. Error: %s\n",strerror(errno));
    fflush(logfile);
    return -3;
  }
  
  else fcntl(fd,F_SETFL,0);
  
  /* Check if the file descriptor points to a terminal device or not. */
  if(isatty(fd)==0) {

    fputs("4) File descriptor does not point to a terminal.(SetupConnection)\n",logfile);
    fflush(logfile);
    return -4;
  }
  
  /* Get the current attributes of the serial port. */
  if(tcgetattr(fd, &termData) < 0) {
    
    fprintf(logfile, "5) tcgetattr error. Error: %s.(SetupConnection)\n",strerror(errno));
    fflush(logfile);
    return -5;
  }

  //Keep the old serial port setting
  oldSetting = termData;

  /////////////////////////SONY////////////////////////////
  /* Serial port transmits raw data. */
      cfmakeraw(&termData);
   termData.c_cc[VMIN] = 0;  /* No minimum character read. */
   termData.c_cc[VTIME] = 1; /* Dont wait  msec for answer */
   
   //////////////////////CANON////////////////////////////
   /* Enable hardware flow control. */
   termData.c_cflag |= CRTSCTS;

   /* Set the input and output transmission speed. */
  if(cfsetispeed(&termData, baudrate) < 0) {

    fprintf(logfile, "6) tcsetispeed error. Error: %s.(SetupConnection)\n",strerror(errno));
    fflush(logfile);
    return -6;
  }

  if(cfsetospeed(&termData, baudrate) < 0) {
    
    fprintf(logfile, "7) tcsetospeed error. Error: %s.(SetupConnection)\n",strerror(errno));
    fflush(logfile);
    return -7;
  }

  /* Sets the new attribute of the serial port */
  if(tcsetattr(fd, TCSANOW, &termData) < 0 ) {
    
    fprintf(logfile, "8) tcsetattr error. Error: %s.(SetupConnection)\n",strerror(errno));
    fflush(logfile);
    return -8;
  }

  return fd;
}

/*
 *
 * Initialises the SONY camera.
 *
 * PARAMS: fd:- A file descriptor for the serial port.
 *         retries:- The number of time to try sending query. 
 *
 * Returns -8 if it fails else it returns 0.
 *
 */
int InitializeSony(int fd, int cameranum) {
 
  ioctl(fd,TCFLSH,0); /* Flush the input buffer */

  int error = 0;
 
  /* Set up the assign Address command. */
  
  command assign; /* Get the number of the camera. */
  command ret; /* Check against the answer of the camera. */
  
  assign.send[0]=0x88; 
  assign.send[1]=0x30;
  assign.send[2]=0x01; 
  assign.send[3]=0xff;
  assign.sendLength=4;
  
  assign.reply[0]=0x88; 
  assign.reply[1]=0x30;
  assign.reply[2]=0x00 | (cameranum+1); 
  assign.reply[3]=0xff;
  assign.replyLength=4;
  assign.end = 0xFF;
  ret.end = 0xFF;
 
  SendToCam(fd, &assign);
 
  if((error = ReadFromCam(fd, &ret,10)) < 0) return error;

  /* Check if proper reply was obtained. */
  else if(IsEqual(&assign, &ret) != 1) {

    fputs("10) Improper reply for initialization of SONY camera.(InitializeSony)\n",logfile);
    PrintError(&ret);
    fflush(logfile);
    return -10; 
  }
  
  return 0;
}

/*
 *
 * Clears the SONY interface to operate the camera.
 *
 * PARAMS: fd:- A file descriptor for the serial port.
 *
 * Returns negative number if it fails else it returns 0.
 *
 */
int ClearSonyInterface(int fd) {


  int retry = 0; /* Number of times command has been sent. */

  int error = 0;

  command clear; /* The instruction to clear the interface. */
  command reply; /* The reply from the camera. */

  clear.send[0]=0x88; clear.send[1]=0x01;
  clear.send[2]=0x00; clear.send[3]=0x01;
  clear.send[4]=0xff;
  clear.sendLength=5;
      
  clear.reply[0]=0x88; clear.reply[1]=0x01;
  clear.reply[2]=0x00; clear.reply[3]=0x01;
  clear.reply[4]=0xff;
  clear.replyLength=5;
    
  clear.end = 0xFF;
  reply.end = 0xFF;
  
  /* Send instruction for a certain number of times. */
  do {
    
    if(SendToCam(fd, &clear) != clear.sendLength) retry++;
    
  } while(retry < 5 && retry > 0);

  /* Check if the insruction was sent properly. */
  if(retry < 5) {
    
    if((error = ReadFromCam(fd, &reply, 1)) < 0) return error;
    /* Check if proper reply was obtained. */
    else if(IsEqual(&clear, &reply) != 1) {

      fputs("11) Improper reply for interface clearing of SONY camera.(ClearSonyInterface)\n",logfile);
      PrintError(&reply);
      fflush(logfile);
      return -11; 
    }
  }

  else {
   
    fputs("9) Program could not send instruction to camera.(ClearSonyInterface)\n",logfile);
    fflush(logfile);
    return -9;
  }

  return 0;
}

/*
 *
 * Initialises the CANON camera.
 *
 * PARAMS: fd:- A file descriptor for the serial port.
 *         retries:- The number of time to try sending query. 
 *
 * Returns negative number if it fails else it returns 0.
 *
 */
int InitializeCanon(int fd) {

 ioctl(fd,TCFLSH,0); /* Flush the input buffer */

  command assign;
  
  int reply = 0;
  
  //Request Host status control command    
  assign.send[0]= 0xff; assign.send[1]= 0x30;
  assign.send[2]= 0x30; assign.send[3]= 0x00;
  assign.send[4]= 0x86; assign.send[5]= 0x30;
  assign.send[6]= 0xef;
  assign.sendLength = 7;
  assign.end = 0xeF;
  
  reply = SendSynchCanon(fd, &assign);
  
  if( reply < 0) return reply;

  //If in local mode assign Host mode
  else  if( (assign.reply[5] & 0x04) ) {
    
      assign.send[0]= 0xff; 
      assign.send[1]= 0x30;
      assign.send[2]= 0x30; 
      assign.send[3]= 0x00;
      assign.send[4]= 0x90; 
      assign.send[5]= 0x30;
      assign.send[6]= 0xef;
      assign.sendLength = 7;
      assign.end = 0xeF;
      
      reply = SendSynchCanon(fd, &assign);
  
      if( reply < 0) return reply;
    }

  //Turn global termination notification on
  assign.send[0]= 0xff; 
  assign.send[1]= 0x30;
  assign.send[2]= 0x30; 
  assign.send[3]= 0x00;
  assign.send[4]= 0x94;
  assign.send[5]= 0x31;
  assign.send[6]= 0xEF;
  assign.sendLength = 7;

  if( (reply = SendSynchCanon(fd, &assign)) < 0) return reply;

  //Initialize pedestal to home position
  assign.send[0]= 0xFF;
  assign.send[1]= 0x30;
  assign.send[2]= 0x30;
  assign.send[3]= 0x00;
  assign.send[4]= 0x58;
  assign.send[5]= 0x31;
  assign.send[6]= 0xEF;
  assign.sendLength = 7;
  assign.end = 0xEF;
    
  reply = SendSynchCanon(fd, &assign);
 
  if(reply < 0) return reply;

  WaitForNotification(fd);

//Turn global termination notification off
  assign.send[0]= 0xff; 
  assign.send[1]= 0x30;
  assign.send[2]= 0x30; 
  assign.send[3]= 0x00;
  assign.send[4]= 0x94;
  assign.send[5]= 0x30;
  assign.send[6]= 0xEF;
  assign.sendLength = 7;

  if( (reply = SendSynchCanon(fd, &assign)) < 0) return reply;

  return 0;
}

/*
 *
 * Sends the instruction and blocks until a completion
 * or error message is received or the program times out
 * The timout is about 181 sec which is the longest time
 * it takes to PAN from one side to the other followed
 * by a TILT from bottom to top at a speed of 01H.
 *
 * PARAMS: fd:- The file descriptor.
 *         inst:- A pointer to the instruction.
 *         retries:- Number of time to retry sending
 *
 * Returns 0  if instruction completes correctly.
 *
 * If any error occurs, the program returns a 
 * negative index into the logfile where the error
 * message is written.
 *
 */
int SendSynchSony(int fd, command *inst, int retries) {
  
  ioctl(fd,TCFLSH,0); /* Flush the input. */
  
  int error = 0;

  int retry = 0; /* Number of times command has been sent. */
  
  /* Send instruction for a certain number of times. */
  do {

    //DEBUG PURPOSES  
    if(DEBUG) {
      PrintCmd(inst);
    }

    //TIMING PURPOSES  
    if(TIME) {
      STARTTIME;
    }
    if(SendToCam(fd, inst) != inst->sendLength) retry++;

    

  } while(retry < retries && retry > 0);

  /* Check if the insruction was sent properly. */
  if(retry < retries) {
    
    /* Get a reply. */
    do {
      error = ReadFromCam(fd, inst, 181);

      //TIMING PURPOSES  
      if(TIME) {
	ENDTIME;
      }
      
      //DEBUG PURPOSES  
      if(DEBUG) {
	PrintReply(inst);
      }

    } while(IsIrSony(inst));
    if(error < 0 ) return error;
    
    else {
      
      /* Check for error messages. */
      if(IsErrorSony(inst) == 1)  {
	
	fputs("12) Camera returned an error (SendSynch):\n",logfile);
	WhichErrorSony(inst);
	PrintError(inst);
	fflush(logfile);
	
	return -12;
      }
      
      else { 
	  
	if(IsAckSony(inst) != 1) {
	    
	  /*Previous instruction's complete message was obtained.
	   *Read acknowledge*/
	  do {
	    error = ReadFromCam(fd, inst, 180);

	    //DEBUG PURPOSES  
	    if(DEBUG) {
	      PrintReply(inst);
	    }
	    
	  } while(IsIrSony(inst));
	}
	
	/*Wait for complete*/
	do {
	  error = ReadFromCam(fd, inst, 180);

	  //DEBUG PURPOSES  
	  if(DEBUG) {
	    PrintReply(inst);
	  }

	} while(IsIrSony(inst));
	if(error < 0 ) return -11; 
	
	else if(IsCplSony(inst) != 1) {

	  if(IsErrorSony(inst) == 1) {
	      
	    fputs("12) Camera returned an error (SendSynch):\n",logfile);
	    WhichErrorSony(inst);
	    PrintError(inst);
	    fflush(logfile);
	      
	    return -12;
	  }
	    
	  else {
	      
	    fputs("13) Unknown message received (SendSynch):\n",logfile);
	    PrintError(inst);
	    fflush(logfile);
	    return -13;
	  }
	}
      }
    }
  }
  
  else {
    
    fputs("9) Program could not send instruction to camera.(SendSynch)\n",logfile);
    fflush(logfile);
    return -9;
  }
  
  return 0; // Command completed successfully
}

/*
 *
 * Sends the instruction and gets the reply message.
 *
 * PARAMS: fd:- The file descriptor.
 *         inst:- A pointer to the instruction.
 *
 * Returns 0  if instruction completes correctly.
 *
 * If any error occurs, the program returns a 
 * negative index into the logfile where the error
 * message is written.
 *
 */
int SendSynchCanon(int fd, command *inst) {

 ioctl(fd,TCFLSH,0); /* Flush the input buffer */

  int bytesReceived = 0;

  //TIMING PURPOSES  
  if(TIME) {
    STARTTIME;
  }

  /* Send the instruction */
  SendToCam(fd, inst);

  //DEBUG PURPOSES  
  if(DEBUG) {
  PrintCmd(inst);
  }

  bytesReceived = ReadFromCam(fd, inst, 10);
 
  //TIMING PURPOSES  
  if(TIME) {
    ENDTIME;
  }

  //DEBUG PURPOSES  
  if(DEBUG) {
    PrintReply(inst);
  }

    if( IsErrorCanon(inst) ) {
      
      canonError = *inst;
      fputs("16) Canon error received (SendSynchCanon):\n",logfile);
      PrintError(inst);
      WhichErrorCanon(inst);
      fflush(logfile);
      return -16;
    }  
  return 0;
}

/*
 *
 * Waits for termination notification from the canon camera.
 * The timout is about 180 sec which is the longest time
 * it takes to PAN from one side to the other followed
 * by a TILT from bottom to top at a speed of 01H.
 *
 * PARAMS: fd:- The file descriptor.
 *   
 * Returns 0  if instruction completes correctly.
 *
 * If any error occurs, the program returns a 
 * negative index into the logfile where the error
 * message is written.
 *
 */
int WaitForNotification(int fd) {

  command notification;
  notification.end = 0xEF;

  ReadFromCam(fd, &notification, 181);
  
  //DEBUG PURPOSES  
  if(DEBUG) {
    PrintReply(&notification);
  }
 
  return 0;
}
/*
 *
 * Sends the instruction without waiting for a
 * completion message.
 *
 * PARAMS: fd:- The file descriptor.
 *         inst:- A pointer to the instruction.
 *
 * This function does not return anything even if there is errors.
 *
 */
int SendAsynch(int fd, command *inst) {

  ioctl(fd,TCFLSH,0); /* Flush the input buffer */

  int error = 0;
  int ret = 0;

  //TIMING PURPOSES  
  if(TIME) {
    STARTTIME;
  }

  /* Send the instruction. */
  SendToCam(fd, inst);
  
  //DEBUG PURPOSES  
  if(DEBUG) {
    PrintCmd(inst);
  }
  
  do {
    error = ReadFromCam(fd, inst, 180);/* Read acknowledgement or Error. */

    //TIMING PURPOSES  
    if(TIME) {
      ENDTIME;
    }

    //DEBUG PURPOSES  
    if(DEBUG) {
      PrintReply(inst);
    }

  } while(IsIrSony(inst) || IsCplSony(inst));
    
  /* If it's an error print it to logfile. */
  if(IsErrorSony(inst) == 1) {
    
    fputs("12) Camera returned an error (SendAsynch):\n",logfile);
    WhichErrorSony(inst);
    PrintError(inst);
    fflush(logfile);
    return -12;
  }
  ret = (inst->reply[1] & 0x0F); //Return socket number
  
  return ret;
}

/*
 *
 * This function sends an inquiry instruction to the SONY camera
 * 
 * PARAMS: fd:- A file descriptor for the serial port.
 *         inq:- The inquiry command
 *
 * RETURNS: 0 or negative number
 *
 */
int Inquiry(int fd, command *inq) {
  
  ioctl(fd,TCFLSH,0); /* Flush the input. */
  
  //TIMING PURPOSES  
  if(TIME) {
    STARTTIME;
  }
  
  //DEBUG PURPOSES  
  if(DEBUG) {
    PrintCmd(inq);
  }

  /* Send the inquiry. */
  SendToCam(fd, inq);
  
  /* Get last serial input. */
  ReadFromCam(fd, inq, 1);

  //TIMING PURPOSES  
  if(TIME) {
    ENDTIME;
  }

  //DEBUG PURPOSES  
  if(DEBUG) {
    PrintReply(inq);
  }


  /* If there is a syntax error. */
  if(IsErrorSony(inq) == 1) {
    
    fputs("12) Camera returned an error (Inquiry):\n",logfile);
    WhichErrorSony(inq);
    PrintError(inq);
    fflush(logfile);
    return -12;
  }
  
  /* Check that previous instruction was not done before inquiy. */
  else if(IsCplSony(inq) == 1) {
    
    /* Read reply. */
    ReadFromCam(fd,inq, 1);
    
    /* If there is a syntax error. */
    if(IsErrorSony(inq) == 1) {
      
      fputs("12) Camera returned an error (Inquiry):\n",logfile);
      WhichErrorSony(inq);
      PrintError(inq);
      fflush(logfile);
      return -12;
    }
  }
  
  /* A good reply was obtained. Return 0 */
  return 0;
}

/*
 *
 * This function sends an inquiry instruction to the CANON camera
 * 
 * PARAMS: fd:- A file descriptor for the serial port.
 *         inq:- The inquiry command
 *
 * RETURNS: 0 if a good reply was obtained else a 
 *          negative number is returned.
 *
 */
int InquiryCanon(int fd, command *inq) {

  ioctl(fd,TCFLSH,0); /* Flush the input. */

  //TIMING PURPOSES  
  if(TIME) {
    STARTTIME;
  }

  //DEBUG PURPOSES  
  if(DEBUG) {
    PrintCmd(inq);
  }

  SendToCam(fd, inq);

  ReadFromCam(fd, inq, 5);
 
  //TIMING PURPOSES  
  if(TIME) {
    ENDTIME;
  }

  //DEBUG PURPOSES  
    if(DEBUG) {
      PrintReply(inq);
    }

  if( IsErrorCanon(inq) ) {
      
    canonError = *inq;
    fputs("16) Canon error received (InquiryCanon):\n",logfile);
    PrintError(inq);
    WhichErrorCanon(inq);
    fflush(logfile);
    return -16;
  }

  return 0;
}

/*
 *
 * This function returns all the port settings to their original value
 * 
 * PARAMS: fd:- A file descriptor for the serial port.
 *
 * RETURNS: 0 if a good reply was obtained else a 
 *          negative number is returned.
 *
 */
int ResetPort(int fd) {
  
  /* Sets the old attribute of the serial port */
  if(tcsetattr(fd, TCSANOW, &oldSetting) < 0 ) {
    
    fprintf(logfile, "8) tcsetattr error. Error: %s.(ResetPort)\n",strerror(errno));
    fflush(logfile);
    return -8;
  }

  close(fd);

  return 0;
}
/**************************************************************************/

/*
 * Checks if two instruction are equal or not.
 * 
 * PARAMS: first:- The first instruction.
 *         second:- The second instruction.
 *
 * Returns 1 if true and 0 if false.
 *
 */
int IsEqual(command *first, command *second) {

  int i = 0;
  
  int retVal = 1;
  
  if(first->replyLength == second->replyLength) { /* The same length ? */

    for(i=0; i<first->replyLength && retVal; i++)
      
      /* The same content ? */
       retVal = retVal && (second->reply[i] == first->reply[i]);

    return retVal;
  }

  else return 0;
}

/*
 *
 * Checks for acknowledgement from the SONY camera.
 *
 * PARAMS: ack:- A pointer to the command sent by the camera.
 *
 * Returns 1 if true and 0 for false.
 *
 */
int IsAckSony(command *ack) {

  return (ack->replyLength == 3 && ack->reply[0] == 0x90 && 
          (ack->reply[1] == 0x41 || ack->reply[1] == 0x42) && 
	  ack->reply[2] == 0xFF);
}

/*
 *
 * Checks for the completion reply from the SONY camera.
 *
 * PARAMS: cpl:- A pointer to the command sent by the camera.
 *
 * Returns 1 if true and 0 for false.
 *
 */
int IsCplSony(command *cpl) {
  
  return (cpl->replyLength == 3 && cpl->reply[0] == 0x90 &&
          (cpl->reply[1] == 0x51 || cpl->reply[1] == 0x52) && 
	  cpl->reply[2] == 0xFF);
}

/*
 *
 * Checks for possible errors sent by the SONY camera.
 *
 * PARAMS: err:- A pointer to the command sent by the camera.
 *
 * Returns 1 for error and 0 for no error.
 *
 */

int IsErrorSony(command *err) {
  
  return (err->replyLength == 4 && (err->reply[1] == 0x60 || 
					 err->reply[1] == 0x61 ||
					 err->reply[1] == 0x62 ||
					 err->reply[1] == 0x38 ) );
}

/*
 *
 * Checks for possible errors sent by the CANON camera.
 *
 * PARAMS: err:- A pointer to the command sent by the camera.
 *
 * Returns 1 for error and 0 for no error.
 *
 */

int IsErrorCanon(command *err) {

  unsigned char e0 = err->reply[3] & 0x0F;
  int error = 0;

  if( (e0 != 0) ) {
  
    error = 1;
  }

  return error;
}



/*
 *
 * Prints out the SONY error
 *
 * PARAMS: err:- A pointer to the command sent by the camera
 *
 */
void WhichErrorSony(command *err) {
 
  switch(err->reply[2]) {
    
  case 0x02:
    fputs("Command with illegal parameters was sent to the camera\n",logfile);
    fflush(logfile);
    break;
    
  case 0x03:
    fputs("No more commands can be accepted since two sockets are already full\n",logfile);
    fflush(logfile);
    break;
    
  case 0x04:
    fprintf(logfile,"Command in socket %d has been cancelled\n",(err->reply[1]&&0x0F));
    fflush(logfile);
    break;

  case 0x05:
    fputs("Invalid socket number or no command found in specified socket number\n",logfile);
    fflush(logfile);
    break;
    
  case 0x41:
    printf("Command cannot be executed due to certain condition of the camera\n");
    fputs("Command cannot be executed due to certain condition of the camera\n",logfile);
    fflush(logfile);
    break;
    
  default:
    fputs("Unrecognized error\n",logfile);
    fflush(logfile);
    break;
  }
}

/*
 *
 * Prints out the CANON error
 *
 * PARAMS: err:- A pointer to the command sent by the camera
 *
 */
void WhichErrorCanon(command *err) {

 switch(err->reply[3]) {
    
  case 0x31:
    fputs("Camera is busy\n",logfile);
    fflush(logfile);
    break;
    
 case 0x33:
   fputs("Command not valid\n",logfile);
   fflush(logfile);
   break;

  case 0x35:
    fputs("Parameter error\n",logfile);
    fflush(logfile);
    break;
    
  case 0x39:
    fputs("Mode error\n", logfile);
    fflush(logfile);
    break;

  default:
    fputs("Unrecognized error\n",logfile);
    fflush(logfile);
    break;

 }
}

/*
 *
 * Returns the last CANON error
 *
 */
command GetLastErrorCanon() {
  
  return canonError;
}

/*
 *
 * Checks for possible IR return sent by the SONY camera.
 *
 * PARAMS: ir:- A pointer to the command sent by the camera.
 *
 * Returns 1 for true and 0 for false.
 *
 */

int IsIrSony(command *ir) {
  
  return ((ir->replyLength == 7) && (ir->reply[1] == 0x07));
}

/*
 *
 * Prints the error to the logfile.
 *
 * PARAMS: err:- A pointer to the command sent by the camera.
 *
 */

void PrintError(command *err) {

   int i = 0;
 
  for (i=0;i<err->replyLength;i++) {

    fprintf(logfile,"%x ",err->reply[i]);
  }
  fprintf(logfile,"\n");
}

/*
 *
 * Print the contents of a sequence
 *
 */
 void PrintCmd(command *aC) {
   
   int i;
 
   for (i=0;i<aC->sendLength;i++) {

     printf("%x ",aC->send[i]);
  }
   printf("\nlength: %d\n",aC->sendLength); 
}

/*
 *
 * Print the contents of a sequence
 *
 */
void PrintReply(command *aC) {
  
  int i;
 
  for (i=0;i<aC->replyLength;i++) {

    printf("%x ",aC->reply[i]);
  }
  printf("\nlength: %d\n",aC->replyLength);
}
