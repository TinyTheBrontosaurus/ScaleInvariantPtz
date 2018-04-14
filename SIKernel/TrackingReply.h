#ifndef TRACKINGREPLY_H
#define TRACKINGREPLY_H

#include "types.h"

/**************************************************************
 *
 * TrackingReply struct
 * Description: The various comms associated with tracking/fixating from the
 *              SICameraComms to the SIKernel classes.
 **************************************************************
 */
typedef struct 
{
  //Pan/tilt angles read from camera
  FLOAT panAngle;
  FLOAT tiltAngle;
  
  //Zoom magnification read from camera. Total = optical * digital
  FLOAT zoomMagDigital;
  FLOAT zoomMagOptical;
  UINT16 zoomMagOpticalRaw;    
  
  //PPD for this frame
  FLOAT pixelsPerDegree;
  //The x position as estimated by the kalman filter
  FLOAT kalmanXPos;
  FLOAT kalmanYPos;
  FLOAT kalmanZPos;

} TrackingReply;


#endif


// File: $Id: TrackingReply.h,v 1.4 2005/09/09 01:24:37 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: TrackingReply.h,v $
// Revision 1.4  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.3  2005/09/07 01:05:16  edn2065
// Finished CVC implementation
//
// Revision 1.2  2005/06/14 12:19:58  edn2065
// Made PT inquiry a float instead of int. Camera resolution was being thrown away
//
// Revision 1.1  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
//
