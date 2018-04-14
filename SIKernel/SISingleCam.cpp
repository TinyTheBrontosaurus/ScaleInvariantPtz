#include <stdio.h>
#include <unistd.h>
#include "SISingleCam.h"

/*****************************************************************
 * SISingleCam ctor
 *****************************************************************
 */

SISingleCam::SISingleCam( Barrier *myHzoPreZoomPrePickoffBarrier,
              Barrier *myHzoPreZoomPostPickoffBarrier,   
              Barrier *myHzoPostZoomPrePickoffBarrier,
              Barrier *myHzoPostZoomPostPickoffBarrier,
             CHAR *myTitleString, CHAR *myConfigString ):
  Thread(),
  videoDestinationScreen( myTitleString )
{
  sprintf( configString, "%s%s",
    "../../config/", myConfigString );
  hzoPreZoomPrePickoffBarrier = myHzoPreZoomPrePickoffBarrier;
  hzoPreZoomPostPickoffBarrier = myHzoPreZoomPostPickoffBarrier;   
  hzoPostZoomPrePickoffBarrier = myHzoPostZoomPrePickoffBarrier;
  hzoPostZoomPostPickoffBarrier = myHzoPostZoomPostPickoffBarrier;

  signalVidDstSaveOrigStream = false;
  signalVidDstSaveDigStream  = false;
  vidDstSaveOrigStream = false;
  vidDstSaveDigStream = false;
  
  //Start the thread
  start(0);

}

/*****************************************************************
 * SISingleCam dtor
 *****************************************************************
 */
SISingleCam::~SISingleCam() {

  //Stop execution
  signalThreadStop = true;
  
  //Save options
  options.saveOptions(configString);
  
  join( 0 );

  //Stop the MFS logging
  mfs.endFile();

  //Delete allocated memory
  cvReleaseImage( &imgOrigRGB );
  cvReleaseImage( &imgBgRGB );

}

/*****************************************************************
 * SISingleCam::execute
 *****************************************************************
 */
void SISingleCam::execute( void *a ) {

  //Set defaults
  apply = true;  //because loading the defaults file

  //Count frames
  frameCounter = 0;

  //Allocate images
  imgOrigRGB = NEW_IMAGE;
  imgBgRGB = NEW_IMAGE;

  //Set the default to be output
  imgOutput = imgOrigRGB;

  //Set pointers to null
  vidSrc = 0;
  vidDst = 0;

  //Initialize the FPS calculation variables.
  fpsCurFrame = 0;
  fpsMaxFrame = 15;
  fps = 0.0;

  //Load defaults
  options.loadOptions(configString);
  intOptions.copy( &options );

  //Initialize tracking
  trkrNumROI = 0;
  trackerRedColorSegmentation.train("/home/edn2065/SI/trk/red.bmp");
  trackerBlueColorSegmentation.train("/home/edn2065/SI/trk/blue.bmp");

  //Register
  videoStream.registerVideo( imgOrigRGB, "Original image");
  trackerRedColorSegmentation.registerVideo( &videoStream, "Red " );
  trackerBlueColorSegmentation.registerVideo( &videoStream, "Blue " );

  //Set up camera communications thread
  siCameraComms.init( intOptions.serialPort, &trackingInfoSem,
               &frameBegSynBar, &frameEndSynBar, &threadRunning, &intOptions,
               &trackingInfo, &trackingReply,
               imgOrigRGB, imgBgRGB, &digitalZoomOutputSem, &videoStream,
               hzoPreZoomPrePickoffBarrier,
               hzoPreZoomPostPickoffBarrier,   
               hzoPostZoomPrePickoffBarrier,
               hzoPostZoomPostPickoffBarrier );

  //Let the barrier know there are three threads waiting on it.
  frameBegSynBar.init( 3 );
  frameEndSynBar.init( 3 );

  //Default to no commands
  panTiltCommand = CMD_NONE;
  zoomCommand = CMD_NONE;

  //Initialize stop signal
  signalThreadStop = false;

  //Initialize the calibration signals
  calSignalSave = false;
  calSignalLoad = false;

  //Get the default calibration
  loadZoomCalibrationTable();
    
  //Start execution
  threadRunning = true;

  siCameraComms.start(0);

  //Keep process running as long as thread is running
  while( threadRunning )
  {

    //Update the options
    updateOptions();

    if( threadRunning )
    {
      //Make sure there is a source. A destination is not necessarily required
      if( vidSrc )
      {
        //Process the frame as long as the source stream is alive.
        if( !processFrame() )
        {
          //End of stream, so get rid of it.
          vidSrc = 0;
        }

      }
      //Idle
      else
      {
        usleep(30000);
      }
    }
    else
    {
      //Signal that it's time to quit
      trackingInfo.badStream = true;
      //Meet at the barrier
      frameBegSynBar.wait();
    }


    //Increment frame
    frameCounter++;
  }

}

/*****************************************************************
 * SISingleCam::processFrame
 *****************************************************************
 */
 BOOL SISingleCam::processFrame()
{
  //Whether the video stream has reach EOF
  BOOL streamAlive;
  //The Pan/Tilt/Zoom inquiry
  CHAR panTiltZoomOutput[100];

  //Grab the tracking information semaphore
  trackingInfoSem.acquire();

  //Wait for Comms thread to be ready
  frameBegSynBar.wait();

  //Calculate the framerate
  if( intOptions.showFPS )
  {
    //Calculate the FPS
    calculateFPS();
  }

  //Get the newest frame
  streamAlive = vidSrc->getFrame( imgOrigRGB );

  // Check for end of stream
  if( streamAlive )
  {
    //Indicate that the stream is good
    trackingInfo.badStream = false;

    //Run tracker
    trackingInfo.objectFound = false;
    if( intOptions.track )
    {
      trkr->track( imgOrigRGB, &trkrNumROI, trkrROI );

      //Make sure an object was found, and save information on the biggest one
      if( trkrNumROI >= 1 )
      {
        //Indicate that the object was found
        trackingInfo.objectFound = true;
        trackingInfo.width = trkrROI[0].width;
        trackingInfo.height = trkrROI[0].height;
        //See 6/10/05 p. 4 notes for equation derivations
        trackingInfo.xPos = trkrROI[0].width / 2 + trkrROI[0].x - IM_WIDTH/2;
        trackingInfo.yPos = trkrROI[0].height / 2 + trkrROI[0].y - IM_HEIGHT/2;
        //Also send a pointer to the ROI
        trackingInfo.roi = &trkrROI[0];
      }
    }

    //Release the tracking information semaphore
    trackingInfoSem.release();

    //Display tracking boxes
    if( intOptions.showTrackingBoxes )
    {
      vidDst->setBoxes( trkrROI, E_MIN(trkrNumROI, intOptions.numTrackingBoxes) );
    }

    //Set the output frame ////
    //Show the FPS calculation
    if( intOptions.showFPS )
    {
      //The output it
      CHAR text[20];
      sprintf(text, "%2.1lf fps", fps);
      vidDst->setText( text );
    }

    //Set the output image pointer
    vidDst->setImg(imgOutput);    

    //Acquire the digital zoom output semaphore
    digitalZoomOutputSem.acquire();
    
    //Display that frame
    vidDst->sendFrame();
    
    //Save the frame to file
    if( vidDstSaveOrigStream )
    {
      vDstFileOrig.setImg(imgOrigRGB);
      vDstFileOrig.sendFrame();
    }
    if( vidDstSaveDigStream )
    {
      vDstFileDig.setImg(siCameraComms.getZoomedImg());
      vDstFileDig.sendFrame();
    }
       
    //Do MFS
    if( intOptions.mfs )
    {
      CvRect *mfsBestROI = &mfsTrkrROI[0];
      //Track the image
      mfsTrkr->track( imgOutput, &mfsTrkrNumROI, mfsTrkrROI );

      if( mfsTrkrNumROI >= 1 )
      {
        //Log the results. the x- and y- positions are w.r.t the center
        //of the image
        mfs.log( mfsBestROI->width, mfsBestROI->height,
          mfsBestROI->width / 2 + mfsBestROI->x - IM_WIDTH/2,
          mfsBestROI->height / 2 + mfsBestROI->y - IM_HEIGHT/2,
          trackingReply.panAngle, trackingReply.tiltAngle,
          trackingReply.zoomMagOptical, trackingReply.zoomMagOpticalRaw,
          trackingReply.zoomMagDigital );
      }
      else
      {
        //Log an error
        mfs.noLog();
      }
    }

    //Release the digital zoom output semaphore
    digitalZoomOutputSem.release();

  }
  else
  {
    //Indicate that the stream has ended
    trackingInfo.badStream = true;

    //Release the tracking info semaphore
    trackingInfoSem.release();
  }

  //Wait for Comms thread to catch up
  frameEndSynBar.wait();
    
  return streamAlive;
}

/*****************************************************************
 * SISingleCam::applyOptions
 *****************************************************************
 */
 void SISingleCam::applyOptions()
{
  apply = true;
}

/*****************************************************************
 * SISingleCam::applyOptions
 *****************************************************************
 */
 void SISingleCam::reset()
{
  doReset = true;
}

/*****************************************************************
 * SISingleCam::updateOptions
 *****************************************************************
 */
void SISingleCam::updateOptions()
{

  //Propagate UI signals for pulse/steps
  trackingInfo.panTiltCommand = panTiltCommand;
  panTiltCommand = CMD_NONE;
  trackingInfo.zoomCommand = zoomCommand;
  zoomCommand = CMD_NONE;
  threadRunning = !signalThreadStop;
  
  //ACKnowledge stream saving signals
  if( signalVidDstSaveOrigStream )
  {
    vidDstSaveOrigStream = signalVidDstSaveOrigStream;
    signalVidDstSaveOrigStream = false;
  }
  
  if( signalVidDstSaveDigStream )
  {
    vidDstSaveDigStream = signalVidDstSaveDigStream;
    signalVidDstSaveDigStream  = false;
  }

  //Check reset
  if( doReset )
  {
    //Signal the camera thread to reset
    trackingInfo.reset = true;

    //Note that setting options directly here causes a race condition with
    // the public options. Hopefully the operator will be too busy pressing reset
    // to change something else before the reset is complete.

    //Set to screen and frame grabber
    options.vidDst = DST_SCREEN;
    options.vidSrc = SRC_FRAME_GRABBER;

    //Turn off MFS
    options.mfs = false;

    //Turn off fixation
    options.fixate = false;
    options.scaleInv = false;
    options.fixateDigital = false;

    //Turn off the inquiries
    options.zoomInquiry = true;
    options.panInquiry = true;
    options.tiltInquiry = true;

    siCameraComms.stopCalibration();

    vidDstSaveOrigStream = false;
    vidDstSaveDigStream = false;
    
    //Apply these changes
    apply=true;

    //Reset the signal
    doReset = false;
  }

  //Only update the options if the apply flag has been set
  if( apply )
  {
    printf("Applying changes...\n");

    //Clear flag
    apply = false;

    //Now copy the options over
    intOptions.copy( &options);

    //Set the preventLine value
    videoDestinationScreen.setPreventLine(intOptions.vidDstPreventLine);

    //Make a new destination
    switch( intOptions.vidDst )
    {

      case DST_SCREEN:
      {
        printf("vidDst set to SCREEN.\n");
        //Set the destination to a screen
        vidDst = &videoDestinationScreen;
        break;
      }

      case DST_FILE:
      {
        printf("vidDst set to FILE.\n");
        //Set the path
        CHAR pathStr[CO_PATH_LENGTH];
        sprintf( pathStr, "%s%s", options.vidDstDirectory,
          options.vidDstPrefix );
        videoDestinationFile.setFile(pathStr, options.vidDstSuffix);
         //Set the destination to a file
        vidDst = &videoDestinationFile;
        break;
      }

      case DST_NONE:
      {
        printf("vidDst set to NONE.\n");
        //Set the destination to nothing
        vidDst = 0;
        break;
      }

      default:
        printf( "Invalid video destination selected. %s:%d" CRLF,
          __FILE__, __LINE__ );
        break;
    }

     //Set the new source
     switch( intOptions.vidSrc )
     {
       case SRC_FRAME_GRABBER:
       {
         printf("vidSrc set to FRAME_GRABBER.\n");
         //Set the path
         CHAR pathStr[CO_PATH_LENGTH];
         sprintf( pathStr, "%s%s", intOptions.vidSrcDirectory, intOptions.vidSrcFile );
         printf( "Frame grabber: %s\n", pathStr );
         videoSourceFrameGrabber.setPath(pathStr);
         vidSrc = &videoSourceFrameGrabber;
         break;
       }

       case SRC_FILE:
       {
         printf("vidSrc set to FILE.\n");
         //Set the path
         videoSourceFile.setFile(intOptions.vidSrcPrefix,
           intOptions.vidSrcSuffix, intOptions.vidSrcNumDigits,
           intOptions.vidSrcStart, intOptions.vidSrcLoop);
         vidSrc = &videoSourceFile;
         break;
       }

       case SRC_NONE:
         printf("vidSrc set to NONE.\n");
         //Nothing
         vidSrc = 0;
         break;

       default:
         printf( "Invalid video source selected.\n");
         break;
     }

     //Set the tracker
     switch( intOptions.trackingTrkr )
     {
       case TRKR_CSEG:
         //Find out which color to segment
         switch( intOptions.trackingTrkrColor )
         {
           case TRKR_COLOR_RED:
             trkr = &trackerRedColorSegmentation;
             break;
           case TRKR_COLOR_BLUE:
             trkr = &trackerBlueColorSegmentation;
             break;
           default:
             printf( "Illegal color of tracker (%s @ %d)"CRLF,
               __FILE__, __LINE__ );
             break;
         }
         break;
       case TRKR_CAMSHIFT:
         printf( "CAMSHIFT tracking algorithm not yet implemented." CRLF );
       case TRKR_NONE:
         trkr = 0;
         break;
     }

    //Set the output
    imgOutput = videoStream.getStream( intOptions.videoStreamOutput );

    //Clear FPS calculation if applicable
    if( !intOptions.showFPS )
    {
      vidDst->setText("");
    }

    //Clear boxes if applicable
    if( !intOptions.showTrackingBoxes )
    {
      vidDst->setBoxes( trkrROI, 0 );
    }

    //Set the splicing option
    siCameraComms.getZoomDigital()->setSpliceOutput(intOptions.spliceZoomOut);

    //Tell camera comms of new options
    siCameraComms.applyOptions();

    //Setup MFS
    if( intOptions.mfs )
    {
      CHAR mfsFileName[CO_PATH_LENGTH];

      //Set the tracker
      switch( intOptions.mfsTrkr )
      {
        case TRKR_CSEG:
          //Find out which color to segment
          switch( intOptions.mfsTrkrColor )
          {
            case TRKR_COLOR_RED:
              mfsTrkr = &trackerRedColorSegmentation;
              break;
            case TRKR_COLOR_BLUE:
              mfsTrkr = &trackerBlueColorSegmentation;
              break;
            default:
              printf( "Illegal color of MFS tracker (%s @ %d)"CRLF,
                __FILE__, __LINE__ );
              break;
          }

          //Set up the MFS file name and open the file
          sprintf( mfsFileName, "%s%s", intOptions.mfsDirectory, intOptions.mfsFile );
          mfs.startFile( mfsFileName );
          break;
        case TRKR_CAMSHIFT:
          printf( "CAMSHIFT tracking algorithm not yet implemented." CRLF );
        case TRKR_NONE:
          mfsTrkr = 0;
          break;
      }
    }
    else
    {
      //Otherwise close the file, even if not open
      mfs.endFile();
    }
  }

  //See if there was a signal to load or save the calibration table
  if( calSignalSave || calSignalLoad)
  {
    //Setup the file names for loading/saving
    CHAR fileNamePrefix[CO_PATH_LENGTH];
    CHAR fileNameRdgToMag[CO_PATH_LENGTH];
    CHAR fileNamePixelsPerDegree[CO_PATH_LENGTH];
    CHAR fileNamePrincipalPoint[CO_PATH_LENGTH];

    sprintf( fileNamePrefix, "%s%s", intOptions.calDirectory,
      intOptions.calFile);
    sprintf( fileNameRdgToMag, "%s.%s", fileNamePrefix,
      "r2m");
    sprintf( fileNamePixelsPerDegree, "%s.%s", fileNamePrefix,
      "ppd");
    sprintf( fileNamePrincipalPoint, "%s.%s", fileNamePrefix,
      "pp");

    if( calSignalSave )
    {
      //Clear flag
      calSignalSave = false;
      siCameraComms.getZoomOptical()->saveLookup(fileNameRdgToMag);
      siCameraComms.getPixelsPerDegreeTable()->save(fileNamePixelsPerDegree);
      siCameraComms.getZoomDigital()->savePrincipalPoint(fileNamePrincipalPoint);
    }
    else
    {
      //Clear flag
      calSignalLoad = false;
      siCameraComms.getZoomOptical()->setLookup(fileNameRdgToMag);
      siCameraComms.getPixelsPerDegreeTable()->load(fileNamePixelsPerDegree);
      siCameraComms.getZoomDigital()->loadPrincipalPoint(fileNamePrincipalPoint);
    }

  }
}

/*****************************************************************
 * SISingleCam::calculateFPS
 *****************************************************************
 */
void SISingleCam::calculateFPS() {

  if(fpsCurFrame == 0) {
    currentTime.getClock(&fpsStartTime);
    fpsCurFrame++;
  }
  else if(fpsCurFrame >= fpsMaxFrame) {
    currentTime.getClock(&fpsEndTime);
    fps = fpsMaxFrame / currentTime.getTimeDiff(fpsStartTime, fpsEndTime);
    fpsCurFrame = 0;
  }
  else {
    fpsCurFrame++;
  }
}

/*****************************************************************
 * SISingleCam::panStep
 *****************************************************************
 */
void SISingleCam::panTiltStep()
{
  panTiltCommand = CMD_STEP;
}

/*****************************************************************
 * SISingleCam::panPulse
 *****************************************************************
 */
void SISingleCam::panTiltPulse()
{
  panTiltCommand = CMD_PULSE;
}

/*****************************************************************
 * SISingleCam::zoomStep
 *****************************************************************
 */
void SISingleCam::zoomStep()
{
  zoomCommand = CMD_STEP;
}

/*****************************************************************
 * SISingleCam::zoomPulse
 *****************************************************************
 */
void SISingleCam::zoomPulse()
{
  zoomCommand = CMD_PULSE;
}

/*****************************************************************
 * SISingleCam::saveZoomCalibrationTable
 *****************************************************************
 */
void SISingleCam::saveZoomCalibrationTable()
{
  calSignalSave = true;
}

/*****************************************************************
 * SISingleCam::loadZoomCalibrationTable
 *****************************************************************
 */
void SISingleCam::loadZoomCalibrationTable()
{
  calSignalLoad = true;
}

/*****************************************************************
 * SISingleCam::startCalibration
 *****************************************************************
 */
void SISingleCam::startCalibration()
{
  siCameraComms.startCalibration();
}

/*****************************************************************
 * SISingleCam::stopCalibration
 *****************************************************************
 */
void SISingleCam::stopCalibration()
{
  siCameraComms.stopCalibration();
}

/*****************************************************************
 * SISingleCam::setFileSaveParams
 *****************************************************************
 */
void SISingleCam::setFileSaveParams(CHAR *prefix, CHAR *descriptor, CHAR *suffix, 
                       BOOL saveDig, BOOL saveOrig)
{
  CHAR pathStr[CO_PATH_LENGTH];
  
  //Save the inputs
  signalVidDstSaveDigStream = saveDig;
  signalVidDstSaveOrigStream = saveOrig;
  
  //Save the strings
  if( signalVidDstSaveDigStream )
  {
    //Set the path
    CHAR pathStr[CO_PATH_LENGTH];
    sprintf( pathStr, "%s%sDigi", prefix, descriptor);
    vDstFileDig.setFile(pathStr, suffix);
  }
  
  //Save the strings
  if( signalVidDstSaveOrigStream )
  {
    //Set the path
    CHAR pathStr[CO_PATH_LENGTH];
    sprintf( pathStr, "%s%sOrig", prefix, descriptor);
    vDstFileOrig.setFile(pathStr, suffix);
  }
}

// File: $Id: SISingleCam.cpp,v 1.67 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SISingleCam.cpp,v $
// Revision 1.67  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.66  2005/09/10 00:35:36  edn2065
// synchronized video streams to file (kind of)
//
// Revision 1.65  2005/09/08 23:07:42  edn2065
// Fixed saving wrong stream bug
//
// Revision 1.64  2005/09/08 01:02:56  edn2065
// Added ability to save video streams. just a tad slow
//
// Revision 1.63  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.62  2005/08/26 18:04:35  edn2065
// Added two pickoff points for SIHzo and added better naming convention to barriers
//
// Revision 1.61  2005/08/26 17:10:05  edn2065
// Added 2nd HZO pickoff pt
//
// Revision 1.60  2005/08/26 00:10:08  edn2065
// Named Windows
//
// Revision 1.58  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SISingleCam
//
// Revision 1.57  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.56  2005/08/24 15:38:46  edn2065
// Committing before HZO
//
// Revision 1.55  2005/08/18 03:10:55  edn2065
// Fixed shutdown config save to relative directory
//
// Revision 1.54  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.53  2005/08/10 02:48:03  edn2065
// Changed to allow warning and error free ICC compile
//
// Revision 1.52  2005/08/09 23:43:46  edn2065
// removed extra debug statement
//
// Revision 1.51  2005/08/09 21:59:27  edn2065
// Added easy serialPort switching via SISingleCamOptions files
//
// Revision 1.50  2005/08/03 01:43:26  edn2065
// Implemented focal length selection
//
// Revision 1.49  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.48  2005/07/26 18:46:43  edn2065
// Added scale invariance
//
// Revision 1.47  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.46  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.45  2005/07/13 23:54:49  edn2065
// Power off save
//
// Revision 1.44  2005/06/28 20:01:49  edn2065
// Debugging comms bugs. overnight check in
//
// Revision 1.43  2005/06/28 15:09:40  edn2065
// Added multiple tracker color option to kernel
//
// Revision 1.42  2005/06/21 15:50:48  edn2065
// Fixed MFS bug where outputted the main trackers output instead of its own
//
// Revision 1.41  2005/06/21 14:09:07  edn2065
// Added PartialBox principal point algorithm to kernel. Early tests work.
//
// Revision 1.40  2005/06/21 11:13:04  edn2065
// Added delay handling for P/T/Z.
//
// Revision 1.39  2005/06/17 18:35:51  edn2065
// Added verbosity to pulsing
//
// Revision 1.38  2005/06/14 18:22:40  edn2065
// Implemented saving for calibration
//
// Revision 1.37  2005/06/14 13:13:07  edn2065
// Added removal of inquiry to reset
//
// Revision 1.36  2005/06/14 12:19:58  edn2065
// Made PT inquiry a float instead of int. Camera resolution was being thrown away
//
// Revision 1.35  2005/06/13 21:04:19  edn2065
// Implemented fixating. Needs to be fine tuned
//
// Revision 1.34  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.33  2005/06/13 14:17:48  edn2065
// Weekend commit
//
// Revision 1.32  2005/06/10 17:03:47  edn2065
// Added MFS
//
// Revision 1.31  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.30  2005/06/10 11:55:51  edn2065
// Fixed reverseLookup error. Added reset option
//
// Revision 1.29  2005/06/09 18:47:40  edn2065
// Implemented Hybrid zoom
//
// Revision 1.28  2005/06/09 14:03:04  edn2065
// Added digital and raw optical to zoom inquiry output
//
// Revision 1.27  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.26  2005/06/08 15:08:00  edn2065
// Add P/T/Z inquiry
//
// Revision 1.25  2005/06/08 13:44:18  edn2065
// Overnight checkin
//
// Revision 1.24  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
// Revision 1.23  2005/06/03 12:49:46  edn2065
// Changed Controller to SISingleCam
//
// Revision 1.22  2005/05/27 14:57:27  edn2065
// Fix VideoSource memory leak and possible OBOE in CSEG
//
// Revision 1.21  2005/05/27 13:20:53  edn2065
// Added ability to change number of ROIs in output
//
// Revision 1.20  2005/05/26 20:14:59  edn2065
// Fixed saveOptions bug
//
// Revision 1.19  2005/05/26 19:15:02  edn2065
// Added VideoStream. tested. Still need to get tracker working
//
// Revision 1.18  2005/05/26 17:47:08  edn2065
// Added VideoStream to compileAll
//
// Revision 1.17  2005/05/26 16:09:51  edn2065
// Added error checking to compileAll. And added tracker ability to Controller
//
// Revision 1.16  2005/05/25 16:59:14  edn2065
// Implemented FPS calculation
//
// Revision 1.15  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
//
// Revision 1.14  2005/05/25 14:45:36  edn2065
// Changed Thread to a pthread implementation
//
// Revision 1.13  2005/05/24 18:01:05  edn2065
// Playing with wxThreads. Trying to get them to wait() correctly.
//
// Revision 1.12  2005/05/24 15:36:37  edn2065
//
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.10  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.9  2005/04/21 13:47:39  edn2065
// Added Menus and fully commented. commented ControllerOptions.
//
// Revision 1.8  2005/04/07 16:43:30  edn2065
// Changed to wxThread
//
// Revision 1.6  2005/04/07 14:28:36  edn2065
// Implemented tester. untested tester.
//
// Revision 1.5  2005/04/06 21:21:09  edn2065
// Have number of bugs with threads. Revamping GUI to VideoSourceFile call
//
// Revision 1.4  2005/04/06 19:02:06  edn2065
// Added functionality for creation and destruction of input and output video streams.
//
// Revision 1.3  2005/04/06 15:40:40  edn2065
// Revamped Makefile for ../shared
//
// Revision 1.2  2005/04/06 15:04:06  edn2065
// Added ControllerTester with main() and Makefile accordingly.
//
// Revision 1.1  2005/04/06 14:35:36  edn2065
// Adding files to repos.
//
// ----------------------------------------------------------------------
//

