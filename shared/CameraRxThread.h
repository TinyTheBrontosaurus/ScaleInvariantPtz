#ifndef CAMERARXTHREAD_H
#define CAMERARXTHREAD_H


#include "types.h"
#include "Thread.h"
#include "Semaphore.h"
#include "Barrier.h"
#include "Time.h"

/**************************************************************
 *
 * CameraRxThread class 
 * Description: A thread that receives responses from the EVI-D100 camera
 *              while being sychronized with other threads.
 *              The only responses this thread expects are 
 *
 **************************************************************
 */
class CameraRxThread : public Thread {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults for all options.
    ******************************************
    */
    CameraRxThread();

   /******************************************
    * dtor
    ******************************************
    */
    ~CameraRxThread();
    
   /******************************************
    * init
    * Description: Initalizes the camera object
    * Parameters:  myCameraNumber - the number of the camera as set by CommandLevel
    *              myFd - the file descriptor to the camera's serial port as set
    *                     by CommandLevel
    *              mySocketsTaken - Pointer to number of sockets of camera in use. 
    *                               protected by mySocketsTakenSem
    *              mySocketsTakenSem - Synchronizes use of mySocketsTaken
    *              myClearToSendSem - Indicates when it is okay to send data
    ******************************************
    */           
    void init( UINT8 myCameraNumber, int myFd, UINT8 *mySocketsTaken,
      Semaphore *mySocketsTakenSem, Semaphore *myClearToSendSem,
      Semaphore *myPanTiltInquirySem, Semaphore *myZoomInquirySem,
      BOOL *myExecThread  );  
    
    /******************************************
    * init
    * Description: Initalizes the camera object
    * Parameters:  myPan - The most recent read pan angle
    *              myTilt - The most recent read tilt angle
    *              timestamp - The timestamp of the most recent P/T inquiry
    * Returns:     True if there has been a recent successful read
    ******************************************
    */               
    BOOL getPTAngle( FLOAT *myPan, FLOAT *myTilt, UINT32 *timestamp );     
    
   /******************************************
    * init
    * Description: Initalizes the camera object
    * Parameters:  myZoomMag - The most recent read zoom magnification
    *              timestamp - The timestamp of the most recent zoom inquiry
    * Returns:     True if there has been a recent successful read
    ******************************************
    */               
    BOOL getZMag( UINT16 *myZoomMag, UINT32 *timestamp );
    
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
    * readByte
    * Description: Reads a single byte from the serial port
    * Returns: True if that byte if 0xFF
    ******************************************
    */    
    BOOL readByte();
    
   /******************************************
    * hexConvert
    * Description: Converts a hexadecimal number, as received from the camera,
    *              to an integer
    * Parameters: hex - The hexadecimal number buffer in the camera's format
    *             firstpos - The index in hex in which the number starts
    * Returns: The converted number
    ******************************************
    */        
    UINT16 hexConvert(UINT8 *hex, int firstpos); 
    
    //True when the thread is running
    BOOL *execThread; 
    
    //The read buffer
    UINT8 buffer[100]; //long enough
    //The number of bytes read since the last 0xFF received
    UINT8 bytesRead;
    
    
    //The number of the camera as set by CommandLevel
    UINT8 cameraNumber;  
    //File descriptor for serial port as set by CommandLevel
    int fd; 
    
    //Used to synchronize pan and tilt variables    
    Semaphore ptAngleReadSem;
    //Used to synchronize zoomMag variable
    Semaphore zMagReadSem;
    
    //True if there hasn't been a call of getZMag since the last receive from
    //the camera
    BOOL recentZRead;
    //True if there hasn't been a call of getPTAngle since the last receive from
    //the camera
    BOOL recentPTRead;
        
    //The most recent zoom magnification reading received from the camera
    UINT16 zoomMag;
    //The timestamp of that receive
    UINT32 zTimestamp;
    
    //The most recent pan and tilt angles received from the camera
    FLOAT pan;
    FLOAT tilt;
    //The timestamp of that receive
    UINT32 ptTimestamp;  
    
    //The number of sockets being used. RxThread only increments this variable
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


// File: $Id: CameraRxThread.h,v 1.4 2005/07/28 01:42:18 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: CameraRxThread.h,v $
// Revision 1.4  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.3  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.2  2005/07/14 20:16:00  edn2065
// Added commenting to RxTxThread
//
// Revision 1.1  2005/07/01 16:43:05  edn2065
// Created sine wave generator for camera
//
