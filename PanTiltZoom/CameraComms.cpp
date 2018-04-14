#include "CameraComms.h"
extern "C" {
#include "CommandLevel.h"
}
#include <unistd.h>
#include <stdio.h>

//Define and initialize static variables
UINT8 CameraComms::totalNumberOfCameras = 0;


/*****************************************************************
 * CameraComms ctor
 *****************************************************************
 */
CameraComms::CameraComms()
{
  //Camera is not yet set up
  initialized = false;
  
  //Set this camera number
  totalNumberOfCameras++;
  //This assumes that cameras are not daisy chained, but rather have separate 
  //connections to PC.
  cameraNumber = 1;//totalNumberOfCameras;
  
}

/*****************************************************************
 * CameraComms dtor
 *****************************************************************
 */
CameraComms::~CameraComms() 
{
  //Only do this if init() has been called
  if( initialized )
  {
    commandLevel.setFd(myFd);
    //Clean up the camera
    commandLevel.SetManual(cameraNumber);
    //Turn off camera
    commandLevel.CAM_Power(cameraNumber, ASYNC, OFF);
    commandLevel.ResetThePort();
  }
}

/*****************************************************************
 * CameraComms::init
 *****************************************************************
 */
SINT8 CameraComms::init(CHAR *mySerialPort) 
{
  SINT8 retval = -1;
  if( !initialized )
  {
    BOOL success = true;
    BOOL latest;
    //Make sure this only gets initialized once
    initialized = true;
    
    latest = commandLevel.Init(mySerialPort,cameraNumber) >= 0;
    // Initialize serial port
    myFd = commandLevel.getFd();
    success = latest && success;       
    
    /*if( CAM_PowerInq(cameraNumber) == ON )
    {
      //Turn offcamera
      latest = CAM_Power(cameraNumber, ASYNC, OFF) >= 0;
      if( !latest )
      {
        printf( "Failed Power off (%s @ %d)"CRLF, __FILE__, __LINE__ );
      }
      success = latest && success;      
      //Give it time to turn off
      sleep(3);   
    }*/
     
    
    //Turn on camera
    latest = commandLevel.CAM_Power(cameraNumber, ASYNC, ON) >= 0;
    if( !latest )
    {
      printf( "Failed Power on (%s @ %d)"CRLF, __FILE__, __LINE__ );
    }
    
    sleep(5);
    
    success = latest && success;
    //Set control of the camera
    latest = commandLevel.SetAuto(cameraNumber) >= 0;
    if( !latest )
    {
      printf( "Failed Auto (%s @ %d)"CRLF, __FILE__, __LINE__ );
    }
    
    latest = commandLevel.CAM_WB(cameraNumber,1,Outdoor);
    success = latest && success;      
    //latest = CAM_ExpComp(cameraNumber,1,OFF);
    //latest = CAM_SlowShutter(cameraNumber,1,Manual);
    //latest = CAM_Focus(cameraNumber,1,ManualFocus);
    /*for( int i = 0; i < 1000; i++ )
    {
      printf( "Shutter: %4d  Iris: %4d  Gain: %4d (%s @ %d)" CRLF,
        CAM_ShutterPosInq(cameraNumber),
        CAM_IrisPosInq(cameraNumber), CAM_GainPosInq(cameraNumber),
            __FILE__, __LINE__ );
    }*/
    latest = commandLevel.CAM_AE(cameraNumber,1,Manual);
    success = latest && success; 
    latest = commandLevel.CAM_Shutter( cameraNumber, 1, Direct, CAM_SHUTTER );
    success = latest && success; 
    latest = commandLevel.CAM_Iris( cameraNumber, 1, Direct, CAM_IRIS );
    success = latest && success; 
    latest = commandLevel.CAM_Gain( cameraNumber, 1, Direct, CAM_GAIN );        
    success = latest && success; 
    
    retval = success ? 0 : -1;
    //sleep(1);

  }  
  return retval;
}

/*****************************************************************
 * CameraComms::getCameraNumber
 *****************************************************************
 */
UINT8 CameraComms::getCameraNumber() 
{
  return cameraNumber;
}

/*
 * CameraComms.cpp
 *
 * Authors: Eric D Nelson
 *
 * Version:
 *     $Id: CameraComms.cpp,v 1.14 2005/08/25 22:12:23 edn2065 Exp $
 *
 * Revisions:
 *     $Log: CameraComms.cpp,v $
 *     Revision 1.14  2005/08/25 22:12:23  edn2065
 *     Made commandLevel a class. Added HZO pickoff point in SIKernel
 *
 *     Revision 1.13  2005/08/24 20:06:29  edn2065
 *     Put in band-aid for camera communications
 *
 *     Revision 1.12  2005/08/03 01:43:26  edn2065
 *     Implemented focal length selection
 *
 *     Revision 1.11  2005/07/28 20:57:49  edn2065
 *     Kalman filter working
 *
 *     Revision 1.10  2005/07/15 01:08:58  edn2065
 *     Integrating RxTxThread. Still has jerkiness bug
 *
 *     Revision 1.9  2005/07/14 20:16:00  edn2065
 *     Added commenting to RxTxThread
 *
 *     Revision 1.8  2005/07/01 16:43:05  edn2065
 *     Created sine wave generator for camera
 *
 *     Revision 1.7  2005/06/28 20:01:49  edn2065
 *     Debugging comms bugs. overnight check in
 *
 *     Revision 1.6  2005/06/27 18:49:55  edn2065
 *     added auto bps checking to amishs code
 *
 *     Revision 1.5  2005/06/22 19:48:16  edn2065
 *     Weekend commit
 *
 *     Revision 1.4  2005/06/09 13:48:03  edn2065
 *     Fixed synchronization errors caused by not initializing Barrier in SIKernal
 *
 *     Revision 1.3  2005/06/08 15:08:00  edn2065
 *     Add P/T/Z inquiry
 *
 *     Revision 1.2  2005/06/08 13:44:18  edn2065
 *     Overnight checkin
 *
 *     Revision 1.1  2005/06/02 16:41:26  edn2065
 *     Added CameraComms
 *
 *
 */
