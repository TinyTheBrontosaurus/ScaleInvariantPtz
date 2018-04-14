#include <stdio.h>
#include "SICameraComms.h"
#include "CommandLevel.h"


// Mapping of pixels per degree based on focal length
#define CAM_DEFAULT_PPDEG1 30/*9.6*/
#define CAM_DEFAULT_PPDEG2 32/*13.7*/
#define CAM_DEFAULT_PPDEG3 33/*19.75*/
#define CAM_DEFAULT_PPDEG4 34/*30.71*/
#define CAM_DEFAULT_PPDEG5 41.08
#define CAM_DEFAULT_PPDEG6 53.85
#define CAM_DEFAULT_PPDEG7 81.63
#define CAM_DEFAULT_PPDEG8 94.13
#define CAM_DEFAULT_PPDEG9 108.0
#define CAM_DEFAULT_PPDEG10 123.51

#define CAM_DEFAULT_PPDEG_LENGTH 10
#define CAM_DEFAULT_PPDEG_MAX 10
#define CAM_DEFAULT_PPDEG_MIN 1

const FLOAT defaultMagToPixelPerDegreegLookupTbl[] =
  {CAM_DEFAULT_PPDEG1, CAM_DEFAULT_PPDEG2, CAM_DEFAULT_PPDEG3,
  CAM_DEFAULT_PPDEG4, CAM_DEFAULT_PPDEG5, CAM_DEFAULT_PPDEG6,
  CAM_DEFAULT_PPDEG7, CAM_DEFAULT_PPDEG8, CAM_DEFAULT_PPDEG9,
  CAM_DEFAULT_PPDEG10};


/*****************************************************************
 * SICameraComms ctor
 *****************************************************************
 */

SICameraComms::SICameraComms():Thread()
{
  fixationInfo.sendPanTilt = false;
  fixationInfo.sendZoom = ZOOM_CMD_NONE;
  fixationInfo.zoomMagDigital = 1.0;
  panTiltPulsing = false;
  zoomPulsing = false;
  calibrationState = CAL_STATE_IDLE;
  zoomMagToPixelsPerDegreeTable.load(defaultMagToPixelPerDegreegLookupTbl,
    CAM_DEFAULT_PPDEG_LENGTH, CAM_DEFAULT_PPDEG_MIN, CAM_DEFAULT_PPDEG_MAX,
    true);
    
  ptzCurrentReading = 0;
  ptzBufferFilled = false;
  apply = false;

}

/*****************************************************************
 * SICameraComms dtor
 *****************************************************************
 */
SICameraComms::~SICameraComms()
{
  cameraTxThread.stop();
  cameraRxThread.stop();
  join( 0 );
}

/*****************************************************************
 * SICameraComms::init
 *****************************************************************
 */
void SICameraComms::init( CHAR *serialPort, Semaphore *myTrackingInfoSem,
               Barrier *myBegFrameSyncBar,
               Barrier *myEndFrameSyncBar, BOOL *myExecuteThread,
               SISingleCamOptions *myIntOptions, TrackingInfo *myTrackingInfo,
               TrackingReply *myTrackingReply, IplImage *digitalZoomImage,
               IplImage *digitalZoomBgImage,
               Semaphore *myDigitalZoomOutputSem, VideoStream *vStream,
               Barrier *myHzoPreZoomPrePickoffBarrier,
               Barrier *myHzoPreZoomPostPickoffBarrier,   
               Barrier *myHzoPostZoomPrePickoffBarrier,
               Barrier *myHzoPostZoomPostPickoffBarrier  )
{
  //Call common initializer
  init(serialPort, myTrackingInfoSem, myBegFrameSyncBar, myEndFrameSyncBar,
       myExecuteThread, myIntOptions,
       myTrackingInfo, myTrackingReply );

  //Save all the pointers
  digitalZoomOutputSem = myDigitalZoomOutputSem;
  hzoPreZoomPrePickoffBarrier = myHzoPreZoomPrePickoffBarrier;
  hzoPreZoomPostPickoffBarrier = myHzoPreZoomPostPickoffBarrier;   
  hzoPostZoomPrePickoffBarrier = myHzoPostZoomPrePickoffBarrier;
  hzoPostZoomPostPickoffBarrier = myHzoPostZoomPostPickoffBarrier;
  
  //Initialize Digital zooming
  zoomDigital.setImages( digitalZoomImage, 0 );
  zoomDigital.registerVideo( vStream );

  //Initialize digital zooming
  zoomHybrid.init( &zoomOptical, &zoomDigital );
  
  //Initialize HZO barriers
  zoomDigital.initBarriers(myHzoPreZoomPrePickoffBarrier,
                           myHzoPreZoomPostPickoffBarrier);
  zoomOptical.initBarriers(myHzoPreZoomPrePickoffBarrier,
                           myHzoPreZoomPostPickoffBarrier);
  zoomHybrid.initBarriers(myHzoPreZoomPrePickoffBarrier,
                           myHzoPreZoomPostPickoffBarrier);
}

/*****************************************************************
 * SICameraComms::init
 *****************************************************************
 */
void SICameraComms::init( CHAR *serialPort, Semaphore *myTrackingInfoSem,
               Barrier *myBegFrameSyncBar,
               Barrier *myEndFrameSyncBar, BOOL *myExecuteThread,
               SISingleCamOptions *myIntOptions, TrackingInfo *myTrackingInfo,
               TrackingReply *myTrackingReply )
{
  //Save all the pointers
  trackingInfoSem = myTrackingInfoSem;
  frameBegSynBar = myBegFrameSyncBar;
  frameEndSynBar = myEndFrameSyncBar;
  executeThread = myExecuteThread;
  intOptions = myIntOptions;
  trackingInfo = myTrackingInfo;
  trackingReply = myTrackingReply;
  
  trackingReply->zoomMagOptical = 1.0;
  trackingReply->zoomMagDigital = 1.0;

  //Set up camera communication
  cameraRxTxSockets = 0;
  cameraRxTxClearToSendSem.init(2);

  //Initialize camera communication
  //Initialize camera comms
  printf( "Initializing Sony EVI-D100 Camera on %s...", serialPort );
  //Let user see that message
  fflush(stdout);

  if( cameraComms.init( serialPort ) == 0)
  {
    printf( "success." CRLF CRLF);
    //Set up camera communication
    cameraRxTxSockets = 0;
    cameraRxTxClearToSendSem.init(PTZ_MAX_SOCKETS);

    cameraTxThread.init(cameraComms.getCameraNumber(),
      cameraComms.getCommandLevel()->getFd(), frameBegSynBar,
      frameEndSynBar,
      &cameraRxTxSockets, &cameraRxTxSocketsSem, &cameraRxTxClearToSendSem,
      &panTiltInquirySem, &zoomInquirySem, executeThread);
    cameraRxThread.init(cameraComms.getCameraNumber(), 
      cameraComms.getCommandLevel()->getFd(),
      &cameraRxTxSockets, &cameraRxTxSocketsSem, &cameraRxTxClearToSendSem,
      &panTiltInquirySem, &zoomInquirySem, executeThread);

    panTilt.init( cameraComms.getCameraNumber(), &cameraTxThread, &cameraRxThread );
    zoomOptical.init( cameraComms.getCameraNumber(), &cameraTxThread,
      &cameraRxThread, cameraComms.getCommandLevel() );

  }
  else
  {
    printf( "failed." CRLF CRLF);
  }

  //Indicate no digital zoom
  digitalZoomOutputSem = 0;

  //Setup controls
  panAngleAtLastCapture = 0;
  tiltAngleAtLastCapture = 0;
  zoomMagAtLastCapture = 1.0;

}

/*****************************************************************
 * SICameraComms::execute
 *****************************************************************
 */
void SICameraComms::execute( void *a )
{

  cameraTxThread.start(0);
  cameraRxThread.start(0);

  //Keep process running as long as thread is running
  while( true )
  {
    //Reset error flags
    errorWithPanTiltInquiry = false;
    errorWithZoomInquiry = false;

    //Grab the digital zoom output semaphore
    if( digitalZoomOutputSem )
    {
      digitalZoomOutputSem->acquire();
    }

    //Wait for options to be gathered
    frameBegSynBar->wait();

    //Deal with new options
    if( apply )
    {
      apply = false;
      siTordoff.init( intOptions->kalmanProcessNoise,
                      intOptions->kalmanMeasurementNoise,
                      intOptions->tordoffPsi,
                      intOptions->tordoffGamma1,
                      intOptions->tordoffGamma2 );

      areaEstimator.init(ZOOM_DIG_KALMAN_PROC_NOISE,
                         ZOOM_DIG_KALMAN_MEAS_NOISE );
    }

    //For zoom only if the option is selected
    if( intOptions->digitalZoom || intOptions->opticalZoom )
    {
      if( intOptions->digitalZoom && intOptions->opticalZoom )
      {
        zoom = (Zoom *) &zoomHybrid;
      }
      else if( intOptions->digitalZoom )
      {
        zoom = (Zoom *)&zoomDigital;
      }
      else
      {
        zoom = (Zoom *)&zoomOptical;
      }
    }
    else
    {
      zoom = 0;
    }

    //Have to wait until after the barrier to check executeThread
    if( *executeThread )
    {

      //Release the digital zoom output semaphore if there is no digital zooming
      if( digitalZoomOutputSem &&
          !(intOptions->digitalZoom || intOptions->fixateDigital))
      {
        digitalZoomOutputSem->release();
      }

      //Wait for the tracking information to be ready
      trackingInfoSem->acquire();
      //along with the camera angles
      panTiltInquirySem.acquire();
      panTiltInquirySem.release();
      zoomInquirySem.acquire();
      zoomInquirySem.release();

      if( intOptions->vidSrc == SRC_FRAME_GRABBER )
      {
        errorWithZoomInquiry = !zoomOptical.inquiryBoth( &trackingReply->zoomMagOpticalRaw,
          &trackingReply->zoomMagOptical);
        trackingReply->pixelsPerDegree = 
          trackingReply->zoomMagOptical *
          zoomMagToPixelsPerDegreeTable.lookup(1.0);
      }
      else
      {
        trackingReply->zoomMagOptical = 1.0;
      }


      if( intOptions->vidSrc == SRC_FRAME_GRABBER )
      {
        errorWithPanTiltInquiry = !panTilt.getPanTilt(&trackingReply->panAngle,
                               &trackingReply->tiltAngle);
      }
      else
      {
        trackingReply->panAngle = 0.0;
        trackingReply->tiltAngle = 0.0;
      }


      //Check to make sure the other thread's video stream is working
      if( !trackingInfo->badStream )
      {

        if( !errorWithPanTiltInquiry && !errorWithZoomInquiry )
        {
          setPTZReading( trackingReply->panAngle, trackingReply->tiltAngle,
            trackingReply->zoomMagOptical );
        }

        //Convert tracking information for camera P/T/Z use
        if( !errorWithPanTiltInquiry && !errorWithZoomInquiry ||
            !(intOptions->fixate || intOptions->fixateDigital ) )
        {
          //printf( "Making fixation decisions PT: %d Z: %d Fix: %d (%s @ %d)"CRLF,
          //  !errorWithPanTiltInquiry, !errorWithZoomInquiry, intOptions->fixate,
          //  __FILE__, __LINE__ );
          makeFixationDecisions();
        }
        else
        {
          //printf( "Not making fixation decisions PT: %d Z: %d Fix: %d (%s @ %d)"CRLF,
          //  !errorWithPanTiltInquiry, !errorWithZoomInquiry, intOptions->fixate,
          //  __FILE__, __LINE__ );
        }
               
        //Do digital zoom
        if( digitalZoomOutputSem &&
            (intOptions->digitalZoom || intOptions->fixateDigital))
        {
          sendDigitalZoomCommands();
          //Release semaphore
          digitalZoomOutputSem->release();
        }
        else
        {
          //Meet pre-zoom barriers
          hzoPreZoomPrePickoffBarrier->wait();
          hzoPreZoomPostPickoffBarrier->wait();
                  
        }
        
        //Meet post-zoom barriers
        hzoPostZoomPrePickoffBarrier->wait();
        hzoPostZoomPostPickoffBarrier->wait();

        
        
        //Command the camera
        sendFixationCommands();

        //Release tracking information and wait for display to finish
        trackingInfoSem->release();

        frameEndSynBar->wait();

      }
      else
      {
        //The stream is bad, so clean up synchronization
        trackingInfoSem->release();

        if( digitalZoomOutputSem &&
            ( intOptions->digitalZoom || intOptions->fixateDigital ) )
        {
          digitalZoomOutputSem->release();
        }
      }

    }
    else
    {
      //Let the HZO know we're leaving
      //hybridZoomOutControlBarrier->wait();
      //hybridZoomOutPostControlBarrier->wait();
      //Only leave if executeThread is false
      break;
    }
  }

}

/*****************************************************************
 * SICameraComms::makeFixationDecisions
 *****************************************************************
 */
void SICameraComms::makeFixationDecisions()
{
  //Check to see if this is the first calibration iteration
  if( calSignalStart )
  {
    //Clear the flag
    calSignalStart = false;
    //The first readingis completely zoomed out
    calNextZoomReading = 0;
    //Show that its the first iteration
    calIterationNumber = 0;
    //Set the number of iterations
    calTotalIterations = intOptions->calTableLength;
    //Enter the get size reading state
    enterCalStateGetSizeReading();
    //Notify user of calibration
    printf( "Starting calibration (%s @ %d)."CRLF, __FILE__, __LINE__ );
  }
  else if( calSignalStop )
  {
    //Clear the flag
    calSignalStop = false;
    //Stop the calibration
    calibrationState = CAL_STATE_IDLE;
  }
  else if( calibrationState != CAL_STATE_IDLE )
  {

    //Make sure the object was found, because calibration can't be done otherwise
    if( trackingInfo->objectFound )
    {
      switch( calibrationState )
      {

        case CAL_STATE_GET_SIZE_READING:
          //Make sure zooming is complete and pan/tilt is close enough
          if( ( trackingReply->zoomMagOpticalRaw == calNextZoomReading ) &&
                ( trackingReply->panAngle >= 0.0 - CAM_PANTILT_DEGREES_PER_UNIT &&
                  trackingReply->panAngle <= 0.0 + CAM_PANTILT_DEGREES_PER_UNIT ) )
          {
            //Keep track of a number of measurements in a row
            if( calMeasurementsMadeThisState < CAL_NUM_MEASUREMENTS_TO_MAKE )
            {
              calMeasurementsMadeThisState++;
              printf("Calibration point #%d, W,%d, H,%d, A,%d, Pos1:,%f,(%s @ %d)."CRLF,
                calMeasurementsMadeThisState,
                trackingInfo->width, trackingInfo->height,
                trackingInfo->width * trackingInfo->height, trackingInfo->xPos,
                 __FILE__, __LINE__);
            }
            else
            {
              //If its the first iteration, save the area
              if( calIterationNumber == 0 )
              {
                calInitialArea = trackingInfo->width * trackingInfo->height;
              }

              //Save the magnitude of the zoom in the table
              calZoomReadingTable[calIterationNumber] =
                sqrt( (FLOAT)trackingInfo->width * trackingInfo->height / calInitialArea);
              printf( "Zoom calibration point obtained: [%d]=%f (%s @ %d)."CRLF,
                calIterationNumber, calZoomReadingTable[calIterationNumber],
                __FILE__, __LINE__ );

              //Get the current position of the target object
              calXPositionAtPTOrigin = trackingInfo->xPos;

              //Go to next state
              calMeasurementsMadeThisState = 0;
              calibrationState = CAL_STATE_GET_POSITION_READING;
              //which involves panning and tilting to (1,1)
              fixationInfo.panAngle = 1.0;
              fixationInfo.tiltAngle = 0.0;
              fixationInfo.sendPanTilt = true;
            }
          }
          else
          {
            //Output accordingly if overshoot occurs after the software thinks the
            //pan angle has been met.
            if( calMeasurementsMadeThisState > 0 )
            {
              printf("Overshoot! pan: %f "
                     "Calibration point, W,%d, H,%d, A,%d, (%s @ %d)."CRLF,
                trackingReply->panAngle,
                trackingInfo->width, trackingInfo->height,
                trackingInfo->width * trackingInfo->height,
                 __FILE__, __LINE__);
            }
          }
          break;

        case CAL_STATE_GET_POSITION_READING:
          //Make sure pan/tilt to (1,1) is complete
          if(  trackingReply->panAngle >= 1.0 - CAM_PANTILT_DEGREES_PER_UNIT &&
               trackingReply->panAngle <= 1.0 + CAM_PANTILT_DEGREES_PER_UNIT  )
          {

            //Keep track of a number of measurements in a row
            if( calMeasurementsMadeThisState < CAL_NUM_MEASUREMENTS_TO_MAKE )
            {
              calMeasurementsMadeThisState++;
              printf( "Pixels Per Degree calibration point(%d): "
                      ",pos2[%d]=,%f, (%s @ %d)."CRLF,
                calMeasurementsMadeThisState,
                calIterationNumber, trackingInfo->xPos,
                __FILE__, __LINE__ );
            }
            else
            {
              //Get the pixels per degree at this zoom level
              calPixelsPerDegreeTable[calIterationNumber] =
                calXPositionAtPTOrigin - trackingInfo->xPos;
              printf( "Pixels Per Degree calibration point obtained: "
                      "[%d]=%f (%s @ %d)."CRLF,
                calIterationNumber, calPixelsPerDegreeTable[calIterationNumber],
                __FILE__, __LINE__ );

              //Go to the next iteration
              calIterationNumber++;
              if( calIterationNumber < calTotalIterations )
              {
                //Calculate next zoom position
                calNextZoomReading = (UINT16)((FLOAT)
                  ZOOM_OPTICAL_MAX_READING * calIterationNumber / (calTotalIterations-1));

                //Go back to GET_SIZE_READING state
                enterCalStateGetSizeReading();

              }
              //It is done, so save the tables
              else
              {
                //Set the lookup tables
                zoomOptical.setLookup(calZoomReadingTable, calTotalIterations, 0,
                  ZOOM_OPTICAL_MAX_READING, true);
                zoomMagToPixelsPerDegreeTable.load(calPixelsPerDegreeTable,
                  calTotalIterations, 1, calZoomReadingTable[calTotalIterations-1],
                  true);
                printf( "Zoom reading and pan/tilt calibration step completed "
                        "(%s @ %d)."CRLF,
                  __FILE__, __LINE__ );

                //Go to next state
                calibrationState = CAL_STATE_PRINCIPAL_POINT_FIXATE;
                calMeasurementsMadeThisState = 0;

                //Re-center the camera
                fixationInfo.panAngle = 0.0;
                fixationInfo.tiltAngle = 0.0;
                fixationInfo.sendPanTilt = true;

              }
            }
          }
          else
          {
            //Output accordingly if overshoot occurs after the software thinks the
            //pan angle has been met.
            if( calMeasurementsMadeThisState > 0 )
            {
              printf( "Overshoot! Pan: %f Pixels Per Degree calibration point(%d): "
                      ",pos2[%d]=,%f, (%s @ %d)."CRLF,
                trackingReply->panAngle,
                calMeasurementsMadeThisState,
                calIterationNumber, trackingInfo->xPos,
                __FILE__, __LINE__ );
            }
          }
          break;

        case CAL_STATE_PRINCIPAL_POINT_FIXATE:
          //Make sure if it is re-centered
          if( trackingReply->panAngle >= 0.0 - CAM_PANTILT_DEGREES_PER_UNIT &&
              trackingReply->panAngle <= 0.0 + CAM_PANTILT_DEGREES_PER_UNIT )
          {

            //Keep track of a number of measurements in a row
            if( calMeasurementsMadeThisState < CAL_NUM_MEASUREMENTS_TO_MAKE )
            {
              calMeasurementsMadeThisState++;
            }
            else
            {
              //Keep track of the drift of the origin when zooming out
              calPrinPtBox.setOriginalBox( trackingInfo->roi, IM_WIDTH/2, IM_HEIGHT/2,
                &calSubBox );

              //Now go to the next state
              calibrationState = CAL_STATE_GET_PRINCIPAL_POINT_READING;
              calMeasurementsMadeThisState = 0;

              //Zoom out fully
              fixationInfo.zoomMagOptical = 1.0;
              fixationInfo.zoomMagDigital = 1.0;
              fixationInfo.sendZoom = ZOOM_CMD_MAG;
            }
          }
          break;

        case CAL_STATE_GET_PRINCIPAL_POINT_READING:
          //Make sure it is fully zoomed out
          if( trackingReply->zoomMagOptical == 1.0 )
          {
            //Keep track of a number of measurements in a row
            if( calMeasurementsMadeThisState < CAL_NUM_MEASUREMENTS_TO_MAKE )
            {
              calMeasurementsMadeThisState++;
            }
            else
            {
              //The new principal point
              SINT16 ppX;
              SINT16 ppY;

              calPrinPtBox.getNewSubBox( trackingInfo->roi, &calSubBox,
                &ppX, &ppY );

              //The location of the object is the principal point
              zoomDigital.setPrincipalPoint( ppX, ppY );
              //Report the findings
              printf( "Principal point found to be (%d, %d) (%s @ %d)"CRLF,
                ppX, ppY, __FILE__, __LINE__ );

              //Go to idle state
              calibrationState = CAL_STATE_IDLE;
              printf( "Calibration complete (%s @ %d)."CRLF, __FILE__, __LINE__ );
            }
          }
          break;

        default:
          printf( "Bad CalibrationState (%d) in %s @ %d"CRLF, (UINT32)calibrationState,
            __FILE__, __LINE__ );
          break;
      }
    }

  }
  //See if fixation is happening
  else if( intOptions->fixate || intOptions->scaleInv || intOptions->fixateDigital)
  {
    //Fixate only if tracking information is accurate
    if( trackingInfo->objectFound )
    {
      fixateToPoint(0.0,0.0);
    }

  }
  //Handle other operations that may be pending
  else
  {

    //Check pulsing for P/T
    if( panTiltPulsing )
    {
      /*printf( "Pulsing report: ,Pan:,%1.3f,H:,%d,W:,%d,A:,%d,xPos:,%f, (%s @ %d)"CRLF,
        trackingReply->panAngle,trackingInfo->height, trackingInfo->width,
        trackingInfo->height * trackingInfo->width,
        trackingInfo->xPos, __FILE__, __LINE__ );*/

      //See if this iteration is finished yet
      if( --panTiltPulsingFramesLeftThisIteration == 0 )
      {
        //If it's finished, check where the motor is
        if( panTiltPulsingForward = !panTiltPulsingForward )
        {
          //Motor is in position 2, so check to see if there are any iterations left
          if( --panTiltPulsingIterationsLeft == 0 )
          {
            //Done
            panTiltPulsing = false;
            printf( "Pan/Tilt pulsing complete."CRLF);
          }
          else
          {
            printf( "Pulsing: Sending comand (%s @ %d)"CRLF, __FILE__, __LINE__ );
            //send to position 1
            panTiltPulsingFramesLeftThisIteration = intOptions->panPulseFrames;
            fixationInfo.sendPanTilt = true;
            fixationInfo.panAngle = intOptions->panPos1;
            fixationInfo.tiltAngle = intOptions->tiltPos1;
          }
        }
        else
        {
          printf( "Pulsing: Sending comand (%s @ %d)"CRLF, __FILE__, __LINE__ );
          //Motor is in position 1, so send to position 2
          panTiltPulsingFramesLeftThisIteration = intOptions->panPulseFrames;
          fixationInfo.sendPanTilt = true;
          fixationInfo.panAngle = intOptions->panPos2;
          fixationInfo.tiltAngle = intOptions->tiltPos2;
        }
      }
    }

    //Check pulsing for Zoom
    if( zoomPulsing )
    {

      /*printf( "Pulsing report: ,Zoom:,%1.3f,%d,H:,%d,W:,%d,A:,%d,xPos:,%f, (%s @ %d)"CRLF,
        trackingReply->zoomMagOptical, trackingReply->zoomMagOpticalRaw,
        trackingInfo->height, trackingInfo->width,
        trackingInfo->height * trackingInfo->width,
        trackingInfo->xPos, __FILE__, __LINE__ );*/

      //See if this iteration is finished yet
      if( --zoomPulsingFramesLeftThisIteration == 0 )
      {
        //If it's finished, check where the motor is
        if( zoomPulsingForward = !zoomPulsingForward )
        {
          //Motor is in position 2, so check to see if there are any iterations left
          if( --zoomPulsingIterationsLeft == 0 )
          {
            //Done
            zoomPulsing = false;
            printf( "Zoom pulsing complete."CRLF);
          }
          else
          {
            //send to position 1
            zoomPulsingFramesLeftThisIteration = intOptions->zoomPulseFrames;
            fixationInfo.sendZoom = ZOOM_CMD_MAG;
            fixationInfo.zoomMagOptical = intOptions->zoomPos1;
            fixationInfo.zoomMagDigital = intOptions->zoomPos1;
          }
        }
        else
        {
          //Motor is in position 1, so send to position 2
          zoomPulsingFramesLeftThisIteration = intOptions->zoomPulseFrames;
          fixationInfo.sendZoom = ZOOM_CMD_MAG;
          fixationInfo.zoomMagOptical = intOptions->zoomPos2;
          fixationInfo.zoomMagDigital = intOptions->zoomPos2;
        }
      }
    }


    //Take commands from the UI
    //for Pan/Tilt
    switch( trackingInfo->panTiltCommand )
    {
      case CMD_STEP:
        //Send the step command
        fixationInfo.sendPanTilt = true;
        fixationInfo.panAngle = intOptions->panPos1;
        fixationInfo.tiltAngle = intOptions->tiltPos1;
        break;
      case CMD_PULSE:
        //Initialize the pulse
        panTiltPulsing = true;
        panTiltPulsingForward = true;
        //Note: only using pan values, ignoring tilt values
        panTiltPulsingIterationsLeft = intOptions->panIterations;
        panTiltPulsingFramesLeftThisIteration = intOptions->panPulseFrames;

        //Signal movement
        fixationInfo.sendPanTilt = true;
        fixationInfo.panAngle = intOptions->panPos1;
        fixationInfo.tiltAngle = intOptions->tiltPos1;

        break;
      case CMD_NONE:
        break;
      default:
        printf( "Bad pan/tilt CommandType in %s:%d" CRLF, __FILE__, __LINE__ );
        break;
    }

    //For zoom only if the option is selected
    if( intOptions->digitalZoom || intOptions->opticalZoom )
    {
      if( intOptions->digitalZoom && intOptions->opticalZoom )
      {
        zoom = (Zoom *) &zoomHybrid;
      }
      else if( intOptions->digitalZoom )
      {
        zoom = (Zoom *)&zoomDigital;
      }
      else
      {
        zoom = (Zoom *)&zoomOptical;
      }

      //Take commands from the UI
      //for Zoom
      switch( trackingInfo->zoomCommand )
      {
        case CMD_STEP:
          fixationInfo.sendZoom = ZOOM_CMD_MAG;
          fixationInfo.zoomMagOptical = intOptions->zoomPos1;
          fixationInfo.zoomMagDigital = intOptions->zoomPos1;
          break;
        case CMD_PULSE:
          //Initialize the pulse
          zoomPulsing = true;
          zoomPulsingForward = true;
          //Note: only using pan values, ignoring tilt values
          zoomPulsingIterationsLeft = intOptions->zoomIterations;
          zoomPulsingFramesLeftThisIteration = intOptions->zoomPulseFrames;

          //Signal movement
          fixationInfo.sendZoom = ZOOM_CMD_MAG;
          fixationInfo.zoomMagOptical = (SINT16)intOptions->zoomPos1;
          fixationInfo.zoomMagDigital = (SINT16)intOptions->zoomPos1;
          break;
        case CMD_NONE:
          break;
        default:
          printf( "Bad pan/tilt CommandType in %s:%d" CRLF, __FILE__, __LINE__ );
          break;
      }
    }
  }
}

/*****************************************************************
 * SICameraComms::sendFixationCommands
 *****************************************************************
 */
void SICameraComms::sendFixationCommands()
{

  //Check the reset flag, which overrides all
  if( trackingInfo->reset )
  {
    //Reset pan tilt and zoom
    panTilt.reset();
    panTiltPulsing = false;
    fixationInfo.sendPanTilt = false;

    if( zoom != 0 )
    {
      zoom->reset();
    }
    zoomPulsing = false;
    fixationInfo.sendZoom = ZOOM_CMD_NONE;
    fixationInfo.zoomMagOptical = 1.0;
    fixationInfo.zoomMagDigital = 1.0;

    //Turn off calibration
    stopCalibration();

    siTordoff.init();
    areaEstimator.init();

    panAngleAtLastCapture = 0;
    tiltAngleAtLastCapture = 0;
    zoomMagAtLastCapture = 1.0;


    //Reset flag
    trackingInfo->reset = false;
  }
  else
  {
    //Send the pan and tilt commands if the flag is set, and reset the flag
    if( fixationInfo.sendPanTilt )
    {
      panTilt.move(fixationInfo.panAngle, fixationInfo.tiltAngle);
      fixationInfo.sendPanTilt = false;
    }

    //Send the zoom command if the flag is set, and reset the flag
    switch( fixationInfo.sendZoom )
    {
      case ZOOM_CMD_MAG:
        if( zoom != 0 )
        {
          zoom->zoom( fixationInfo.zoomMagOptical );
        }
        break;
      case ZOOM_CMD_RAW:
        zoomOptical.zoomRaw( fixationInfo.zoomRaw );
        break;
      case ZOOM_CMD_NONE:
        break;
      default:
        printf( "Bad type in %s @ %d"CRLF, __FILE__, __LINE__ );
        break;
    }
    fixationInfo.sendZoom = ZOOM_CMD_NONE;
  }

}

/*****************************************************************
 * SICameraComms::sendDigitalZoomCommands
 *****************************************************************
 */
void SICameraComms::sendDigitalZoomCommands()
{
  if( zoom != 0 )
  {
    if( intOptions->digitalZoom)
    {
      if( intOptions->fixateDigital )
      {
        zoom->zoomImage(fixationInfo.zoomMagDigital,  
                      trackingReply->zoomMagOptical,
                      trackingInfo->xPos + IM_WIDTH/2,
                      trackingInfo->yPos + IM_HEIGHT/2);
      }
      else
      {
        zoom->zoomImage(fixationInfo.zoomMagDigital, 
                      trackingReply->zoomMagOptical );

      }
    }
    else
    {
      if( intOptions->fixateDigital )
      {
        zoomDigital.zoomImage(1.0, 1.0,
                      trackingInfo->xPos + IM_WIDTH/2,
                      trackingInfo->yPos + IM_HEIGHT/2);
      }
      else
      {
        printf( "Error! No digital fixation nor digital zoom in"
                " sendDigitalZoomCommands (%s @ %d)"CRLF,
                 __FILE__, __LINE__ );

      }
    }
    if( !zoomDigital.inquiry(&trackingReply->zoomMagDigital ) )
    {
      printf( "Problem with digital zoom inquiry (%s @ %d)"CRLF, __FILE__, __LINE__ );
    }
  }
  else
  {
    if( intOptions->fixateDigital )
    {
      zoomDigital.zoomImage(1.0, 1.0,
                    trackingInfo->xPos + IM_WIDTH/2,
                    trackingInfo->yPos + IM_HEIGHT/2);
    }
    else
    {
      printf( "Error! No digital fixation nor digital zoom in"
              " sendDigitalZoomCommands (%s @ %d)"CRLF,
                __FILE__, __LINE__ );

    }
  }
}

/*****************************************************************
 * SICameraComms::getZoomOptical
 *****************************************************************
 */
ZoomOptical *SICameraComms::getZoomOptical()
{
  return &zoomOptical;
}

/*****************************************************************
 * SICameraComms::getZoomDigital
 *****************************************************************
 */
ZoomDigital *SICameraComms::getZoomDigital()
{
  return &zoomDigital;
}

/*****************************************************************
 * SICameraComms::getPixelsPerDegreeTable
 *****************************************************************
 */
LookupTable *SICameraComms::getPixelsPerDegreeTable()
{
  return &zoomMagToPixelsPerDegreeTable;
}
/*****************************************************************
 * SICameraComms::startCalibration
 *****************************************************************
 */
void SICameraComms::startCalibration()
{
  calSignalStart = true;
}

/*****************************************************************
 * SICameraComms::stopCalibration
 *****************************************************************
 */
void SICameraComms::stopCalibration()
{
  calSignalStop = true;
}

/*****************************************************************
 * SICameraComms::applyOptions
 *****************************************************************
 */
void SICameraComms::applyOptions()
{
  apply = true;
}

/*****************************************************************
 * SICameraComms::enterCalStateGetSizeReading
 *****************************************************************
 */
void SICameraComms::enterCalStateGetSizeReading()
{
  //Start the calibration
  calibrationState = CAL_STATE_GET_SIZE_READING;

  //Signal the zoom to move
  fixationInfo.zoomRaw = calNextZoomReading;
  fixationInfo.sendZoom = ZOOM_CMD_RAW;

  //Signal the pan/tilt to center
  fixationInfo.panAngle = 0.0;
  fixationInfo.tiltAngle = 0.0;
  fixationInfo.sendPanTilt = true;

  //Reset the number of measurements made
  calMeasurementsMadeThisState = 0;
}

/*****************************************************************
 * SICameraComms::fixateToPoint
 *****************************************************************
 */
void SICameraComms::fixateToPoint( FLOAT xPoint, FLOAT yPoint )
{
  FLOAT panAngleAtCapture;
  FLOAT tiltAngleAtCapture;
  FLOAT zoomMagAtCapture;
  FLOAT panCmdTordoff;
  FLOAT tiltCmdTordoff;
  FLOAT zoomCmdTordoff;
  FLOAT panPixelCmd;
  FLOAT tiltPixelCmd;

  //Get the pan/tilt/zoom that corresponds to the tracking
  if( getPTZForThisFrame(&panAngleAtCapture, &tiltAngleAtCapture,
        &zoomMagAtCapture ) )
  {
  
    if( intOptions->fixationAlg == FIX_KALMAN )
    {
      //Update filter
      siTordoff.setMeasurement(((FLOAT)trackingInfo->xPos)/IM_WIDTH,
                            ((panAngleAtLastCapture - panAngleAtCapture )*
                            zoomMagToPixelsPerDegreeTable.lookup(1.0)) / IM_WIDTH,
                            ((FLOAT)trackingInfo->yPos)/IM_HEIGHT,
                            ((tiltAngleAtCapture - tiltAngleAtLastCapture)*
                            zoomMagToPixelsPerDegreeTable.lookup(1.0)) / IM_WIDTH,
                            zoomMagAtCapture );

      //Keep track of pan and tilt angles
      panAngleAtLastCapture = panAngleAtCapture;
      tiltAngleAtLastCapture = tiltAngleAtCapture;

      //Get the commands to send to the camera
      siTordoff.getCommands(panCmdTordoff, tiltCmdTordoff, zoomCmdTordoff,
        intOptions->tordoffErrToHigherZoom );
    }
    if( intOptions->fixate /*&& !errorWithPanTiltInquiry*/)
    {

      switch( intOptions->fixationAlg )
      {
        case FIX_P:
          //Move object to (xPoint,yPoint) using negative of position of object
          fixationInfo.panAngle = (FLOAT)(panAngleAtCapture +
            ((FLOAT)trackingInfo->xPos - xPoint )/
            zoomMagToPixelsPerDegreeTable.lookup(zoomMagAtCapture)*
            intOptions->fixationGain);
          fixationInfo.tiltAngle = (FLOAT)(tiltAngleAtCapture -
            ((FLOAT)trackingInfo->yPos - yPoint) /
            zoomMagToPixelsPerDegreeTable.lookup(zoomMagAtCapture)*
            intOptions->fixationGain);
          break;

        case FIX_KALMAN:
          //Convert the commands to pixels
          panPixelCmd = (panCmdTordoff * IM_WIDTH);
          tiltPixelCmd = (tiltCmdTordoff * IM_HEIGHT);
          trackingReply->kalmanXPos = panPixelCmd;
          trackingReply->kalmanYPos = tiltPixelCmd;
          //Then convert the commands to angles
          fixationInfo.panAngle = (FLOAT)(panAngleAtCapture +
                              (panPixelCmd - xPoint) /
                              zoomMagToPixelsPerDegreeTable.lookup(1.0)*
                              intOptions->fixationGain);
          fixationInfo.tiltAngle = (FLOAT)(tiltAngleAtCapture -
                              (tiltPixelCmd - yPoint) /
                              zoomMagToPixelsPerDegreeTable.lookup(1.0)
                              *intOptions->fixationGain);
          break;
        default:
          printf( "Bad FixationType (%d) (%s @ %d)"CRLF,
            (UINT8) intOptions->fixationAlg, __FILE__, __LINE__ );
          break;
      }

      //Tell the commands to be sent
      fixationInfo.sendPanTilt = true;

      #ifndef DO_PAN
      fixationInfo.panAngle = panAngleAtCapture;
      #endif

      #ifndef DO_TILT
      fixationInfo.tiltAngle = tiltAngleAtCapture;
      #endif

      #ifdef DEBUG_FIXATION
      printf( "Fixation (%1.2f, %1.2f) - pan: %2.3f to %2.3f (%2.1f (%2.1f)pixels)"
              "  tilt: %2.3f to %2.3f (%2.1f (%2.1f)pixels) (%s @ %d)"CRLF,
              xPoint, yPoint,
              panAngleAtCapture,fixationInfo.panAngle,
              trackingInfo->xPos-xPoint, panPixelCmd - xPoint,
              tiltAngleAtCapture, fixationInfo.tiltAngle,
              trackingInfo->yPos-yPoint, tiltPixelCmd - yPoint,
              __FILE__, __LINE__ );
      #endif
    }
    
    
    //Control zoom lens
    if( intOptions->scaleInv )
    {
      
      UINT32 measuredArea = trackingInfo->width * trackingInfo->height;
      FLOAT  zEstimate;
      estimatedAreaAtLastCommand = estimatedAreaAtLastCommand *
        ( 1 - intOptions->zoomCoeff ) + 
        (FLOAT)measuredArea * intOptions->zoomCoeff;
      FLOAT zoomCmdBasic = 
        sqrt( (FLOAT)intOptions->targetAreaOpticalZoom / estimatedAreaAtLastCommand ) 
        * zoomMagAtCapture;

      //Calculate the digital zoom using a Kalman filter
      FLOAT measurement = 1/(zoomMagAtCapture*
        sqrt( (FLOAT)measuredArea));

      //Set measurement noise
      areaEstimator.scaleMeasurementNoise( 9.0 /
        (measuredArea * measuredArea * zoomMagAtCapture * zoomMagAtCapture));

      //Setup area estimator. See notes 8/5 page 3
      areaEstimator.setMeasurement( measurement,
        1/( sqrt((FLOAT)measuredAreaAtLastCapture ))*
        (1/zoomMagAtCapture-1/zoomMagAtLastCapture), 1.0);

      //Save zoom magnitude
      zoomMagAtLastCapture = zoomMagAtCapture;
      measuredAreaAtLastCapture = (FLOAT)measuredArea;

      //Send the command
      fixationInfo.sendZoom = ZOOM_CMD_MAG;
      zEstimate = areaEstimator.getCommand();
      trackingReply->kalmanZPos = zEstimate;
      fixationInfo.zoomMagDigital = sqrt((FLOAT)intOptions->targetAreaDigitalZoom)*
                              zEstimate*
                              zoomMagAtCapture*zoomMagAtCapture;
      #ifdef DEBUG_SCALE_INV
      printf( "Scale Invariance (%7u) - Area: %7u (Meas) %7.0f (Est) Zoom: %2.3lf to %2.3lf"
              "(%s @ %d)"CRLF,
              intOptions->targetAreaOpticalZoom, measuredArea,
              pow(1/(areaEstimator.getCommand() * zoomMagAtCapture),2),
              zoomMagAtCapture, fixationInfo.zoomMagOptical,
              __FILE__, __LINE__ );
      #endif
      #ifdef DEBUG_SCALE_INV_KALMAN
      printf( "Scale Invariant KF - Area: %7u Pos: %1.3f (Meas) %1.3f (Est) Zoom: to %2.3lf"
              "(%s @ %d)"CRLF,
              measuredArea, measurement, areaEstimator.getCommand(),
              fixationInfo.zoomMag,
              __FILE__, __LINE__ );

      #endif

      //Do the optical zoom
      fixationInfo.sendZoom = ZOOM_CMD_MAG;
      fixationInfo.zoomMagOptical = SI_MIN( zoomCmdTordoff, zoomCmdBasic );

    }

  }
  else
  {
    #ifdef DEBUG_FIXATION
    printf( "Fixation: Unable to acquire current P/T/Z points (%s @ %d)"CRLF,
      __FILE__, __LINE__ );
    #endif

  }
}

/*****************************************************************
 * SICameraComms::setPTZReading
 *****************************************************************
 */
void SICameraComms::setPTZReading( FLOAT myPanAngle, FLOAT myTiltAngle,
      FLOAT myZoomMag )
{

  savedPanAngle = myPanAngle;
  savedTiltAngle = myTiltAngle;
  savedPvsZoomMag = savedZoomMag;
  savedZoomMag = myZoomMag;

}

/*****************************************************************
 * SICameraComms::getPTZReading
 *****************************************************************
 */
BOOL SICameraComms::getPTZForThisFrame( FLOAT *myPanAngle, FLOAT *myTiltAngle,
      FLOAT *myZoomMag )
{

  *myPanAngle = savedPanAngle;
  *myTiltAngle = savedTiltAngle;
  *myZoomMag = savedPvsZoomMag + 
    intOptions->zoomDelayCoeff * (savedZoomMag - savedPvsZoomMag );
  return true;

}

// File: $Id: SICameraComms.cpp,v 1.58 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SICameraComms.cpp,v $
// Revision 1.58  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.57  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.56  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.55  2005/09/07 01:05:16  edn2065
// Finished CVC implementation
//
// Revision 1.54  2005/09/06 19:44:17  edn2065
// Moved HZO barriers to Zoom to help parallelism
//
// Revision 1.53  2005/09/06 01:57:12  edn2065
// Added zoom delay handling
//
// Revision 1.52  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.51  2005/08/26 20:27:13  edn2065
// Pickoff points work. Implemented HZO camera setup
//
// Revision 1.50  2005/08/26 18:04:35  edn2065
// Added two pickoff points for SIHzo and added better naming convention to barriers
//
// Revision 1.49  2005/08/25 23:55:59  edn2065
// Removed deadlock in SIHzo and separated SISingleCams from GUI
//
// Revision 1.48  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SISingleCam
//
// Revision 1.47  2005/08/24 15:38:46  edn2065
// Committing before HZO
//
// Revision 1.46  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.45  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.44  2005/08/10 20:07:07  edn2065
// Digital zoom in fixation works. zoom out doesnt
//
// Revision 1.43  2005/08/10 02:48:03  edn2065
// Changed to allow warning and error free ICC compile
//
// Revision 1.42  2005/08/09 21:59:27  edn2065
// Added easy serialPort switching via SISingleCamOptions files
//
// Revision 1.41  2005/08/09 04:23:42  edn2065
// Scaled measurement noise to go with zoom
//
// Revision 1.39  2005/08/03 02:34:54  edn2065
// Added tordoff psi and gammas to menu
//
// Revision 1.38  2005/08/03 01:43:26  edn2065
// Implemented focal length selection
//
// Revision 1.37  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.36  2005/07/28 20:57:49  edn2065
// Kalman filter working
//
// Revision 1.35  2005/07/28 18:10:33  edn2065
// Added SIKalman
//
// Revision 1.33  2005/07/28 00:22:24  edn2065
// Added SITordoff to repository
//
// Revision 1.32  2005/07/26 20:31:51  edn2065
// Gave up trying to improve timing
//
// Revision 1.31  2005/07/26 18:46:43  edn2065
// Added scale invariance
//
// Revision 1.30  2005/07/18 23:27:28  edn2065
// Integrated RxTxThread and fixed major deadlocks
//
// Revision 1.29  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.28  2005/07/14 20:16:00  edn2065
// Added commenting to RxTxThread
//
// Revision 1.27  2005/07/13 23:54:49  edn2065
// Power off save
//
// Revision 1.26  2005/06/30 14:46:28  edn2065
// overnight checkin
//
// Revision 1.25  2005/06/28 20:01:49  edn2065
// Debugging comms bugs. overnight check in
//
// Revision 1.24  2005/06/27 18:49:55  edn2065
// added auto bps checking to amishs code
//
// Revision 1.23  2005/06/22 19:48:16  edn2065
// Weekend commit
//
// Revision 1.22  2005/06/22 16:57:40  edn2065
// Tied pantilt and zoom readings together to make them faster
//
// Revision 1.21  2005/06/21 14:56:15  edn2065
// Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
//
// Revision 1.20  2005/06/21 14:09:07  edn2065
// Added PartialBox principal point algorithm to kernel. Early tests work.
//
// Revision 1.19  2005/06/21 11:13:04  edn2065
// Added delay handling for P/T/Z.
//
// Revision 1.18  2005/06/17 18:35:51  edn2065
// Added verbosity to pulsing
//
// Revision 1.17  2005/06/14 18:22:40  edn2065
// Implemented saving for calibration
//
// Revision 1.16  2005/06/14 16:06:41  edn2065
// Extended calibration algorithm
//
// Revision 1.15  2005/06/14 14:40:21  edn2065
// Added better precision to pantilt commands as well
//
// Revision 1.14  2005/06/14 12:19:58  edn2065
// Made PT inquiry a float instead of int. Camera resolution was being thrown away
//
// Revision 1.13  2005/06/13 21:04:19  edn2065
// Implemented fixating. Needs to be fine tuned
//
// Revision 1.12  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.11  2005/06/10 17:03:47  edn2065
// Added MFS
//
// Revision 1.10  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.9  2005/06/10 11:55:51  edn2065
// Fixed reverseLookup error. Added reset option
//
// Revision 1.8  2005/06/09 20:51:22  edn2065
// Added pulsing to zoom and pt
//
// Revision 1.7  2005/06/09 18:47:40  edn2065
// Implemented Hybrid zoom
//
// Revision 1.6  2005/06/09 14:03:04  edn2065
// Added digital and raw optical to zoom inquiry output
//
// Revision 1.5  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.4  2005/06/08 15:08:00  edn2065
// Add P/T/Z inquiry
//
// Revision 1.3  2005/06/08 13:44:18  edn2065
// Overnight checkin
//
// Revision 1.2  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
// Revision 1.1  2005/06/07 10:44:46  edn2065
// Added to repository
//
//

