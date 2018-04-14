#ifndef FIXATIONINFO_H
#define FIXATIONINFO_H

#include "types.h"


typedef enum 
{

  //Don't send a command
  ZOOM_CMD_NONE,
  //Send the zoom command using the raw value
  ZOOM_CMD_RAW,
  //Send the zoom command using the lookup table
  ZOOM_CMD_MAG

}ZoomCommandType;

/**************************************************************
 *
 * FixationInfo struct
 * Description: The internal communication within the SICameraComms thread
 *              that tells what commands should be sent to the camera.
 **************************************************************
 */
typedef struct 
{
  //True if the panAngle and tiltAngle should be sent to the camera in this iteration
  BOOL sendPanTilt;
  FLOAT panAngle;
  FLOAT tiltAngle;
  
  //True if the zoomAngle should be sent to the camera in this iteration
  ZoomCommandType sendZoom;
  FLOAT  zoomMagDigital;
  FLOAT  zoomMagOptical;
  UINT16 zoomRaw;

} FixationInfo;

#endif


// File: $Id: FixationInfo.h,v 1.2 2005/08/18 03:07:22 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: FixationInfo.h,v $
// Revision 1.2  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.1  2005/07/14 20:23:19  edn2065
// Adding FixationInfo to repository
//
// Revision 1.1  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
//
