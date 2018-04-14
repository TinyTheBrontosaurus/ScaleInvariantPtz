#include <stdio.h>
#include <unistd.h>
#include "Time.h"
#include "CameraTxThread.h"
#include "CommandLevel.h"

/*****************************************************************
 * CameraTxThread ctor
 *****************************************************************
 */

CameraTxThread::CameraTxThread():Thread()
{

}

/*****************************************************************
 * CameraTxThread dtor
 *****************************************************************
 */
CameraTxThread::~CameraTxThread() 
{  
  //Wait for it to finish
  join( 0 );
}

/*****************************************************************
 * CameraTxThread::init
 *****************************************************************
 */
void CameraTxThread::init( UINT8 myCameraNumber, int myFd, 
      Barrier *myBeginIterationBarrier,
      Barrier *myEndIterationBarrier, UINT8 *mySocketsTaken,
      Semaphore *mySocketsTakenSem, Semaphore *myClearToSendSem,
      Semaphore *myPanTiltInquirySem, Semaphore *myZoomInquirySem, 
      BOOL *myExecThread )
{
  //Save all the passed in parameters
  fd = myFd;
  beginIterationBarrier = myBeginIterationBarrier;
  endIterationBarrier = myEndIterationBarrier;
  socketsTaken = mySocketsTaken;
  socketsTakenSem = mySocketsTakenSem;
  clearToSendSem = myClearToSendSem; 
  panTiltInquirySem = myPanTiltInquirySem;
  zoomInquirySem = myZoomInquirySem;
  execThread = myExecThread;
  
  //Set up the buffers accordingly
  ptInqBuf[0] = 0x80 | myCameraNumber;
  ptInqBuf[1] = 0x09;
  ptInqBuf[2] = 0x06;
  ptInqBuf[3] = 0x12;
  ptInqBuf[4] = 0xFF;
  
  zInqBuf[0] = 0x80 | myCameraNumber;
  zInqBuf[1] = 0x09;
  zInqBuf[2] = 0x04;
  zInqBuf[3] = 0x47;
  zInqBuf[4] = 0xFF;

  ptCmdBuf[0] = 0x80 | myCameraNumber;
  ptCmdBuf[1] = 0x01;
  ptCmdBuf[2] = 0x06;
  ptCmdBuf[3] = 0x02;
  ptCmdBuf[4] = 0x18;  //full speed
  ptCmdBuf[5] = 0x14;  //full speed
  ptCmdBuf[6] = 0x00;
  ptCmdBuf[7] = 0x00;
  ptCmdBuf[8] = 0x00;
  ptCmdBuf[9] = 0x00;
  ptCmdBuf[10] = 0x00;
  ptCmdBuf[11] = 0x00;
  ptCmdBuf[12] = 0x00;
  ptCmdBuf[13] = 0x00;
  ptCmdBuf[14] = 0xFF;

  zCmdBuf[0] = 0x80 | myCameraNumber;
  zCmdBuf[1] = 0x01;
  zCmdBuf[2] = 0x04;
  zCmdBuf[3] = 0x47;
  zCmdBuf[4] = 0x00;
  zCmdBuf[5] = 0x00;
  zCmdBuf[6] = 0x00;
  zCmdBuf[7] = 0x00;
  zCmdBuf[8] = 0xFF;

}      
     
/*****************************************************************
 * CameraTxThread::execute
 *****************************************************************
 */
void CameraTxThread::execute( void *a ) 
{

  //Keep track of the number of iterations for the sake of managing 
  //commands
  UINT32 iterations = 0;
    
  UINT32 panFrequency = CAM_PAN_FREQUENCY;
  UINT32 zoomFrequency = CAM_ZOOM_FREQUENCY;
  
  #ifdef DEBUG_CAM_TX_TIMER
  Time timer;
  SINT32 bodyTime
  SINT32 syncTime;
  SINT32 ptInqTime;
  SINT32 zInqTime;
  SINT32 ptCmdTime;
  #endif
  
  SINT32 zCmdTime;  
  UINT32 recentWatchdog = 0;
  Time   writeTimer;
  
  while( true )
  {
    #ifdef DEBUG_CAM_TX_TIMER
    ptInqTime = -1;
    zInqTime = -1;
    ptCmdTime = -1;    
    #endif
    zCmdTime = -1;

    #ifdef DO_PANTILT  
    if( iterations % panFrequency == 0 )
    {
      //Signal a pan/tilt inquiry is being done this iteration
      panTiltInquirySem->acquire();    
    }
    #endif
        
    #ifdef DO_ZOOM_INQ
    //Signal a zoom inquiry is being done this iteration
    zoomInquirySem->acquire();    
    #endif
  
    
    beginIterationBarrier->wait();

    #ifdef DEBUG_CAM_TX_TIMER
    timer.tick();
    #endif    
    
    if( !(*execThread) )
    {
      
      break;
    }
        
#ifdef DO_PANTILT
    //Send P/T request
    if( iterations % panFrequency == 0 )
    {
      #ifdef DEBUG_CAM_TX_TIMER
      writeTimer.tick();
      #endif
      writeBuf( ptInqBuf, TX_BUF_PT_INQ_LENGTH );    
      #ifdef DEBUG_CAM_TX_TIMER
      ptInqTime = writeTimer.tock(false);
      #endif
    }
#endif
    
#ifdef DO_ZOOM_INQ
    #ifdef DEBUG_CAM_TX_TIMER
    writeTimer.tick(); 
    #endif
    //Send Z request
    writeBuf( zInqBuf, TX_BUF_Z_INQ_LENGTH );
    #ifdef DEBUG_CAM_TX_TIMER
    zInqTime = writeTimer.tock(false);
    #endif
#endif    
    
#ifdef DO_PANTILT
    //Send P/T command
    if( (iterations + 1) % panFrequency == 0 )
    {
      #ifdef DEBUG_CAM_TX_TIMER
      writeTimer.tick();
      #endif
      ptAngleWriteSem.acquire();    
      writeBuf( ptCmdBuf, TX_BUF_PT_CMD_LENGTH );
      ptAngleWriteSem.release();
      #ifdef DEBUG_CAM_TX_TIMER
      ptCmdTime = writeTimer.tock(false);
      #endif
    }
#endif

#ifdef DO_ZOOM_CMD    
    
    if( (iterations + 1) % zoomFrequency == 0 && recentWatchdog <= 0)
    {
      //Send Z command
      writeTimer.tick();
      zMagWriteSem.acquire();
      writeBuf( zCmdBuf, TX_BUF_Z_CMD_LENGTH );
      zMagWriteSem.release();
      zCmdTime = writeTimer.tock(false);
    }
#endif
     
    iterations++; 
    
    #ifdef DEBUG_CAM_TX_TIMER
    bodyTime = timer.tock(false);    
    timer.tick();
    #endif
    
    endIterationBarrier->wait();
    
    #ifdef DEBUG_CAM_TX_TIMER
    syncTime = timer.tock(false);
    printf( "Bod: %-4d ms Syn: %-4d ms Tot: %-4d ms Zi %-+4d Pi %-+4d Zc %-+4d Pc %-+4d" CRLF, 
            bodyTime, syncTime, bodyTime + syncTime, 
            zInqTime, ptInqTime, zCmdTime, ptCmdTime );
    #endif
    
    #ifdef CAM_TX_WATCHDOG_ENABLE        
    if( zCmdTime > CAM_TX_WATCHDOG_LATENCY )
    {
      if( recentWatchdog == 0 )
      {
        printf( "Watchdog bites! (%s @ %d)" CRLF, __FILE__, __LINE__ );
        recentWatchdog = CAM_TX_WATCHDOG_RESET;        
        
      }
      else
      {
        printf( "Watchdog waiting. (%s @ %d)" CRLF, __FILE__, __LINE__ );
        recentWatchdog--;
      }
    }
    else
    {
      if( recentWatchdog > 0 )
      {
        recentWatchdog--;
      }
    }
    #endif
    
  }
  zoomInquirySem->release();    
  panTiltInquirySem->release();   
}

/*****************************************************************
 * CameraTxThread::stop
 *****************************************************************
 */
void CameraTxThread::stop() 
{
  *execThread = false;
   
}

/*****************************************************************
 * CameraTxThread::getPTAngle
 *****************************************************************
 */
void CameraTxThread::setPTAngle( FLOAT myPan, FLOAT myTilt )
{
  UINT16 panHex;
  UINT16 tiltHex;
  
  //Convert to what the camera can understand
  panHex = (UINT16)((SINT16)(myPan/CAM_PANTILT_DEGREES_PER_UNIT)); 
  tiltHex = (UINT16)((SINT16)(myTilt/CAM_PANTILT_DEGREES_PER_UNIT)); 
      
  //Set the command buffer accordingly, but make sure a send is not being done
  ptAngleWriteSem.acquire();
  pan = myPan;
  tilt = myTilt;
  ptCmdBuf[6] = (UINT8)(panHex >> 12)   & 0x0F;
  ptCmdBuf[7] = (UINT8)(panHex >> 8)    & 0x0F;
  ptCmdBuf[8] = (UINT8)(panHex >> 4)    & 0x0F;
  ptCmdBuf[9] = (UINT8)(panHex)         & 0x0F;
  ptCmdBuf[10] = (UINT8)(tiltHex >> 12) & 0x0F;
  ptCmdBuf[11] = (UINT8)(tiltHex >> 8)  & 0x0F;
  ptCmdBuf[12] = (UINT8)(tiltHex >> 4)  & 0x0F;
  ptCmdBuf[13] = (UINT8)(tiltHex)       & 0x0F;    
  ptAngleWriteSem.release();
  
}
    
/*****************************************************************
 * CameraTxThread::getZMag
 *****************************************************************
 */
void CameraTxThread::setZMag( UINT16 myZoomMag )
{

  //Set the command buffer accordingly, but make sure a send is not being done
  zMagWriteSem.acquire();
  zCmdBuf[4] = (UINT8)(myZoomMag >> 12) & 0x0F;
  zCmdBuf[5] = (UINT8)(myZoomMag >> 8) & 0x0F;
  zCmdBuf[6] = (UINT8)(myZoomMag >> 4) & 0x0F;
  zCmdBuf[7] = (UINT8)(myZoomMag) & 0x0F;
  zMagWriteSem.release();
  
}

/*****************************************************************
 * CameraTxThread::writeBuf
 *****************************************************************
 */
void CameraTxThread::writeBuf( UINT8 *buf, UINT8 length )
{  
  UINT8 totalWritten;
  UINT8 justWritten;
  CHAR output[100];

  totalWritten = 0;

  //Wait until all is clear on the bus  
  clearToSendSem->acquire();      
      
  while( totalWritten < length )
  {
    totalWritten += 
      write(fd, &buf[totalWritten], length - totalWritten );
  }
  
#ifdef DEBUG_CAM_TX
  sprintf( output, "    Tx: " );
  for( UINT8 ctr = 0; ctr < length; ctr++ )
  {
    sprintf( output, "%s%2X ", output, buf[ctr] );
    if( ctr % 4 == 3 && ctr != length - 1)
    {
      sprintf( output, "%s| ", output );
    }
  }
  printf( "%s (%s @ %d)" CRLF, output, __FILE__, __LINE__ );
#endif
}

// File: $Id: CameraTxThread.cpp,v 1.9 2005/09/11 01:13:59 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: CameraTxThread.cpp,v $
// Revision 1.9  2005/09/11 01:13:59  edn2065
// Added TxThread Watchdog
//
// Revision 1.8  2005/07/26 18:46:43  edn2065
// Added scale invariance
//
// Revision 1.7  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.6  2005/07/15 02:50:58  edn2065
// Working on adding synch to fix instability
//
// Revision 1.5  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.4  2005/07/14 20:16:00  edn2065
// Added commenting to RxTxThread
//
// Revision 1.3  2005/07/13 23:54:49  edn2065
// Power off save
//
// Revision 1.1  2005/07/01 16:43:05  edn2065
// Created sine wave generator for camera
//
