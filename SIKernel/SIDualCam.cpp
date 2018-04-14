#include <stdio.h>
#include <unistd.h>
#include "SIDualCam.h"

/*****************************************************************
 * SIDualCam ctor
 *****************************************************************
 */

SIDualCam::SIDualCam() : 
  Thread(),
  movePanImgToZoomBgBarrier(3),
  startZoomBarrier(2),
  endZoomBarrier(2),
  postArbitrationBarrier(3),
  dummyBarrier(1),
  panCam( new SISingleCam(&dummyBarrier, &dummyBarrier, 
                       &movePanImgToZoomBgBarrier,
                       &postArbitrationBarrier, "Panoramic Camera",
    "lastPanoramic")),
  zoomCam(new SISingleCam(&movePanImgToZoomBgBarrier, 
                       &startZoomBarrier,
                       &endZoomBarrier,
                       &postArbitrationBarrier, "Zooming Camera", 
    "lastZoom")),
  vDstScreen( "Hybrid Zoom View" )   
{

  options.loadOptions("../../config/lastHybrid");
  vDst = &vDstScreen;
  
  apply = true;
  doReset = false;
  doResetNextIter = false;
  signalThreadStop = false;
  signalSetupCameras = false;
  camerasSyncd = false;
  consecutiveBadFrames = 0;
  saveJustActivated = true;
  
  //Start the thread
  start(0);

}

/*****************************************************************
 * SIDualCam dtor
 *****************************************************************
 */
SIDualCam::~SIDualCam() {

  //Stop execution
  signalThreadStop = true;  
  delete panCam;
  delete zoomCam;      
  join( 0 );
  statsLogger.endFile();
  options.saveOptions("../../config/lastHybrid");
}

/*****************************************************************
 * SIDualCam::execute
 *****************************************************************
 */
void SIDualCam::execute( void *a ) {

  threadRunning = true;
  //Keep process running as long as thread is running
  while( true )
  {                   
    //Wait for the SISingleCams to complete everything except sending commands
    movePanImgToZoomBgBarrier.wait();
    
    //Only time to update the options
    updateOptions();
    
    //Move panoramic result to zooming camera's background
    zoomCam->setBgImg( panCam->getZoomedImg() );
    
    //Let zooming camera continue
    startZoomBarrier.wait();
    //Determine the state of the cameras
    determineState();
    //Wait for zooming camera to finish
    endZoomBarrier.wait();
            
    //Override camera control if necessary
    if( intOptions.arbitrateControl )
    {
      arbitrateControl();
    }      
    //Override camera display if necessary    
    if( intOptions.arbitrateDisplay && !saveJustActivated )
    {
      arbitrateDisplay();
    }
    
    if( intOptions.saveStats && !saveJustActivated )
    {
      saveStats();
    }
    
    if( saveJustActivated )
    {
      CHAR pathStr[CO_PATH_LENGTH];
      //Setup hybrid stream save
      if( intOptions.vidDstSaveSplicedStream )
      {
        //Set the path
        sprintf( pathStr, "%s%sHybrid", intOptions.vidDstDirectory,
          intOptions.vidDstPrefix );
        vDstFile.setFile(pathStr, intOptions.vidDstSuffix);
      }
      
      //Setup original and digital stream saves
      if( intOptions.vidDstSaveOrigStream || intOptions.vidDstSaveDigStream )
      {
        sprintf( pathStr, "%s%s", intOptions.vidDstDirectory,
          intOptions.vidDstPrefix );      
        zoomCam->setFileSaveParams(pathStr, "Z", intOptions.vidDstSuffix,
          intOptions.vidDstSaveDigStream, intOptions.vidDstSaveOrigStream);
        panCam->setFileSaveParams(pathStr, "P", intOptions.vidDstSuffix,
          intOptions.vidDstSaveDigStream, intOptions.vidDstSaveOrigStream);
      }
      
      if( intOptions.saveStats )
      {        
        sprintf( pathStr, "%s%s.mfs.csv", intOptions.vidDstDirectory,
          intOptions.vidDstPrefix );      
        statsLogger.startFile( pathStr );    
      }
      
      saveJustActivated = false;
    }

    //See if the threads are done
    if( !threadRunning )
    {
      zoomCam->stop();
      panCam->stop();      
    }
              
    //Let SISingleCams continue
    postArbitrationBarrier.wait();
    
    //Leave if the thread is done
    if( !threadRunning )
    {
      break;
    }

  }

}

/*****************************************************************
 * SIDualCam::applyOptions
 *****************************************************************
 */
void SIDualCam::applyOptions()
{
  apply = true;
}

/*****************************************************************
 * SIDualCam::reset
 *****************************************************************
 */
void SIDualCam::reset()
{
  doReset = true;
}

/*****************************************************************
 * SIDualCam::setupCameras
 *****************************************************************
 */
void SIDualCam::setupCameras()
{
  signalSetupCameras = true;
}


/*****************************************************************
 * SIDualCam::updateOptions
 *****************************************************************
 */
void SIDualCam::updateOptions()
{
  saveJustActivated = false;
  
  if( doResetNextIter  )
  {
    options.arbitrateControl = false;
    options.vidDstSaveOrigStream = false;
    options.vidDstSaveDigStream = false;
    options.vidDstSaveSplicedStream = false;
    options.saveStats = false;
      
    apply = true;
    doResetNextIter = false;
  }
  
  //Reset the next iteration to sync with SISingleCams
  if( doReset )
  {
    doResetNextIter = true;
    doReset = false;
  }
  
  if( signalSetupCameras )
  {
    //Load the appropriate options
    panCam->options.loadOptions("../../config/hzoPanoramic");
    zoomCam->options.loadOptions("../../config/hzoZoom");
    //Clear the flag
    signalSetupCameras = false;
  }
  if( apply )
  {
    apply = false;
    printf( "Applying HZO changes..." CRLF );
    intOptions.copy( &options );
    
    consecutiveBadFrames = 0;
        
    saveJustActivated = intOptions.vidDstSaveSplicedStream ||
                        intOptions.vidDstSaveOrigStream || 
                        intOptions.vidDstSaveDigStream ||
                        intOptions.saveStats;                            
                        
    if( !intOptions.saveStats )
    {
      statsLogger.endFile();
    } 
    
  }
  if( signalThreadStop )
  {
    threadRunning = false;
  }
}


/*****************************************************************
 * SIDualCam::arbitrateControl
 *****************************************************************
 */
void SIDualCam::arbitrateControl()
{
  
  //Set minimum zoom level 
  if( zoomCam->getFixationInfo()->sendZoom == ZOOM_CMD_MAG &&
      zoomCam->getFixationInfo()->zoomMagOptical < intOptions.minZoomMagZoomCam )
  {
    zoomCam->getFixationInfo()->zoomMagOptical = intOptions.minZoomMagZoomCam;
  }

  //See 9/9 pp 3-5 for derivation of camera view correspondence
  if( !zoomCamInView && panCamInView )
  {

    FLOAT estZ;
     
    //Use camera correspondence to put cameras together AND zoom out
    //Lock tilts
    zoomCam->getFixationInfo()->tiltAngle =
      panCam->getFixationInfo()->tiltAngle;
    
    //Calculate pan
    estZ = cvcZ * sqrt((FLOAT)panCam->getTrackingInfo()->width *
                            panCam->getTrackingInfo()->height / cvcArea);
    zoomCam->getFixationInfo()->panAngle = 90.0 -
      (atan( 1.0/ 
        (1.0/( tan( ( 90.0 - panCam->getFixationInfo()->panAngle) *3.14/180 ) ) -
        HZO_CVC_D / estZ ) 
        ) * 180/3.14);
        
    //Fix glitch
    if( zoomCam->getFixationInfo()->panAngle > 90.0 )
    {
      zoomCam->getFixationInfo()->panAngle -= 180.0;
    }
    
    zoomCam->getFixationInfo()->sendPanTilt = true;
        
    #ifdef DEBUG_CVC
    printf( "Correcting angle: estZ: %f P: %f Z: %f (%s @ %d)"CRLF, 
            estZ,
            panCam->getFixationInfo()->panAngle, 
            zoomCam->getFixationInfo()->panAngle, 
            __FILE__, __LINE__ );
    #endif
    
  }  
  else if( panCamInView )
  {
    // See notes pg 8/27-2 for derivation
    FLOAT zoomPanAngle = ( 90.0 - zoomCam->getTrackingReply()->panAngle) * 
      3.14 / 180;
    FLOAT panPanAngle = ( 90.0 - panCam->getTrackingReply()->panAngle) * 
      3.14 / 180;

  
    cvcZ = HZO_CVC_D / 
      ( 1.0 / tan( panPanAngle ) - 1.0 / tan( zoomPanAngle) );
    cvcX = cvcZ / tan( panPanAngle) - HZO_CVC_D;
    cvcArea = (FLOAT)panCam->getTrackingInfo()->width *
                     panCam->getTrackingInfo()->height;
    
    #ifdef DEBUG_CVC                     
    printf( "Z: %f X: %f P: %f Z: %f (%s @ %d)"CRLF, cvcZ, cvcX, 
            panCam->getFixationInfo()->panAngle, 
            zoomCam->getFixationInfo()->panAngle,
            __FILE__, __LINE__ );
    #endif
  }

}

/*****************************************************************
 * SIDualCam::arbitrateDisplay
 *****************************************************************
 */
void SIDualCam::arbitrateDisplay()
{
  IplImage *imgToDisplay;
  
  //Output the appropriate display
  if( !zoomCamInView )
  {
    imgToDisplay = panCam->getZoomedImg();
  }
  else
  {    
    imgToDisplay = zoomCam->getSplicedImg();
  }
  
  //Set the image appropriately
  //but only set to file if option selected
  if( intOptions.vidDstSaveSplicedStream )
  {
    vDstFile.setImg(imgToDisplay );
  }
  vDstScreen.setImg( imgToDisplay );
  
  //Display the image  
  //but only save to file if option selected
  if( intOptions.vidDstSaveSplicedStream )
  {
    vDstFile.sendFrame();
  }
  vDstScreen.sendFrame();
}

/*****************************************************************
 * SIDualCam::saveStats
 *****************************************************************
 */
void SIDualCam::saveStats()
{

  statsLogger.log( 
    (FLOAT)panCam->getTrackingInfo()->xPos / IM_WIDTH,
    (FLOAT)panCam->getTrackingInfo()->yPos / IM_HEIGHT,
    (FLOAT)panCam->getTrackingReply()->kalmanXPos / IM_WIDTH,
    (FLOAT)panCam->getTrackingReply()->kalmanYPos / IM_HEIGHT,
    (FLOAT)panCam->getTrackingReply()->kalmanZPos,
    (FLOAT)panCam->getTrackingInfo()->width,
    (FLOAT)panCam->getTrackingInfo()->height,
    (FLOAT)panCam->getTrackingReply()->panAngle,
    (FLOAT)panCam->getTrackingReply()->tiltAngle,
    panCam->getTrackingReply()->zoomMagOpticalRaw,
    panCam->getTrackingReply()->zoomMagOptical,
    panCam->getTrackingReply()->zoomMagDigital,
     
    (FLOAT)zoomCam->getTrackingInfo()->xPos / IM_WIDTH,
    (FLOAT)zoomCam->getTrackingInfo()->yPos / IM_HEIGHT,
    (FLOAT)zoomCam->getTrackingReply()->kalmanXPos / IM_WIDTH,
    (FLOAT)zoomCam->getTrackingReply()->kalmanYPos / IM_HEIGHT,
    (FLOAT)zoomCam->getTrackingReply()->kalmanZPos,
    (FLOAT)zoomCam->getTrackingInfo()->width,
    (FLOAT)zoomCam->getTrackingInfo()->height,
    (FLOAT)zoomCam->getTrackingReply()->panAngle,
    (FLOAT)zoomCam->getTrackingReply()->tiltAngle,
    zoomCam->getTrackingReply()->zoomMagOpticalRaw,
    zoomCam->getTrackingReply()->zoomMagOptical,
    zoomCam->getTrackingReply()->zoomMagDigital,
     
    zoomCamInView,
    zoomCamInView ? zoomCam->getTrackingReply()->zoomMagOptical :
                    panCam->getTrackingReply()->zoomMagOptical,
    zoomCamInView ? zoomCam->getTrackingReply()->zoomMagDigital :
                    panCam->getTrackingReply()->zoomMagDigital
     );

}

/*****************************************************************
 * SIDualCam::determineState
 *****************************************************************
 */
void SIDualCam::determineState()
{

  //Assume the panoramic camera is fixated on the object
  panCamInView = panCam->getTrackingInfo()->objectFound;

  //Now figure out if the zooming camera is fixated on the object
  zoomCamInView = false;
  if( zoomCam->getTrackingInfo()->objectFound )
  {
    FLOAT panWidthMin = panCam->getTrackingInfo()->width - CAM_PIXEL_ERROR;
    FLOAT panWidthMax = panCam->getTrackingInfo()->width + CAM_PIXEL_ERROR;    
    FLOAT panHeightMin = panCam->getTrackingInfo()->height - CAM_PIXEL_ERROR;
    FLOAT panHeightMax = panCam->getTrackingInfo()->height + CAM_PIXEL_ERROR;
    FLOAT zoomWidthMin = (zoomCam->getTrackingInfo()->width - CAM_PIXEL_ERROR)/
                         zoomCam->getTrackingReply()->zoomMagOptical;
    FLOAT zoomWidthMax = (zoomCam->getTrackingInfo()->width + CAM_PIXEL_ERROR)/
                         zoomCam->getTrackingReply()->zoomMagOptical;
    FLOAT zoomHeightMin = (zoomCam->getTrackingInfo()->height - CAM_PIXEL_ERROR)/
                         zoomCam->getTrackingReply()->zoomMagOptical;
    FLOAT zoomHeightMax = (zoomCam->getTrackingInfo()->height + CAM_PIXEL_ERROR)/
                         zoomCam->getTrackingReply()->zoomMagOptical;
    
    FLOAT panAreaMin = panWidthMin * panHeightMin;                          
    FLOAT panAreaMax = panWidthMax * panHeightMax;                          
    FLOAT zoomAreaMin = zoomWidthMin * zoomHeightMin;                          
    FLOAT zoomAreaMax = zoomWidthMax * zoomHeightMax;                  
    
    if(  
        //Make sure width is within bounds
        ( ( zoomWidthMin <= panWidthMax && zoomWidthMin >= panWidthMin ) ||
          ( zoomWidthMax <= panWidthMax && zoomWidthMax >= panWidthMin ) ||
          ( zoomWidthMin <= panWidthMin && zoomWidthMax >= panWidthMax ) )
        &&  
        //Make sure height is within bounds
        ( ( zoomHeightMin <= panHeightMax && zoomHeightMin >= panHeightMin ) ||
          ( zoomHeightMax <= panHeightMax && zoomHeightMax >= panHeightMin ) ||
          ( zoomHeightMin <= panHeightMin && zoomHeightMax >= panHeightMax ) )
        &&  
        //Make sure area is within bounds
        ( ( zoomAreaMin <= panAreaMax && zoomAreaMin >= panAreaMin ) ||
          ( zoomAreaMax <= panAreaMax && zoomAreaMax >= panAreaMin ) ||
          ( zoomAreaMin <= panAreaMin && zoomAreaMax >= panAreaMax ) )
       )
    {
    zoomCamInView = true;
             
    #ifdef DEBUG_HZO_ARBITRATION_AREA
    
    printf( "Area( Zoom %d min: %6.0f max: %6.0f "
                  "Pan %d min: %6.0f max: %6.0f ) (%s @ %d)" CRLF,
             zoomCamInView, zoomAreaMin, zoomAreaMax, 
             !zoomCamInView, panAreaMin, panAreaMax, 
             __FILE__, __LINE__ );
    #endif
    }
  }
  
  //Deal with consecutive bad frames
  if( zoomCamInView )
  {
    if( consecutiveBadFrames > 0 )
    {
      consecutiveBadFrames--;
    }
    if( consecutiveBadFrames != 0 )
    {
      zoomCamInView = false;
    }
  }
  else
  {
  
    //Note the bad frame
    consecutiveBadFrames++;
    //Stay below the maximum
    if( consecutiveBadFrames >= HZO_MAX_BAD_FRAMES )
    {
      consecutiveBadFrames = HZO_MAX_BAD_FRAMES;
    }
  }
  
}
// File: $Id: SIDualCam.cpp,v 1.22 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SIDualCam.cpp,v $
// Revision 1.22  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.21  2005/09/10 02:42:48  edn2065
// Added minimum zoom option. Added timer to apply in menu.
//
// Revision 1.20  2005/09/10 01:48:01  edn2065
// Expanded MFS again to include final image stats
//
// Revision 1.19  2005/09/10 00:35:36  edn2065
// synchronized video streams to file (kind of)
//
// ----------------------------------------------------------------------
//

