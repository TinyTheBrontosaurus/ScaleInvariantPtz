#ifndef SISINGLECAM_H
#define SISINGLECAM_H


#include "Time.h"
#include "Thread.h"
#include "Video.h"
#include "TrackerColorSegmentation.h"
#include "SISingleCamOptions.h"
#include "SICameraComms.h"
#include "types.h"
#include "TrackingInfo.h"
#include "TrackingReply.h"
#include "MeasureFinalSizeLogger.h"


/**************************************************************
 *
 * SISingleCam class
 * Description: The API for the kernel that controls the Scale
 *              Invariant Tracking program.
 **************************************************************
 */
class SISingleCam : public Thread {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults for all options and initializes
    *              all images to null.
    ******************************************
    */
    SISingleCam( Barrier *myHzoPreZoomPrePickoffBarrier,
              Barrier *myHzoPreZoomPostPickoffBarrier,   
              Barrier *myHzoPostZoomPrePickoffBarrier,
              Barrier *myHzoPostZoomPostPickoffBarrier,
             CHAR *myTitleString, CHAR *myConfigString = "last");

   /******************************************
    * dtor
    ******************************************
    */
    ~SISingleCam();

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
    
    void stop() { signalThreadStop = true;}
   
   /******************************************
    * getVideoStream
    * Description: Returns a pointer to the videoStream.
    * Returns:     A pointer to the video stream.
    ******************************************
    */
    VideoStream *getVideoStream() { return &videoStream; } 
    
   /******************************************
    * zoomStep
    * Description: Signals to have the zoom step to the position 
    *              indicated in the options.
    *              Threadsafe.
    ******************************************
    */
    void zoomStep();
    
   /******************************************
    * zoomPulse
    * Description: Signals to have the zoom pulse between the positions
    *              indicated in the options.
    *              Threadsafe.
    ******************************************
    */
    void zoomPulse();
    
   /******************************************
    * panTiltStep
    * Description: Signals to have the pan/tilt step to the position 
    *              indicated in the options.
    *              Threadsafe.
    ******************************************
    */
    void panTiltStep();        

   /******************************************
    * panTiltPulse
    * Description: Signals to have the pan/tilt pulse between the positions
    *              indicated in the options.
    *              Threadsafe.
    ******************************************
    */
    void panTiltPulse();    
    
   /******************************************
    * saveZoomCalibrationTable
    * Description: Signals to have the lookup table for calibration saved.
    *              Threadsafe.
    ******************************************
    */
    void saveZoomCalibrationTable();         
    
   /******************************************
    * loadZoomCalibrationTable
    * Description: Signals to have the lookup table for calibration loaded.
    *              Threadsafe.
    ******************************************
    */
    void loadZoomCalibrationTable();      

   /******************************************
    * startCalibration
    * Description: Signals for calibration to start.
    *              Threadsafe.
    ******************************************
    */        
    void startCalibration();

   /******************************************
    * stopCalibration
    * Description: Signals for calibration to stop.
    *              Threadsafe.
    ******************************************
    */        
    void stopCalibration();    
    
    FixationInfo *getFixationInfo() { return siCameraComms.getFixationInfo(); }
    IplImage *getSplicedImg() { return siCameraComms.getSplicedImg(); }
    IplImage *getZoomedImg() { return siCameraComms.getZoomedImg(); }
    void setBgImg( IplImage *bgImg ) { siCameraComms.setBgImg(bgImg); }
    TrackingInfo *getTrackingInfo() { return &trackingInfo; }
    TrackingReply *getTrackingReply() { return &trackingReply; }
   
   /******************************************
    * setFileSaveParams
    * Description: Set the options for saving streams to file
    * Parameters:  prefix - file name prefix
    *              descriptor - file name descriptor
    *              suffix - file name suffix
    *              saveDig - true if saving digital stream
    *              saveOrig - true if saving original stream
    ******************************************
    */          
    void setFileSaveParams(CHAR *prefix, CHAR *descriptor, CHAR *suffix,
                           BOOL saveDig, BOOL saveOrig);
           
  protected:
    virtual void execute(void *);
  private:
   /******************************************
    * processFrame
    * Description: Do all processing that needs to be done for
    *              a new frame.
    * Returns:     False if the video stream has ended. true otherwise.
    ******************************************
    */
    BOOL processFrame();

   /******************************************
    * updateOptions
    * Description: Copies the values of the public options to the
    *              internal options; applies changes to options..
    ******************************************
    */
    void updateOptions();
    
   /******************************************
    * calculateFPS
    * Description: To properly determine the FPS, this should be called before
    *              every frame is captured
    ******************************************
    */
    void calculateFPS();
    
  public:
    //The public version of the various settings used by this object.
    //The settings are not used until applyOptions is called, which copies
    //the values from options to intOptions.
    SISingleCamOptions options;
  protected:
  private:

    //Camera communication//////////////////////////
    //Thread
    SICameraComms siCameraComms;
    
    //Synchronization///////
    //Barrier that makes sure this thread goes only as fast as frames are captured
    Barrier frameBegSynBar;   
    Barrier frameEndSynBar;   
    
    //Semaphore that makes sure the tracking information is ready before trying
    //to fixate
    Semaphore trackingInfoSem;
    
    //Semaphore to make sure that the digital zoom output image is complete before 
    //outputting it.
    Semaphore digitalZoomOutputSem;
  
    
    //Other variables ////////////////////////////
    //True when processing frames
    BOOL threadRunning;
    
    //True when another thread is telling this thread to stop.
    BOOL signalThreadStop;

    //True when an applyOptions call is pending
    BOOL apply;
    
    //True when a reset should occur
    BOOL doReset;
    
    //Signal when the lookup table for optical zoom should be loaded or saved
    BOOL calSignalLoad;
    BOOL calSignalSave;
        
    //Tells the camera whether to do a step or pulse command for pan/tilt/zoom
    CommandType panTiltCommand;
    CommandType zoomCommand;

    //The current frame being processed
    UINT32 frameCounter;

    //The internal version of the various settings used by this object.
    //These are the settings used during execution.
    SISingleCamOptions intOptions;    
    
    //Tracking comms sent to other thread
    TrackingInfo trackingInfo;
    
    //Tracking comms sent from other thread
    TrackingReply trackingReply;
        
    //Imaging Variables///////////////////////////////////////////
    //The destination of images
    VideoDestination *vidDst;

    //The source of images
    VideoSource *vidSrc;
   
    //The image being processed
    //in red/green/blue format
    IplImage *imgOrigRGB;
    
    //The background image in RGB format
    IplImage *imgBgRGB;

    // The video stream for output
    VideoStream videoStream;
        
    //The pointer to the image that will be output
    IplImage *imgOutput;
  
    //////////////////////////////////////////////////
    //The following section is for the FPS calculation
    //////////////////////////////////////////////////
    //The current time
    Time currentTime;
    //The frame counter
    UINT8 fpsCurFrame;
    //The time at which the first frame was read
    timespec fpsStartTime;
    //The time at which the last frame was read
    timespec fpsEndTime;
    //The number of frames between the first and last frame
    UINT8 fpsMaxFrame;
    //The latest fps reading
    FLOAT fps;
    
    //////////////////////////////////////////////////
    // Tracker variables
    //////////////////////////////////////////////////
    Tracker *trkr;
    UINT8 trkrNumROI;
    CvRect trkrROI[MAX_ROIS];
    //The red and blue trackers, respectively
    TrackerColorSegmentation trackerRedColorSegmentation;
    TrackerColorSegmentation trackerBlueColorSegmentation;    
        
    /////////////////////////////////////////////////
    // Measure Final Size Logger
    /////////////////////////////////////////////////
    MeasureFinalSizeLogger mfs;
    Tracker *mfsTrkr;
    UINT8 mfsTrkrNumROI;
    CvRect mfsTrkrROI[MAX_ROIS];
    
    /////////////////////////////////////////////////
    // Video and image I/O
    /////////////////////////////////////////////////
    VideoDestinationScreen videoDestinationScreen;
    VideoDestinationFile videoDestinationFile;
    VideoSourceFrameGrabber videoSourceFrameGrabber;
    VideoSourceFile videoSourceFile;
    
    //Where the config file is located
    CHAR configString[CO_PATH_LENGTH];
    //Barriers for HZO
    Barrier *hzoPreZoomPrePickoffBarrier;
    Barrier *hzoPreZoomPostPickoffBarrier;   
    Barrier *hzoPostZoomPrePickoffBarrier;
    Barrier *hzoPostZoomPostPickoffBarrier;
    
    //Options for saving streams
    //True when the original streams from the camera should be saved
    BOOL vidDstSaveOrigStream;
    BOOL signalVidDstSaveOrigStream;
  
    //True when the digitally altered streams should be saved
    BOOL vidDstSaveDigStream;
    BOOL signalVidDstSaveDigStream;
    
    //The destination to save the streams
    VideoDestinationFileRamDrive vDstFileOrig;
    VideoDestinationFileRamDrive vDstFileDig;
    
};

#endif


// File: $Id: SISingleCam.h,v 1.30 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SISingleCam.h,v $
// Revision 1.30  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.29  2005/09/10 00:35:36  edn2065
// synchronized video streams to file (kind of)
//
// Revision 1.28  2005/09/08 19:07:49  edn2065
// Created RAM drive for file storage
//
// Revision 1.27  2005/09/08 01:02:56  edn2065
// Added ability to save video streams. just a tad slow
//
// Revision 1.26  2005/08/27 22:42:24  edn2065
// Working out CVC
//
// Revision 1.25  2005/08/26 19:04:14  edn2065
// Pickoff points work, but framerate dropped
//
// Revision 1.24  2005/08/26 18:04:35  edn2065
// Added two pickoff points for SIHzo and added better naming convention to barriers
//
// Revision 1.23  2005/08/26 00:57:21  edn2065
// Added menu options for HZO. Setup control arbitration
//
// Revision 1.22  2005/08/26 00:10:08  edn2065
// Named Windows
//
// Revision 1.21  2005/08/25 23:55:59  edn2065
// Removed deadlock in SIHzo and separated SISingleCams from GUI
//
// Revision 1.20  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SISingleCam
//
// Revision 1.19  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.18  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.17  2005/06/28 15:09:40  edn2065
// Added multiple tracker color option to kernel
//
// Revision 1.16  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.15  2005/06/13 14:17:48  edn2065
// Weekend commit
//
// Revision 1.14  2005/06/10 17:03:47  edn2065
// Added MFS
//
// Revision 1.13  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.12  2005/06/10 11:55:51  edn2065
// Fixed reverseLookup error. Added reset option
//
// Revision 1.11  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.10  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
// Revision 1.9  2005/06/03 12:49:46  edn2065
// Changed Controller to SISingleCam
//
// Revision 1.8  2005/05/26 19:15:02  edn2065
// Added VideoStream. tested. Still need to get tracker working
//
// Revision 1.7  2005/05/26 16:09:51  edn2065
// Added error checking to compileAll. And added tracker ability to Controller
//
// Revision 1.6  2005/05/25 16:59:14  edn2065
// Implemented FPS calculation
//
// Revision 1.5  2005/04/27 22:58:30  edn2065
// Added pure static controller to menu
//
// Revision 1.4  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.3  2005/04/06 19:02:06  edn2065
// Added functionality for creation and destruction of input and output video streams.
//
//
// ----------------------------------------------------------------------
//
