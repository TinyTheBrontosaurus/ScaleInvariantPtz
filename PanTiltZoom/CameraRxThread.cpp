#include <stdio.h>
#include <unistd.h>
#include "Time.h"
#include "CommandLevel.h"
#include "CameraRxThread.h"


/*****************************************************************
 * CameraRxThread ctor
 *****************************************************************
 */

CameraRxThread::CameraRxThread():Thread()
{
  
}

/*****************************************************************
 * CameraRxThread dtor
 *****************************************************************
 */
CameraRxThread::~CameraRxThread() 
{
  //Signal the thread to stop
  stop();
  //Wait for it to finish
  join( 0 );
}

/*****************************************************************
 * CameraRxThread::init
 *****************************************************************
 */
void CameraRxThread::init( UINT8 myCameraNumber, int myFd, UINT8 *mySocketsTaken,
      Semaphore *mySocketsTakenSem, Semaphore *myClearToSendSem,
      Semaphore *myPanTiltInquirySem, Semaphore *myZoomInquirySem,
      BOOL *myExecThread  )
{

  //Save all the parameters
  cameraNumber = myCameraNumber;
  fd = myFd;
  socketsTaken = mySocketsTaken;
  socketsTakenSem = mySocketsTakenSem;
  clearToSendSem = myClearToSendSem; 
  panTiltInquirySem = myPanTiltInquirySem;
  zoomInquirySem = myZoomInquirySem;
  execThread = myExecThread;
  recentZRead = false;
  recentPTRead = false;
  
  //Reset the buffer
  bytesRead = 0;

}      
/*****************************************************************
 * CameraRxThread::execute
 *****************************************************************
 */
void CameraRxThread::execute( void *a ) 
{

  BOOL gotReply;
  CHAR output[100];

  while( *execThread )
  {
  
    //Keep polling for the end of a message to be received  
    if( readByte() )
    {
      //Set flag for if the reply was recognized
      gotReply = false;
    
      //See what kind of packet was received
      switch( bytesRead )
      {
      
        //ACK or Completion
        case 3:
          //Set flag indicating a response
          gotReply = true;
          break;
          
        //Zoom inquiry response
        case 7:
          //Save zoom reading, but be sure there is not another thread trying to 
          //read the variable
          zMagReadSem.acquire();
          zTimestamp = Time::getTimestamp();
          zoomMag = hexConvert(buffer, 2);
          recentZRead = true;
          zMagReadSem.release();
          zoomInquirySem->release();
          
          //Set flag indicating a response
          gotReply = true;
          break;
          
        case 11:
          //Save pan/tilt reading, but be sure there is not another thread trying 
          //to read the variables
          ptAngleReadSem.acquire();
          ptTimestamp = Time::getTimestamp();
          pan = ((SINT16)hexConvert(buffer, 2)) *
            CAM_PANTILT_DEGREES_PER_UNIT;
          tilt = ((SINT16)hexConvert(buffer, 6))*
            CAM_PANTILT_DEGREES_PER_UNIT;
          recentPTRead = true;  
          ptAngleReadSem.release();
          panTiltInquirySem->release();
          
          //Set flag indicating a response
          gotReply = true;
          break;
        
        default:
          //Error
          #ifdef DEBUG_CAM_RX
          //Print out error
          sprintf( output, "Unknown Packet Received: " );
          for( UINT8 ctr = 0; ctr < bytesRead; ctr++ )
          {
            sprintf( output, "%s%2X ", output, buffer[ctr] );
            if( ctr % 4 == 3 && ctr != bytesRead - 1)
            {
              sprintf( output, "%s| ", output );
            }
          }
          printf( "%s (%s @ %d)" CRLF, output, __FILE__, __LINE__ );
          #endif
          
          break;
      }
          
      //If there is a response, then indicate that it is clear to send another 
      //command
      if( gotReply )
      {
      #ifdef DEBUG_CAM_RX
        //Print out the received command
        sprintf( output, "      Rx: " );
        for( UINT8 ctr = 0; ctr < bytesRead; ctr++ )
        {
          sprintf( output, "%s%2X ", output, buffer[ctr] );
          if( ctr % 4 == 3 && ctr != bytesRead - 1)
          {
            sprintf( output, "%s| ", output );
          }
        }
        printf( "%s (%s @ %d)" CRLF, output, __FILE__, __LINE__ );
      #endif
        
        clearToSendSem->release();
        
      }
      bytesRead = 0;
    }
  
  }
  zoomInquirySem->release();    
  panTiltInquirySem->release(); 

}

/*****************************************************************
 * CameraRxThread::stop
 *****************************************************************
 */
void CameraRxThread::stop() 
{
  *execThread = false;
}

/*****************************************************************
 * CameraRxThread::getPTAngle
 *****************************************************************
 */
BOOL CameraRxThread::getPTAngle( FLOAT *myPan, FLOAT *myTilt, UINT32 *timestamp )
{
  //True if a recent read has occurred
  BOOL retval;

  //Make sure there is not a read happening right now, then save the local values
  //for the calling function
  ptAngleReadSem.acquire();
  *myPan = pan;
  *myTilt = tilt;
  *timestamp = ptTimestamp;
  
  //Save and reset the recently read flag
  retval = recentPTRead;
  recentPTRead = false;
  
  ptAngleReadSem.release();  
  return retval;  
  
}
    
/*****************************************************************
 * CameraRxThread::getZMag
 *****************************************************************
 */
BOOL CameraRxThread::getZMag( UINT16 *myZoomMag, UINT32 *timestamp )
{

  BOOL retval;

  //Make sure there is not a read happening right now, then save the local values
  //for the calling function
  zMagReadSem.acquire();
  *myZoomMag = zoomMag;
  *timestamp = zTimestamp;
  
  //Save and reset the recently read flag
  retval = recentZRead;
  recentZRead = false;
  
  zMagReadSem.release();
  return retval;

}

/*****************************************************************
 * CameraRxThread::readByte
 *****************************************************************
 */
BOOL CameraRxThread::readByte()
{
  BOOL finishedPacket = false;
  
  //Read 1 byte into the buffer from the serial port.
  if( read(fd, &buffer[bytesRead], 1) != -1 )
  { 
    bytesRead++;
    if( buffer[bytesRead-1] == 0xFF )
    {
      finishedPacket = true;
    }        
    else if( buffer[bytesRead-1] == (cameraNumber+8)<<4 )
    {
      //Beginning of packet
      buffer[0] = buffer[bytesRead-1];
      bytesRead = 1;
    }
  }
  
  return finishedPacket;
}

/*****************************************************************
 * CameraRxThread::HexConvert
 *****************************************************************
 */
//Convert to hex numbers stored in an array.
UINT16 CameraRxThread::hexConvert(UINT8 *hex, int firstpos) 
{

  UINT16 position = 0;

  position += hex[firstpos]   << 12;
  position += hex[firstpos+1] << 8; 
  position += hex[firstpos+2] << 4; 
  position += hex[firstpos+3]; 
  
  return position;
}

// File: $Id: CameraRxThread.cpp,v 1.6 2005/07/28 01:42:18 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: CameraRxThread.cpp,v $
// Revision 1.6  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.5  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.4  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.3  2005/07/14 20:23:39  edn2065
// Finished name change for RxTxThread
//
// Revision 1.2  2005/07/14 20:16:00  edn2065
// Added commenting to RxTxThread
//
// Revision 1.1  2005/07/01 16:43:05  edn2065
// Created sine wave generator for camera
//
