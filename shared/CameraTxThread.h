#ifndef CAMERATXTHREAD_H
#define CAMERATXTHREAD_H


#include "types.h"
#include "Thread.h"
#include "Semaphore.h"
#include "Barrier.h"
#include "Time.h"

#define TX_BUF_PT_INQ_LENGTH 5
#define TX_BUF_Z_INQ_LENGTH 5
#define TX_BUF_PT_CMD_LENGTH 15
#define TX_BUF_Z_CMD_LENGTH 9

/**************************************************************
 *
 * CameraTxThread class 
 * Description: A thread that sends commands to the EVI-D100 camera
 *              while being sychronized with other threads.
 *              The only commands sent are P/T inquiries and commands
 *              and zoom inquiries and commands.
 *
 **************************************************************
 */
class CameraTxThread : public Thread {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults for all options.
    ******************************************
    */
    CameraTxThread();

   /******************************************
    * dtor
    ******************************************
    */
    ~CameraTxThread();
    
   /******************************************
    * init
    * Description: Initalizes the camera object
    * Parameters:  myCameraNumber - the number of the camera as set by CommandLevel
    *              myFd - the file descriptor to the camera's serial port as set
    *                     by CommandLevel
    *              myIterationBarrier - Synchronizes this thread so that it only 
    *                                   sends a certain number of commands per
    *                                   iteration
    *              mySocketsTaken - Pointer to number of sockets of camera in use. 
    *                               protected by mySocketsTakenSem
    *              mySocketsTakenSem - Synchronizes use of mySocketsTaken
    *              myClearToSendSem - Indicates when it is okay to send data
    ******************************************
    */       
    void init( UINT8 myCameraNumber, int myFd, 
      Barrier *myBeginIterationBarrier,
      Barrier *myEndIterationBarrier, UINT8 *mySocketsTaken,
      Semaphore *mySocketsTakenSem, Semaphore *myClearToSendSem,
      Semaphore *myPanTiltInquirySem, Semaphore *myZoomInquirySem,
      BOOL *myExecThread  );  
    
   /******************************************
    * setPTAngle
    * Description: Sets the pan and tilt angles to be sent to the camera
    * Parameters:  myPan - the pan angle
    *              myTilt - the tilt angle
    ******************************************
    */           
    void setPTAngle( FLOAT myPan, FLOAT myTilt );     
    
   /******************************************
    * setZMag
    * Description: Sets the zoom magnification to be sent to the camera
    * Parameters:  myZoomMag - the zoom magnification reading
    ******************************************
    */            
    void setZMag( UINT16 myZoomMag);
   
   /******************************************
    * stop
    * Description: Stops execution of the thread
    ******************************************
    */                
    void stop();     

  protected:   
    virtual void execute( void *a );
    
  private:
   /******************************************
    * writeBuf
    * Description: Writes a buffer to the serial port
    * Parameters: buf - The buffer to be sent
    *             length - The length of the buffer 
    ******************************************
    */        
    void writeBuf( UINT8 *buf, UINT8 length );
    
    //True when the thread is running
    BOOL *execThread;
    
    //The transmit buffers for inquries and commands
    UINT8 ptInqBuf[TX_BUF_PT_INQ_LENGTH]; 
    UINT8 zInqBuf [TX_BUF_Z_INQ_LENGTH]; 
    UINT8 ptCmdBuf[TX_BUF_PT_CMD_LENGTH]; 
    UINT8 zCmdBuf [TX_BUF_Z_CMD_LENGTH]; 
  
    //The number of the camera as set by CommandLevel
    UINT8 cameraNumber;  
    //File descriptor for serial port as set by CommandLevel
    int fd; 
    
    //Used to synchronize pan and tilt variables
    Semaphore ptAngleWriteSem;
    //Used to synchronize zoomMag variable
    Semaphore zMagWriteSem;
    
    //Synchronizes this thread so that it only sends a certain number of 
    //commands per iteration
    Barrier *beginIterationBarrier;
    Barrier *endIterationBarrier;
      
    //The magnitude of the zoom magnification to be sent
    UINT16 zoomMag;
    //The timestamp of the last send
    UINT32 zTimestamp;
    
    //The pan and tilt angles to be sent
    FLOAT pan;
    FLOAT tilt;
    //The timestamp of the last send
    UINT32 ptTimestamp;     
            
    //The number of sockets being used. TxThread only increments this variable
    UINT8 *socketsTaken;
    //Use to synchronize socketsTaken variable
    Semaphore *socketsTakenSem;   
    //Acquire when desired to send. TxThread only acquires this. RxThread releases
    //it when a response is received.
    Semaphore *clearToSendSem;
    
    //Semaphores to make sure inquiries have finished
    Semaphore *panTiltInquirySem;
    Semaphore *zoomInquirySem;
};

#endif


// File: $Id: CameraTxThread.h,v 1.3 2005/07/18 23:27:28 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: CameraTxThread.h,v $
// Revision 1.3  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.2  2005/07/14 20:16:00  edn2065
// Added commenting to RxTxThread
//
// Revision 1.1  2005/07/01 16:43:05  edn2065
// Created sine wave generator for camera
//
