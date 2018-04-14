
#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "cv.h"
#include "types.h"

#define VS_TEXT_LENGTH (100)

/**************************************************************
 *
 * VideoSource class (abstract)
 * Description: An interface for inputting images from video.
 **************************************************************
 */
class VideoSource
{
  public:
   /******************************************
    * getFrame
    * Description: Retrieves an image from the appropriate source.
    * Parameters:  newImg - Where the new image should be saved
    * Returns:     True if successful
    ******************************************
    */
    virtual BOOL getFrame(IplImage *newImg) = 0;

 protected:

 private:

};

#endif

// File: $Id: VideoSource.h,v 1.4 2005/04/23 14:26:32 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: VideoSource.h,v $
// Revision 1.4  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.3  2005/04/02 15:38:46  edn2065
// Entering into CVS
//
// Revision 1.2  2005/04/02 13:59:21  edn2065
// Added comment about getFrame()
//
// Revision 1.1  2005/04/02 13:49:28  edn2065
// Adding to repository
//
// Revision 1.1  2005/04/02 12:52:13  edn2065
// Adding file
// ----------------------------------------------------------------------
//
