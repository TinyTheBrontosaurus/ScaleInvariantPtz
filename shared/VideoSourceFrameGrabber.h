
#ifndef VIDEOSOURCEFRAMEGRABBER_H
#define VIDEOSOURCEFRAMEGRABBER_H

#include <string.h>
#include "VideoSource.h"
#include "V4l2Capture.h"

/**************************************************************
 *
 * VideoSourceFrameGrabber class
 * Description: Retrieves video from a frame grabber.
 **************************************************************
 */
class VideoSourceFrameGrabber : public VideoSource {

 public:

 /******************************************
  * ctor
  * Description: Sets defaults.
  ******************************************
  */
  VideoSourceFrameGrabber();

 /******************************************
  * dtor
  ******************************************
  */
  ~VideoSourceFrameGrabber();

 /******************************************
  * setPath
  * Description: Sets the path of the frame grabber.
  * Parameters:  path - The path of the frame grabber
  *                     (for example "/dev/v4l/video0")
  ******************************************
  */
  void setPath( CHAR *path );

 /******************************************
  * getFrame
  * Description: Retrieves an image from the a frame grabber.
  * Parameters:  newImg - Where the new image should be saved
  * Returns:     True if successful
  ******************************************
  */
  virtual BOOL getFrame(IplImage *newImg);

 protected:

 private:

   //An image pointer for the grabbed image
   IplImage *grabbedImage;

   //Pointer to grabbedImage original image data location
   CHAR *oldGrabbedImageData;
   
   //The video capture object
   V4l2Capture vidCapture;
};

#endif

// File: $Id: VideoSourceFrameGrabber.h,v 1.5 2005/08/09 23:32:34 edn2065 Exp $
// Author: Eric D Nelson
// Description: Frame grabber
// Revisions:
// $Log: VideoSourceFrameGrabber.h,v $
// Revision 1.5  2005/08/09 23:32:34  edn2065
// Turned v4lcapture into a class
//
// Revision 1.4  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.3  2005/04/02 18:42:34  edn2065
// hardcoded width and height
//
// Revision 1.2  2005/04/02 15:38:46  edn2065
// Entering into CVS
//
// Revision 1.1  2005/04/02 14:25:58  edn2065
// Commiting files for first time
//
// ----------------------------------------------------------------------
//
