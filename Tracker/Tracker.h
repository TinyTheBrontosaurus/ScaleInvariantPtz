#ifndef TRACKER_H
#define TRACKER_H

#include "types.h"
#include "cv.h"


/**************************************************************
 *
 * Tracker class (abstract)
 * Description: An interface for tracking objects in video.
 *
 *
 **************************************************************
 */
class Tracker {
 
  public:
   
   
   /****************************************************
    * track
    * Description: Tracks an object in video
    * Parameters:  frame - The current frame in the video
    *              numROI - The number of valid roi
    *              roi - Possible candidates for the object given
    *                    the current frame and all previous frames.
    *                    Make sure this array is of ample size.
    *******************************************************
    */       
    virtual void track( IplImage *frame, UINT8 *numROI, CvRect *roi ) = 0;
  
};
#endif

/* 
 * Author: Eric D Nelson
 *
 * Version:
 *     $Id: Tracker.h,v 1.2 2005/05/26 14:08:11 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Tracker.h,v $
 *     Revision 1.2  2005/05/26 14:08:11  edn2065
 *     Tracker compiles, but doesnt build because of execute in Menu.h
 *
 *     Revision 1.1  2005/05/26 13:34:49  edn2065
 *     Added Tracker.h and finished changing names.
 *
 *
 */
