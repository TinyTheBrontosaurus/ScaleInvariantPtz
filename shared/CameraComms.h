
#ifndef CAMERACOMMS_H
#define CAMERACOMMS_H

#include "types.h"

extern "C" {
#include "CommandLevel.h"
}

/**************************************************************
 *
 * CameraComms class 
 * Description: Initializes and stops communication with Sony EVI-D100 cameras
 *
 **************************************************************
 */
class CameraComms {
 public:
   
   /******************************************
    * ctor
    * Description: Sets initialized to false. Saves camera number.
    ******************************************
    */ 
   CameraComms();

   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */      
   ~CameraComms(); 
  
   /******************************************
    * init
    * Description: Initalizes the camera communication
    * Parameters:  mySerialPort - the path to the serial port that the camera
    *                             is plugged into
    ******************************************
    */   
    SINT8 init(CHAR *mySerialPort );
    
   /******************************************
    * getCameraNumber
    * Description: Returns the number of the camera.
    * Returns:     The number of the camera so other classes
    *              can communicate with the camera.
    ******************************************
    */       
    UINT8 getCameraNumber();
    
    CommandLevel *getCommandLevel() { return &commandLevel; };
       
 private:
    //The ID of this camera
    UINT8 cameraNumber;
    
    //The number of instances of this class
    static UINT8 totalNumberOfCameras;
       
    //True if the init() function has been called. false otherwise.
    BOOL initialized;
    
    int myFd;
    
    CommandLevel commandLevel;
    
};

#endif

/*
 * CameraComms.h
 *
 *
 * Authors: Eric D Nelson
 *
 * Version:
 *     $Id: CameraComms.h,v 1.5 2005/08/25 22:12:23 edn2065 Exp $
 *
 * Revisions:
 *     $Log: CameraComms.h,v $
 *     Revision 1.5  2005/08/25 22:12:23  edn2065
 *     Made commandLevel a class. Added HZO pickoff point in SIKernel
 *
 *     Revision 1.4  2005/08/24 20:06:29  edn2065
 *     Put in band-aid for camera communications
 *
 *     Revision 1.3  2005/07/14 20:16:00  edn2065
 *     Added commenting to RxTxThread
 *
 *     Revision 1.2  2005/06/28 20:01:49  edn2065
 *     Debugging comms bugs. overnight check in
 *
 *     Revision 1.1  2005/06/02 16:41:26  edn2065
 *     Added CameraComms
 *
 *
 */
