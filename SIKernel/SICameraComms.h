#ifndef SICAMERACOMMS_H
#define SICAMERACOMMS_H


#include "types.h"
#include "CameraComms.h"
#include "Thread.h"
#include "Semaphore.h"
#include "Barrier.h"
#include "PanTilt.h"
#include "Zoom.h"
#include "ZoomOptical.h"
#include "ZoomDigital.h"
#include "ZoomHybrid.h"
#include "TrackingInfo.h"
#include "TrackingReply.h"
#include "FixationInfo.h"
#include "SISingleCamOptions.h"
#include "LookupTable.h"
#include "Time.h"
#include "PartialBox.h"
#include "SITordoff.h"

typedef enum
{
  CAL_STATE_IDLE,
  CAL_STATE_GET_SIZE_READING,
  CAL_STATE_GET_POSITION_READING,
  CAL_STATE_PRINCIPAL_POINT_FIXATE,
  CAL_STATE_GET_PRINCIPAL_POINT_READING
}CalibrationState;


typedef struct
{
  //The time at which the p/t/z readings occurred in milliseconds
  UINT32 timestamp;
  //The read pan/til angle, and zoom magnification
  FLOAT panAngle;
  FLOAT tiltAngle;
  FLOAT zoomMag;  
  

} PTZReadings;

//Number of measurements to make when calibrating
#define CAL_NUM_MEASUREMENTS_TO_MAKE 10

/**************************************************************
 *
 * SICameraComms class
 * Description: The thread that deals with camera communication.
 **************************************************************
 */
class SICameraComms : public Thread {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults for all options.
    ******************************************
    */
    SICameraComms();

   /******************************************
    * dtor
    ******************************************
    */
    ~SICameraComms();

   /******************************************
    * init
    * Description: Initializes the synchronization between threads and 
    *              communication.
    * Parameters:  serialPort - The port that the camera is hooked up to
    *              myTrackingInfoSem - Semaphore that is only released when 
    *                               tracking has been completed for a given
    *                               frame.
    *              myDigitalZoomOutputSem - Semaphore that is only released after
    *                               the digital zoom image is ready for a given
    *                               frame.
    *              myFrameSyncBar - Barrier that is met after options are read from
    *                               the UI, and after all processes are completed
    *                               for a given frame.
    *              myExecuteThread - True as long as the frame process continues.
    *              myIntOptions - Communication from the UI thread. 
    *              myTrackingInfo - Communication from calling thread regarding tracking.
    *              myTrackingReply - Communication to calling thread regarding tracking.
    *              digitalZoomImage - The image coming from the zooming camera
    *              digitalZoomBgImage - The image coming from the panoramic camera
    *              vStream - The video stream to register the digital zoom on
    *
    ******************************************
    */
    void init( CHAR *serialPort, Semaphore *myTrackingInfoSem, 
               Barrier *myBegFrameSyncBar, 
               Barrier *myEndFrameSyncBar, BOOL *myExecuteThread,
               SISingleCamOptions *myIntOptions, TrackingInfo *myTrackingInfo, 
               TrackingReply *myTrackingReply, IplImage *digitalZoomImage,
               IplImage *digitalZoomBgImage,      
               Semaphore *myDigitalZoomOutputSem, VideoStream *vStream,
               Barrier *myHzoPreZoomPrePickoffBarrier,
               Barrier *myHzoPreZoomPostPickoffBarrier,   
               Barrier *myHzoPostZoomPrePickoffBarrier,
               Barrier *myHzoPostZoomPostPickoffBarrier );
               
   /******************************************
    * init
    * Description: Initializes the synchronization between threads and 
    *              communication. Leaves digital zoom disabled.
    * Parameters:  Same as above, but without digital zooming semaphore.
    *
    ******************************************    
    */
    void init( CHAR *serialPort, Semaphore *myTrackingInfoSem, 
               Barrier *myFrameSyncBar,  
               Barrier *myEndFrameSyncBar, BOOL *myExecuteThread,
               SISingleCamOptions *myIntOptions, TrackingInfo *myTrackingInfo, 
               TrackingReply *myTrackingReply );    
               
   /******************************************
    * getZoomDigital
    * Description: Accessor of digital zoom object
    * Returns:     Pointer to digital zoom object
    ******************************************    
    */                                
    ZoomDigital *getZoomDigital();

   /******************************************
    * getZoomOptical
    * Description: Accessor of optical zoom object
    * Returns:     Pointer to optical zoom object
    ******************************************    
    */                                
    ZoomOptical *getZoomOptical();   
    
   /******************************************
    * getPixelsPerDegreeTable
    * Description: Accessor of pixels per degree object
    * Returns:     Pointer to pixels per degree object
    ******************************************    
    */                                
    LookupTable *getPixelsPerDegreeTable();       
    
   /******************************************
    * startCalibration
    * Description: Signals the thread to start calibrating zoom. Threadsafe.
    ******************************************    
    */                         
    void startCalibration();

   /******************************************
    * stopCalibration
    * Description: Signals the thread to stop calibrating zoom. Threadsafe.
    ******************************************    
    */                                 
    void stopCalibration();
   
   /******************************************
    * applyOptions
    * Description: Notifies this object that new options have been applied. 
    *              Threadsafe.
    ******************************************    
    */                     
    void applyOptions();
    
    FixationInfo *getFixationInfo() { return &fixationInfo; }
    IplImage *getSplicedImg() { return zoomDigital.getSplicedImg(); }
    IplImage *getZoomedImg() { return zoomDigital.getZoomedImg(); }
    void setBgImg( IplImage *bgImg ) { zoomDigital.setBgImage(bgImg); }
    
        
  protected:
    virtual void execute(void *);
  private:
    
   /******************************************
    * makeFixationDecisions
    * Description: Takes the tracking information and converts it to what commands
    *              should be sent to the camera.
    *
    ******************************************    
    */    
    void makeFixationDecisions();
    
   /******************************************
    * sendFixationCommands
    * Description: Sends fixation commands to the camera.
    *
    ******************************************    
    */        
    void sendFixationCommands();
    
   /******************************************
    * sendDigitalZoomCommands
    * Description: Sends digital zooming command to zoom object.
    *
    ******************************************    
    */        
    void sendDigitalZoomCommands(); 
        
   /******************************************
    * enterCalStateSizeReading
    * Description: Executes entry actions to the CAL_STATE_GET_SIZE_READING
    *              state; moves P/T to center and zooms out.
    ******************************************    
    */            
    void enterCalStateGetSizeReading();
    
   /******************************************
    * fixateToPoint
    * Description: Moves the Pan/Tilt platform such that the 
    *              tracked object will move to (xPoint,yPoint)
    *              in the image.
    ******************************************    
    */                
    void fixateToPoint( FLOAT xPoint, FLOAT yPoint );
        
   /******************************************
    * getPTZForThisFrame
    * Description: Calculate the pan, tilt, and zoom readings that
    *              correspond to the most recently grabbed frame.
    *              This account for delays in retrieving the readings
    *              and in retrieving the image.
    * Parameters:  myPanAngle  - returned as the pan angle corresponding
    *                          to the most recent frame
    *              myTiltAngle - returned as the tilt angle corresponding
    *                          to the most recent frame
    *              myZoomMag - returned as the zoom magnification corresponding
    *                          to the most recent frame
    * Returns:     True if the returned values are legal.
    ******************************************    
    */      
    BOOL getPTZForThisFrame( FLOAT *myPanAngle, FLOAT *myTiltAngle, 
      FLOAT *myZoomMag );
   
   /******************************************
    * setPTZReading
    * Description: Save the pan, tilt, and zoom readings that
    *              correspond to the most recently grabbed frame.
    *              This account for delays in retrieving the readings
    *              and in retrieving the image.
    * Parameters:  myPanAngle  - returned as the pan angle corresponding
    *                          to the most recent frame
    *              myTiltAngle - returned as the tilt angle corresponding
    *                          to the most recent frame
    *              myZoomMag - returned as the zoom magnification corresponding
    *                          to the most recent frame
    ******************************************    
    */         
    void setPTZReading( FLOAT myPanAngle, FLOAT myTiltAngle, 
      FLOAT myZoomMag );
    
  private:
 
    //////////////////////////////////////////
    //Camera communication
    //////////////////////////////////////////
    CameraComms cameraComms;
    CameraRxThread cameraRxThread;
    CameraTxThread cameraTxThread;
    UINT8 cameraRxTxSockets;
    Semaphore cameraRxTxSocketsSem;
    Semaphore cameraRxTxClearToSendSem;
    
    //Camera control
    PanTilt panTilt;
    ZoomOptical zoomOptical;
    ZoomDigital zoomDigital;
    ZoomHybrid  zoomHybrid;
        
    //The zoom being used
    Zoom *zoom;
    
    //Says what angles/magnifications to send to the camera
    FixationInfo fixationInfo;
    
    //Synchronization and thread communication//////////////////////////
    //Barrier that makes sure this thread goes only as fast as frames are captured
    Barrier *frameBegSynBar;   
    Barrier *frameEndSynBar;   
    
    //Semaphore that makes sure the tracking information is ready before trying
    //to fixate
    Semaphore *trackingInfoSem;
    
    //Semaphore to make sure that the digital zoom output image is complete before 
    //outputting it.
    Semaphore *digitalZoomOutputSem;
    
    //Semaphores to make sure inquiries have finished
    Semaphore panTiltInquirySem;
    Semaphore zoomInquirySem;
    
    //True as long as process should be repeated
    BOOL *executeThread;        
    
    //The options from the UI
    SISingleCamOptions *intOptions;
    
    //Tracking comms sent from other thread
    TrackingInfo *trackingInfo;
    
    //Tracking comms sent to other thread
    TrackingReply *trackingReply;
    
    //True when applying new options.
    BOOL apply;
       
    //////////////////////////////////////////
    //P/T/Z Pulsing
    //////////////////////////////////////////
    //True when a pulse is taking place
    BOOL zoomPulsing;
    //True when the motor is going in the forward direction
    BOOL zoomPulsingForward;
    //The number of pulses left
    UINT32 zoomPulsingIterationsLeft;
    //The number of frames left before the end of this pulse
    UINT32 zoomPulsingFramesLeftThisIteration;
    
    //True when a pulse is taking place
    BOOL panTiltPulsing;
    //True when the motor is going in the forward direction
    BOOL panTiltPulsingForward;
    //The number of pulses left
    UINT32 panTiltPulsingIterationsLeft;
    //The number of frames left before the end of this pulse
    UINT32 panTiltPulsingFramesLeftThisIteration;
    
    //////////////////////////////////////////
    //Calibration of zoom lens readings
    //////////////////////////////////////////
    //The table used for temporarily saving zoom reading values 
    FLOAT calZoomReadingTable[LKP_DEFAULT_LENGTH];
    //The table used for temporarily saving pixels per degree values 
    FLOAT calPixelsPerDegreeTable[LKP_DEFAULT_LENGTH];
    //True when calibrating
    CalibrationState calibrationState;
    //True when calibration should start
    BOOL   calSignalStart;
    //True when calibration should stop
    BOOL   calSignalStop;
    //The next zoom reading being sought
    UINT16 calNextZoomReading;
    //The number of the current iteration for this calibration
    UINT16 calIterationNumber;
    //The total number of iterations for this calibration
    UINT16 calTotalIterations;    
    //The area of the tracked object in the most zoomed out frame
    UINT32 calInitialArea;
    //The position of the target object when P/T to (0,0)
    FLOAT  calXPositionAtPTOrigin;
    //The number of measurements made in this state. Will not leave the state
    //until a certain number of measurements have been made.
    UINT16 calMeasurementsMadeThisState;
    //Maps the magnification of the zoom lens to pixels per degree 
    //of pan/tilt of the camera
    LookupTable zoomMagToPixelsPerDegreeTable;
    //The box used to find the principal point
    PartialBox calPrinPtBox;
    //The "sub" box centered around the center of the image
    CvRect calSubBox;
    
    //////////////////////////////////////////
    //Handling of delays in Pan/Tilt/Zoom readings
    //////////////////////////////////////////
    //The current time
    Time currentTime;
    timespec timestamp;
    //The most recent pan/tilt/zoom readings from the camera
    PTZReadings ptzReadingsHistory[PTZ_HISTORY_SIZE];
    //The index of the most recent single pan/tilt/zoom reading
    UINT16 ptzCurrentReading;     
    //Only true once all circular buffers have been filled with valid data
    BOOL ptzBufferFilled;      
    //True if an error inquiring pan/tilt
    BOOL errorWithPanTiltInquiry;    
    //True if an error inquiring zoom
    BOOL errorWithZoomInquiry;  
    
    FLOAT savedPanAngle;
    FLOAT savedTiltAngle;
    FLOAT savedZoomMag;
    FLOAT savedPvsZoomMag;
    
    ////////////////////////////////////////////
    // Control algorithm
    ////////////////////////////////////////////
    SITordoff siTordoff;
    FLOAT panAngleAtLastCapture;
    FLOAT tiltAngleAtLastCapture;
    FLOAT zoomMagAtLastCapture;
    FLOAT measuredAreaAtLastCapture;
    FLOAT estimatedAreaAtLastCommand;
    SIKalman areaEstimator;
    
    ///////////////////////////////////////////
    //Hybrid Zoom out synchronization
    ///////////////////////////////////////////
    Barrier *hzoPreZoomPrePickoffBarrier;
    Barrier *hzoPreZoomPostPickoffBarrier;   
    Barrier *hzoPostZoomPrePickoffBarrier;
    Barrier *hzoPostZoomPostPickoffBarrier;
    
};

#endif


// File: $Id: SICameraComms.h,v 1.32 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SICameraComms.h,v $
// Revision 1.32  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.31  2005/09/06 01:57:12  edn2065
// Added zoom delay handling
//
// Revision 1.30  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.29  2005/08/26 19:04:14  edn2065
// Pickoff points work, but framerate dropped
//
// Revision 1.28  2005/08/26 18:04:35  edn2065
// Added two pickoff points for SIHzo and added better naming convention to barriers
//
// Revision 1.27  2005/08/26 00:57:21  edn2065
// Added menu options for HZO. Setup control arbitration
//
// Revision 1.26  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SISingleCam
//
// Revision 1.25  2005/08/05 23:08:09  edn2065
// Made kalman filter for zooming
//
// Revision 1.24  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.23  2005/07/28 20:57:49  edn2065
// Kalman filter working
//
// Revision 1.22  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.21  2005/07/26 18:46:43  edn2065
// Added scale invariance
//
// Revision 1.20  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.19  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.18  2005/07/14 20:16:00  edn2065
// Added commenting to RxTxThread
//
// Revision 1.17  2005/06/30 14:46:28  edn2065
// overnight checkin
//
// Revision 1.16  2005/06/28 20:01:49  edn2065
// Debugging comms bugs. overnight check in
//
// Revision 1.15  2005/06/21 14:56:15  edn2065
// Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
//
// Revision 1.14  2005/06/21 14:09:07  edn2065
// Added PartialBox principal point algorithm to kernel. Early tests work.
//
// Revision 1.13  2005/06/21 11:13:04  edn2065
// Added delay handling for P/T/Z.
//
// Revision 1.12  2005/06/17 18:35:51  edn2065
// Added verbosity to pulsing
//
// Revision 1.11  2005/06/14 18:22:40  edn2065
// Implemented saving for calibration
//
// Revision 1.10  2005/06/14 16:06:41  edn2065
// Extended calibration algorithm
//
// Revision 1.9  2005/06/13 21:04:19  edn2065
// Implemented fixating. Needs to be fine tuned
//
// Revision 1.8  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.7  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.5  2005/06/09 20:51:22  edn2065
// Added pulsing to zoom and pt
//
// Revision 1.4  2005/06/09 18:47:40  edn2065
// Implemented Hybrid zoom
//
// Revision 1.3  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.2  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
// Revision 1.1  2005/06/07 10:44:46  edn2065
// Added to repository
//
//
