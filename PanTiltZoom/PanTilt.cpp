#include "PanTilt.h"
extern "C" {
#include "CommandLevel.h"
}
#include <stdio.h>
#include "Time.h"

/*****************************************************************
 * PanTilt ctor
 *****************************************************************
 */
PanTilt::PanTilt()
{
 
  //Make all asynchronous
  sync = ASYNC;
}

/*****************************************************************
 * PanTilt dtor
 *****************************************************************
 */
PanTilt::~PanTilt() 
{
}

/*****************************************************************
 * PanTilt::init
 *****************************************************************
 */
void PanTilt::init( UINT8 myCameraNumber, CameraTxThread *myCameraTxThread, 
      CameraRxThread *myCameraRxThread )
{
  cameraNumber = myCameraNumber;
  cameraTxThread = myCameraTxThread;
  cameraRxThread = myCameraRxThread;
  reset();
  
  
}

void PanTilt::init( UINT8 myCameraNumber )
{
  printf( "PanTilt::init( UINT8 ) obsolete (%s @ %d)" CRLF, __FILE__, __LINE__ );
 
}

/*****************************************************************
 * PanTilt::move
 *****************************************************************
 */
SINT8 PanTilt::move( FLOAT panPos,  FLOAT panSpeed, 
                   FLOAT tiltPos, FLOAT tiltSpeed) 
{
  //Make sure the inputs are legal
  //for panPos
  if( panPos < CAM_MIN_PAN_POS )
  {
    panPos = CAM_MIN_PAN_POS;
  }
  else if( panPos > CAM_MAX_PAN_POS )
  {
    panPos = CAM_MAX_PAN_POS;
  }
  
  //for panSpeed
  if( panSpeed < CAM_MIN_PAN_SPEED )
  {
    panSpeed = CAM_MIN_PAN_SPEED;
  }
  else if( panSpeed > CAM_MAX_PAN_SPEED )
  {
    panSpeed = CAM_MAX_PAN_SPEED;
  }
  
  //for tiltPos
  if( tiltPos < CAM_MIN_TILT_POS )
  {
    tiltPos = CAM_MIN_TILT_POS;
  }
  else if( tiltPos > CAM_MAX_TILT_POS )
  {
    tiltPos = CAM_MAX_TILT_POS;
  }
  
  //for tiltSpeed
  if( tiltSpeed < CAM_MIN_TILT_SPEED )
  {
    tiltSpeed = CAM_MIN_TILT_SPEED;
  }
  else if( tiltSpeed > CAM_MAX_TILT_SPEED )
  {
    tiltSpeed = CAM_MAX_TILT_SPEED;
  }
    
  printf( "PanTilt::move(float float float float) function obsolete (%s @ %d)" CRLF,
    __FILE__, __LINE__);
  
  return -1;
}

/*****************************************************************
 * PanTilt::move
 *****************************************************************
 */
SINT8 PanTilt::move( FLOAT panPos, FLOAT tiltPos ) 
{
  //Make sure the inputs are legal
  //for panPos
  if( panPos < CAM_MIN_PAN_POS )
  {
    panPos = CAM_MIN_PAN_POS;
  }
  else if( panPos > CAM_MAX_PAN_POS )
  {
    panPos = CAM_MAX_PAN_POS;
  }
  
  //for tiltPos
  if( tiltPos < CAM_MIN_TILT_POS )
  {
    tiltPos = CAM_MIN_TILT_POS;
  }
  else if( tiltPos > CAM_MAX_TILT_POS )
  {
    tiltPos = CAM_MAX_TILT_POS;
  }
  cameraTxThread->setPTAngle(panPos, tiltPos);
  
  return 0;
}

/*****************************************************************
 * PanTilt::reset
 *****************************************************************
 */
SINT8 PanTilt::reset()
{
  move(0.0, 0.0);
  return 0;
}

/*****************************************************************
 * PanTilt::getPanTilt
 *****************************************************************
 */
BOOL PanTilt::getPanTilt(FLOAT *pan, FLOAT *tilt) 
{

  BOOL successful;
  UINT32 timestamp;
  
  successful = cameraRxThread->getPTAngle( pan, tilt, &timestamp );
   
  return successful;
    
}


/*
 * PanTilt.cpp
 *
 * Description: Pan Tilt class implementation.
 *              This class is in charge of panning and tilting
 *              the Sony EVI-D100 camera
 *
 * Authors: John Ruppert
 *          Jared Holsopple
 *          Justin Hnatow
 *          Eric D Nelson
 *
 * Version:
 *     $Id: PanTilt.cpp,v 1.12 2005/07/28 01:42:18 edn2065 Exp $
 *
 * Revisions:
 *     $Log: PanTilt.cpp,v $
 *     Revision 1.12  2005/07/28 01:42:18  edn2065
 *     Made it so fixation was only calculated when p/t/z values were available
 *
 *     Revision 1.11  2005/07/15 01:08:58  edn2065
 *     Integrating RxTxThread. Still has jerkiness bug
 *
 *     Revision 1.10  2005/06/22 15:52:18  edn2065
 *     Created a new function that inquires both pan tilt and zoom in a single instruction
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
 *     Revision 1.5  2005/06/02 15:58:44  edn2065
 *     Getting ready to create CameraComms class
 *
 *     Revision 1.4  2005/06/01 17:40:17  edn2065
 *     PanTilt tested
 *
 *     Revision 1.3  2005/06/01 16:25:44  edn2065
 *     Worked out compilation issues
 *
 *     Revision 1.2  2005/06/01 15:15:39  edn2065
 *     Altered to include just Pan/Tilt func.
 *
 *     Revision 1.1  2005/06/01 13:37:58  edn2065
 *     Copied from camera.h and .cpp from JJJ
 *
 *     Revision 1.10  2005/03/11 21:27:32  edn2065
 *     Works like before but now with LookupTable
 *
 *     Revision 1.9  2005/03/11 20:46:21  edn2065
 *     Reverting to non reverse lookup table
 *
 *     Revision 1.8  2005/03/11 17:42:00  edn2065
 *     Check in before adding lookup table
 *
 *     Revision 1.7  2005/02/28 23:16:33  edn2065
 *     Measures magnification
 *
 *     Revision 1.6  2005/02/05 19:10:54  jdh5443
 *     Changed back to a lookup table for panTiltXY
 *
 *     Revision 1.5  2005/02/01 16:57:04  jdh5443
 *     Changed to do a liner mapping between zoom level and pixels per degree
 *
 *     Revision 1.4  2005/01/18 21:47:53  jdh5443
 *     No changes
 *
 *     Revision 1.3  2005/01/14 04:38:47  jdh5443
 *     Updated camera class with new functions and took out capture
 *
 *     Revision 1.2  2004/12/22 16:50:53  jar4773
 *     Added panTiltRelPos function
 *
 *     Revision 1.1  2004/12/22 16:10:18  jar4773
 *     Initial revision
 *
 *
 */
