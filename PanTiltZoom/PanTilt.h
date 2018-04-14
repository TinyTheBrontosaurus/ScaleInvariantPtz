
#ifndef PANTILT_H
#define PANTILT_H

#include "types.h"
#include "CameraTxThread.h"
#include "CameraRxThread.h"

#define CAM_MIN_PAN_POS    -100.0
#define CAM_MAX_PAN_POS     100.0
#define CAM_MIN_PAN_SPEED     1.0
#define CAM_MAX_PAN_SPEED    24.0
#define CAM_MIN_TILT_POS    -25.0
#define CAM_MAX_TILT_POS     25.0
#define CAM_MIN_TILT_SPEED    1.0
#define CAM_MAX_TILT_SPEED   20.0

/**************************************************************
 *
 * PanTilt class 
 * Description: Controls the panning and tilting of a Sony
 *              EVI-D100 camera.
 *
 **************************************************************
 */
class PanTilt {
 public:
   
   /******************************************
    * ctor
    * Description: Sets pan and tilt to default. Does not communicate with camera.
    ******************************************
    */ 
   PanTilt();

   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */      
   ~PanTilt();
  
  
   /******************************************
    * init
    * Description: Initalizes the camera communication and resets position
    * Parameters:  cameraNumber - the number of the camera for communication purposes
    *              myCameraTxThread - The thread for transmitting commands to the
    *                                 camera
    *              myCameraRxThread - The thread for receiving commands to the
    *                                 camera
    ******************************************
    */   
    void init( UINT8 myCameraNumber, CameraTxThread *myCameraTxThread, 
      CameraRxThread *myCameraRxThread );
    void init( UINT8 myCameraNumber); //obsolete
  
   /******************************************
    * move
    * Description: Pans and tilts to a position at certain speeds
    * Parameters:  panPos - The target pan position in degrees
    *              panSpeed - The speed at which the camera will move toward the target pan
    *                         position
    *              tiltPos - The target tilt position in degrees
    *              tiltSpeed - The speed at which the camera will move toward the target tilt
    *                         position
    * Returns:     0 if successful. -1 if failed
    ******************************************
    */
    SINT8 move( FLOAT panPos,  FLOAT panSpeed, 
                FLOAT tiltPos, FLOAT tiltSpeed); 
   
   /******************************************
    * move
    * Description: Pans and tilts to a position at maximum speed
    * Parameters:  panPos - The target pan position in degrees
    *              tiltPos - The target tilt position in degrees
    * Returns:     0 if successful. -1 if failed
    ******************************************
    */
    SINT8 move( FLOAT panPos, FLOAT tiltPos );                  
   
   /******************************************
    * reset
    * Description: Commands the camera to return to the home position
    * Returns:     0 if successful. -1 if failed
    ******************************************
    */
    SINT8 reset();
    
   /******************************************
    * getPanTilt
    * Description: Retrieves the pan and tilt angles from the camera
    * Parameters:  pan - returned as the pan angle to the nearest degree
    *              tilt - returned as the tilt angle to the nearest degree
    * Returns:     True if successful
    ******************************************
    */
    BOOL getPanTilt(FLOAT *pan, FLOAT *tilt);
    
 private:
    //The ID of this camera
    UINT8 cameraNumber;
    
    //If synchronous, waits until the operation is finished before returning. Otherwise
    //returns immediately
    UINT8 sync;
    
    //Used for communicating with camera
    CameraTxThread *cameraTxThread; 
    CameraRxThread *cameraRxThread;
    
};

#endif

/*
 * camera.h
 *
 * Description: Class for controlling cameras pan/tilt
 *
 * Authors: John Ruppert
 *          Jared Holsopple
 *          Justin Hnatow
 *          Eric D Nelson
 *
 * Version:
 *     $Id: PanTilt.h,v 1.10 2005/07/15 01:08:58 edn2065 Exp $
 *
 * Revisions:
 *     $Log: PanTilt.h,v $
 *     Revision 1.10  2005/07/15 01:08:58  edn2065
 *     Integrating RxTxThread. Still has jerkiness bug
 *
 *     Revision 1.9  2005/06/21 11:13:04  edn2065
 *     Added delay handling for P/T/Z.
 *
 *     Revision 1.8  2005/06/14 12:19:58  edn2065
 *     Made PT inquiry a float instead of int. Camera resolution was being thrown away
 *
 *     Revision 1.7  2005/06/09 13:48:03  edn2065
 *     Fixed synchronization errors caused by not initializing Barrier in SIKernal
 *
 *     Revision 1.6  2005/06/02 16:41:26  edn2065
 *     Added CameraComms
 *
 *     Revision 1.5  2005/06/01 18:46:23  edn2065
 *     Create Zoom, now need to edit LookupTable before testing
 *
 *     Revision 1.4  2005/06/01 17:40:17  edn2065
 *     PanTilt tested
 *
 *     Revision 1.3  2005/06/01 16:25:44  edn2065
 *     Worked out compilation issues
 *
 *     Revision 1.1  2005/06/01 13:37:58  edn2065
 *     Copied from camera.h and .cpp from JJJ
 *
 *     Revision 1.9  2005/03/11 21:27:43  edn2065
 *     Works like before but now with LookupTable
 *
 *     Revision 1.8  2005/03/11 20:47:03  edn2065
 *     Reverse looup table more trouble than worth
 *
 *     Revision 1.7  2005/03/11 17:42:09  edn2065
 *     Check in before adding lookup table
 *
 *     Revision 1.6  2005/02/01 16:13:31  jdh5443
 *     updated ppdeg with all values
 *
 *     Revision 1.5  2005/01/23 21:11:08  jdh5443
 *     Added lookup for x5 and x6
 *
 *     Revision 1.4  2005/01/18 21:48:10  jdh5443
 *     No changes
 *
 *     Revision 1.3  2005/01/14 04:39:15  jdh5443
 *     Updated class to correspond with changes in cpp file
 *
 *     Revision 1.2  2004/12/22 16:50:34  jar4773
 *     Added panTiltRelPos function
 *
 *     Revision 1.1  2004/12/22 15:39:41  jar4773
 *     Initial revision
 *
 *
 */
