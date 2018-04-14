#ifndef SIDUALCAM_H
#define SIDUALCAM_H


#include "types.h"
#include "Thread.h"
#include "Video.h"
#include "SISingleCam.h"
#include "SIDualCamOptions.h"
#include "FixationInfo.h"

/**************************************************************
 *
 * SIDualCam class
 * Description: Puts two SISingleCam cameras together to do
 *              Hybrid Zoom Out.
 **************************************************************
 */
class SIDualCam : public Thread {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults for all options.
    ******************************************
    */
    SIDualCam();

   /******************************************
    * dtor
    ******************************************
    */
    ~SIDualCam();

   /******************************************
    * applyOptions
    * Description: Signals to copy the values of the public options to the
    *              internal options; applies changes to options.
    *              Threadsafe.
    ******************************************
    */
    void applyOptions();

   /******************************************
    * reset
    * Description: Signals to reset the camera.
    *              Threadsafe.
    ******************************************
    */
    void reset();     
    
   /******************************************
    * setupCameras
    * Description: Signals Hzo to set up the cameras for proper HZO and
    *              applies changes to options.
    *              Threadsafe.
    ******************************************
    */
    void setupCameras();

    
    
    SISingleCam *getZoomCam() { return zoomCam; }
    SISingleCam *getPanCam() { return panCam; }
           
  protected:
    virtual void execute(void *);
  private:
  
   /******************************************
    * updateOptions
    * Description: Copies the values of the public options to the
    *              internal options; applies changes to options..
    ******************************************
    */
    void updateOptions();  
  
   /******************************************
    * processFrame
    * Description: Do all processing that needs to be done for
    *              a new frame.
    * Returns:     False if the video stream has ended. true otherwise.
    ******************************************
    */
    void arbitrateControl();

   /******************************************
    * updateOptions
    * Description: Copies the values of the public options to the
    *              internal options; applies changes to options..
    ******************************************
    */
    void arbitrateDisplay();     

   /******************************************
    * saveStats
    * Description: Logs statistics for this frame
    ******************************************
    */
    void saveStats();     
    
   /******************************************
    * determineState
    * Description: Determines whether the object is correctly in view of
    *              the cameras
    ******************************************
    */        
    void determineState();   
    
  public:
    //The public version of the various settings used by this object.
    //The settings are not used until applyOptions is called, which copies
    //the values from options to intOptions.
    SIDualCamOptions options;
       
  private:
    //The internal version of the various settings used by this object.
    //These are the settings used during execution.  
    SIDualCamOptions intOptions;
    
    //True when running
    BOOL threadRunning;
    
    //True when an applyOptions call is pending
    BOOL apply;
    
    //True when a rest is penting
    BOOL doReset;    
    BOOL doResetNextIter;
           
    //True when the end of the thread is pending
    BOOL signalThreadStop;
    
    //True when camera setup is pending
    BOOL signalSetupCameras;
     
    //The cameras to put together
    SISingleCam *panCam;    
    SISingleCam *zoomCam;
    
    //The output
    VideoDestination *vDst;
    //The screen output
    VideoDestinationScreen vDstScreen;  
    //File output
    VideoDestinationFileRamDrive vDstFile;      
    
    //Synchronization with SISingleCams
    Barrier movePanImgToZoomBgBarrier;
    Barrier startZoomBarrier;
    Barrier endZoomBarrier;
    Barrier postArbitrationBarrier;
    Barrier dummyBarrier;
    
    //The states of the cameras
    BOOL zoomCamInView;
    BOOL panCamInView;
    BOOL camerasSyncd;
    //The number of consecutive bad frames that have been been sent recently.
    UINT32 consecutiveBadFrames;
    
    //CVC parameters
    FLOAT cvcZ;
    FLOAT cvcX;
    FLOAT cvcArea;
    
    //Logging
    MeasureFinalSizeLogger statsLogger;
    BOOL saveJustActivated;
    
    
    
};

#endif


// File: $Id: SIDualCam.h,v 1.14 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SIDualCam.h,v $
// Revision 1.14  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.13  2005/09/10 00:35:36  edn2065
// synchronized video streams to file (kind of)
//
// Revision 1.12  2005/09/09 20:49:42  edn2065
// Improved robustness of stateDetermination using conescutive bad frames
//
// Revision 1.11  2005/09/09 02:43:00  edn2065
// Tried to align files
//
// Revision 1.10  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.9  2005/09/08 19:07:49  edn2065
// Created RAM drive for file storage
//
// Revision 1.8  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.7  2005/09/07 01:05:16  edn2065
// Finished CVC implementation
//
// Revision 1.6  2005/08/27 22:42:24  edn2065
// Working out CVC
//
// Revision 1.5  2005/08/26 20:27:13  edn2065
// Pickoff points work. Implemented HZO camera setup
//
// Revision 1.4  2005/08/26 18:04:35  edn2065
// Added two pickoff points for SIDualCam and added better naming convention to barriers
//
// Revision 1.3  2005/08/26 00:57:21  edn2065
// Added menu options for HZO. Setup control arbitration
//
// Revision 1.2  2005/08/25 23:55:59  edn2065
// Removed deadlock in SIDualCam and separated SISingleCams from GUI
//
// Revision 1.1  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SISingleCam
//
