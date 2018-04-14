#ifndef TRACKINGINFO_H
#define TRACKINGINFO_H

#include "types.h"

/**************************************************************
 *
 * TrackingInfo struct
 * Description: The various comms associated with tracking/fixating from the
 *              SIKernel to the SICameraComms classes.
 **************************************************************
 */
typedef struct 
{
  
  //True if the end of the stream has been reached, meaning this
  //frame cannot be processed.
  BOOL badStream;   
  
  //True if the cameras should be reset
  BOOL reset;
  
  //Tells the camera whether to do a step or pulse command for pan/tilt/zoom
  CommandType panTiltCommand; 
  CommandType zoomCommand;
  
  //TRUE if an object was found in this iteration
  BOOL objectFound;
  
  //The size and position of the tracked object
  UINT16 width;
  UINT16 height;
  FLOAT xPos;
  FLOAT yPos;
  //The ROI with the corresponding size and position values
  CvRect *roi;
  
  
} TrackingInfo;

#endif


// File: $Id: TrackingInfo.h,v 1.6 2005/06/21 14:09:07 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: TrackingInfo.h,v $
// Revision 1.6  2005/06/21 14:09:07  edn2065
// Added PartialBox principal point algorithm to kernel. Early tests work.
//
// Revision 1.5  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.4  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.3  2005/06/10 11:55:51  edn2065
// Fixed reverseLookup error. Added reset option
//
// Revision 1.2  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.1  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
//
