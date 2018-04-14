#include "ZoomOptical.h"
extern "C" {
#include "CommandLevel.h"
}
#include <stdio.h>
#include "Time.h"

//Define and initialize static variables
UINT8 ZoomOptical::totalNumberOfCameras = 0;

//Create the default lookup table
#define ZOOMX0 0  
#define ZOOMX1 3693
#define ZOOMX2 6286
#define ZOOMX3 9479
#define ZOOMX4 11138
#define ZOOMX5 12592
#define ZOOMX6 13614
#define ZOOMX7 14429
#define ZOOMX8 15176
#define ZOOMX9 16384
#define ZOOM_TABLE_LENGTH 10
#define ZOOM_TABLE_MAX 10
#define ZOOM_TABLE_MIN 1
const FLOAT defaultRdgToMagLookupTbl[] = {ZOOMX0, ZOOMX1, 
                                      ZOOMX2, ZOOMX3,
                                      ZOOMX4, ZOOMX5,
                                      ZOOMX6, ZOOMX7,
                                      ZOOMX8, ZOOMX9};

#define SYNC 1
#define ASYNC 0


//More constants
#define CAM_MIN_ZOOM_SPEED    0
#define CAM_MAX_ZOOM_SPEED    7

#define CAM_TELE_STD  3
#define CAM_WIDE_STD  4
#define CAM_TELE_VAR  5
#define CAM_WIDE_VAR  6
#define CAM_DIRECT    7
#define CAM_DZOOM_ON  8
#define CAM_DZOOM_OFF 9

/*****************************************************************
 * ZoomOptical ctor
 *****************************************************************
 */
ZoomOptical::ZoomOptical()
{
  //Make all asynchronous
  sync = ASYNC;
  
  //Set the default lookup table. 
  rdgToMagTbl.load( defaultRdgToMagLookupTbl, ZOOM_TABLE_LENGTH, 
                     ZOOM_TABLE_MIN, ZOOM_TABLE_MAX, false );
}

/*****************************************************************
 * ZoomOptical dtor
 *****************************************************************
 */
ZoomOptical::~ZoomOptical() 
{
}


/*****************************************************************
 * ZoomOptical::init
 *****************************************************************
 */
void ZoomOptical::init(UINT8 myCameraNumber, CameraTxThread *myCameraTxThread, 
      CameraRxThread *myCameraRxThread, CommandLevel *myCmdLvl) 
{
  commandLevel = myCmdLvl;
  cameraNumber = myCameraNumber;
  commandLevel->CAM_Zoom(cameraNumber, SYNC, CAM_DZOOM_OFF);  // turn off digital zoom
  
  cameraTxThread = myCameraTxThread;
  cameraRxThread = myCameraRxThread;
  reset();
  
  
}

void ZoomOptical::init( UINT8 myCameraNumber )
{
  printf( "ZoomOptical::init( UINT8 ) obsolete (%s @ %d)" CRLF, __FILE__, __LINE__ );
  
  
}

/*****************************************************************
 * ZoomOptical::setLookup
 *****************************************************************
 */
void ZoomOptical::setLookup(const FLOAT *myTable, 
                        const UINT32 myLength, FLOAT min, FLOAT max,
                        BOOL myDoForward) 
{
  //Set the lookup table
  rdgToMagTbl.load(myTable, myLength, min, max, myDoForward);
}

/*****************************************************************
 * ZoomOptical::setLookup
 *****************************************************************
 */
void ZoomOptical::setLookup(const CHAR *fileName ) 
{
  //Set the lookup table
  rdgToMagTbl.load(fileName);
}

/*****************************************************************
 * ZoomOptical::saveLookup
 *****************************************************************
 */
void ZoomOptical::saveLookup(const CHAR *fileName ) 
{
  //Save the lookup table
  rdgToMagTbl.save(fileName);
}

/*****************************************************************
 * ZoomOptical::zoomRaw
 *****************************************************************
 */
void ZoomOptical::zoomRaw( UINT16 magRaw )
{
  // Send the command
  cameraTxThread->setZMag( magRaw );
}

/*****************************************************************
 * ZoomOptical::inquiryRaw
 *****************************************************************
 */
BOOL ZoomOptical::inquiryRaw( UINT16 *myZoomMag )
{    
  UINT32 timestamp;

  return cameraRxThread->getZMag( myZoomMag, &timestamp );
}

/*****************************************************************
 * ZoomOptical::zoom
 *****************************************************************
 */
void ZoomOptical::zoom( FLOAT magnification ) 
{

  //Clip the magnification
  if( magnification > 10.0 )
  {
    magnification = 10.0;
  }
  else if( magnification < 1.0 )
  {
    magnification = 1.0;
  }
  // Send the command
  zoomRaw( (UINT16)rdgToMagTbl.lookupReverse(magnification) );     
}

/*****************************************************************
 * ZoomOptical::inquiry
 *****************************************************************
 */
BOOL ZoomOptical::inquiry( FLOAT *myZoomMag )
{
  BOOL successful = false;
  UINT16 rawZoomMag;
  
  successful = inquiryRaw(&rawZoomMag);
  
  //Get the zoom setting
  *myZoomMag = rdgToMagTbl.lookup((FLOAT)rawZoomMag);
  
  return successful;
}

/*****************************************************************
 * ZoomOptical::inquiryBoth
 *****************************************************************
 */
BOOL ZoomOptical::inquiryBoth(UINT16 *rawMag, FLOAT *mag)
{
  BOOL successful = false;
  
  successful = inquiryRaw(rawMag);
  
  //Get the zoom setting
  if( successful )
  {
    *mag = rdgToMagTbl.lookup((FLOAT)*rawMag);
  }
  
  return successful;  
  
}

/*****************************************************************
 * ZoomOptical::inquiryAll
 *****************************************************************
 */
BOOL ZoomOptical::inquiryAll(FLOAT *pan, FLOAT *tilt, UINT16 *rawMag, FLOAT *mag)
{
  BOOL success = false;
  
  printf( "ZoomOptical::inquiryAll obsolete (%s @ %d)" CRLF, __FILE__, __LINE__ );
  
  return success;
}    

/*****************************************************************
 * ZoomOptical::reset
 *****************************************************************
 */
void ZoomOptical::reset()
{
  // Send the command
  zoomRaw(0); 
}


/*
 * ZoomOptical.cpp
 *
 * Description: Zoom Optical class implementation.
 *              This class is in charge of zooming
 *              the Sony EVI-D100 camera
 *
 * Authors: Eric D Nelson
 *
 * Version:
 *     $Id: ZoomOptical.cpp,v 1.15 2005/08/25 22:12:23 edn2065 Exp $
 *
 * Revisions:
 *     $Log: ZoomOptical.cpp,v $
 *     Revision 1.15  2005/08/25 22:12:23  edn2065
 *     Made commandLevel a class. Added HZO pickoff point in SIKernel
 *
 *     Revision 1.14  2005/07/26 18:46:43  edn2065
 *     Added scale invariance
 *
 *     Revision 1.13  2005/07/15 01:08:58  edn2065
 *     Integrating RxTxThread. Still has jerkiness bug
 *
 *     Revision 1.12  2005/06/28 20:01:49  edn2065
 *     Debugging comms bugs. overnight check in
 *
 *     Revision 1.11  2005/06/22 16:57:40  edn2065
 *     Tied pantilt and zoom readings together to make them faster
 *
 *     Revision 1.10  2005/06/22 15:52:18  edn2065
 *     Created a new function that inquires both pan tilt and zoom in a single instruction
 *
 *     Revision 1.9  2005/06/21 14:56:15  edn2065
 *     Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
 *
 *     Revision 1.8  2005/06/13 21:04:19  edn2065
 *     Implemented fixating. Needs to be fine tuned
 *
 *     Revision 1.7  2005/06/13 18:47:14  edn2065
 *     Calibration implemented without principle point finder.
 *
 *     Revision 1.6  2005/06/10 11:55:51  edn2065
 *     Fixed reverseLookup error. Added reset option
 *
 *     Revision 1.5  2005/06/09 13:48:03  edn2065
 *     Fixed synchronization errors caused by not initializing Barrier in SIKernal
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
 */
