
#ifndef ZOOMOPTICAL_H
#define ZOOMOPTICAL_H

#include "Zoom.h"
#include "LookupTable.h"
#include "CameraRxThread.h"
#include "CameraTxThread.h"
extern "C" {
#include "CommandLevel.h"
}

/**************************************************************
 *
 * ZoomOptical class
 * Description: Zooms using the lens in the Sony EVI-D100 camera
 *
 **************************************************************
 */
class ZoomOptical : public Zoom {
    
  public:
  
   /******************************************
    * ctor
    * Description: Sets defaults. Does not communicate with camera.
    ******************************************
    */ 
    ZoomOptical();
    
   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */          
    ~ZoomOptical();
    
   /******************************************
    * init
    * Description: Initializes class and points to an initialized serial port.
    * Parameters:  myCameraNumber - the number of the camera, as determined by another
    *                               object
    *              myCameraTxThread - The thread for transmitting commands to the
    *                                 camera
    *              myCameraRxThread - The thread for receiving commands to the
    *                                 camera
    ******************************************
    */           
    void init( UINT8 myCameraNumber, CameraTxThread *myCameraTxThread, 
      CameraRxThread *myCameraRxThread, CommandLevel *myCmdLvl);
    void init( UINT8 myCameraNumber ); //obsolete
       
   /******************************************
    * setLookup
    * Description: Copy the LookupTable from an array.
    * Parameter:   myTable - the array of values
    *              myLength - the length of that table
    *              min - the minimum value represented by myTable[0]
    *              max - the maximum value represented by myTable[myLength-1]
    *              myLookupForward - True if the table goes from reading to mag
    ******************************************
    */    
    void setLookup(const FLOAT *myTable, const UINT32 myLength, 
                   FLOAT min, FLOAT max, BOOL myDoForward);
                   
   /******************************************
    * setLookup
    * Description: Load a new LookupTable from file
    * Parameter:   fileName - the file's path
    ******************************************
    */                
    void setLookup( const CHAR *fileName );
    
   /******************************************
    * saveLookup
    * Description: Save a new LookupTable to file
    * Parameter:   fileName - the file's path
    ******************************************
    */                    
    void saveLookup( const CHAR *fileName );
    
   /******************************************
    * zoom
    * Description: Zooms the lens to a certain magnification.
    * Parameters:  magRaw - The new target magnification. This is the 
    *                       data understood by the camera and needs to
    *                       be nonlinearly converted to magnification.  
    ******************************************
    */
    void zoomRaw( UINT16 magRaw );
    
   /******************************************
    * inquiryRaw
    * Description: Returns the current zoom level
    * Parameter:   myZoomMag -  The  magnification. This is the 
    *              data understood by the camera and needs to
    *              be nonlinearly converted to magnification.  
    * Returns:     True if inquiry successful    
    ******************************************
    */    
    BOOL inquiryRaw( UINT16 *myZoomMag );
    
   /******************************************
    * zoom
    * Description: Zooms the lens to a certain magnification
    * Parameters:  magnification - The new target magnification
    ******************************************
    */   
    virtual void zoom( FLOAT magnification );
  
   /******************************************
    * inquiry
    * Description: Returns the current zoom level
    * Parameters:  myZoomMag - Returned as the magnification
    * Returns:     True if inquiry successful
    ******************************************
    */
    virtual BOOL inquiry( FLOAT *myZoomMag ); 

   /******************************************
    * inquiryBoth
    * Description: Returns the current zoom level in both raw and 
    *              looked up form
    * Parameters:  rawMag - The reading directly from the camera
    *              mag - The reading as interpreted by the lookup table
    * Returns:     True if inquiry successful    
    ******************************************
    */        
    BOOL inquiryBoth(UINT16 *rawMag, FLOAT *mag);
    
   /******************************************
    * inquiryAll
    * Description: Returns the current pan and tilt angles along with the 
    *              zoom level in both raw and looked up form
    * Parameters:  pan - returned as tilt angle
    *              tilt - Returned as tilt angle
    *              rawMag - The reading directly from the camera
    *              mag - The reading as interpreted by the lookup table
    * Returns:     True if inquiry successful    
    ******************************************
    */        
    BOOL inquiryAll(FLOAT *pan, FLOAT *tilt, UINT16 *rawMag, FLOAT *mag);    
    
   /******************************************
    * reset
    * Description: Returns magnification to the default
    ******************************************
    */       
    virtual void reset();
    
  private:
    //The ID of this camera
    UINT8 cameraNumber;
    
    //The number of instances of this class
    static UINT8 totalNumberOfCameras;
    
    //If synchronous, waits until the operation is finished before returning. Otherwise
    //returns immediately
    UINT8 sync;
    
    //True if the init() function has been called. false otherwise.
    BOOL initialized;
    
    //Converts the reading from the camera to magnification.
    //A lookup gives the magnification. A reverse lookup gives the raw reading. 
    LookupTable rdgToMagTbl;
    
    //Used for communicating with camera
    CameraTxThread *cameraTxThread; 
    CameraRxThread *cameraRxThread;
    
    CommandLevel *commandLevel;
    

};

#endif

/*
 * Zoom.h
 *
 * Description: Serves as an interface for the various types of zoom.
 *
 * Authors: Eric D Nelson
 *
 * Version:
 *     $Id: ZoomOptical.h,v 1.10 2005/09/06 19:44:17 edn2065 Exp $
 *
 * Revisions:
 *     $Log: ZoomOptical.h,v $
 *     Revision 1.10  2005/09/06 19:44:17  edn2065
 *     Moved HZO barriers to Zoom to help parallelism
 *
 *     Revision 1.9  2005/08/25 22:12:23  edn2065
 *     Made commandLevel a class. Added HZO pickoff point in SIKernel
 *
 *     Revision 1.8  2005/07/15 01:08:58  edn2065
 *     Integrating RxTxThread. Still has jerkiness bug
 *
 *     Revision 1.7  2005/06/22 16:57:40  edn2065
 *     Tied pantilt and zoom readings together to make them faster
 *
 *     Revision 1.6  2005/06/21 14:56:15  edn2065
 *     Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
 *
 *     Revision 1.5  2005/06/13 18:47:14  edn2065
 *     Calibration implemented without principle point finder.
 *
 *     Revision 1.4  2005/06/02 18:11:42  edn2065
 *     Tested ZoomOptical
 *
 *     Revision 1.3  2005/06/02 16:41:26  edn2065
 *     Added CameraComms
 *
 *     Revision 1.2  2005/06/02 15:58:44  edn2065
 *     Getting ready to create CameraComms class
 *
 *     Revision 1.1  2005/06/01 18:46:23  edn2065
 *     Create Zoom, now need to edit LookupTable before testing
 *
 *
 *
 */
