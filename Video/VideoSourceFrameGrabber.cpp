#include <stdio.h>
#include "VideoSourceFrameGrabber.h"

/*****************************************************************
 * VideoSourceFrameGrabber ctor
 *****************************************************************
 */
VideoSourceFrameGrabber::VideoSourceFrameGrabber()
{
  grabbedImage = NEW_IMAGE;
  //Save image data pointer
  oldGrabbedImageData = grabbedImage->imageData;
}

/*****************************************************************
 * VideoSourceFrameGrabber dtor
 *****************************************************************
 */
VideoSourceFrameGrabber::~VideoSourceFrameGrabber()
{
  vidCapture.stopCapture();
  //Restore image data pointer
  grabbedImage->imageData = oldGrabbedImageData;
  cvReleaseImage(&grabbedImage);
}

/*****************************************************************
 * VideoSourceFrameGrabber::setPath
 *****************************************************************
 */
void VideoSourceFrameGrabber::setPath( CHAR *myPath )
{
  vidCapture.startCapture( myPath, V4LC_SVIDEO);    
}

/*****************************************************************
 * VideoSourceFrameGrabber::getFrame
 *****************************************************************
 */
BOOL VideoSourceFrameGrabber::getFrame(IplImage *newImg)
{
 
  //Get the image pointer  
  grabbedImage->imageData = vidCapture.readFrame();
        
  //Copy the image
  cvCopyImage( grabbedImage, newImg);
  
  return vidCapture.isRunning();
}


// File: $Id: VideoSourceFrameGrabber.cpp,v 1.9 2005/08/09 23:32:34 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: VideoSourceFrameGrabber.cpp,v $
// Revision 1.9  2005/08/09 23:32:34  edn2065
// Turned v4lcapture into a class
//
// Revision 1.8  2005/06/09 15:36:28  edn2065
// Changed input to S-Video
//
// Revision 1.7  2005/05/24 15:37:17  edn2065
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.6  2005/04/25 16:13:22  edn2065
// Tested Video subsystem in lab. Changed cvCopyImage calls to be (src, dst) instead of (dst, src). Inverted boolean getFrame() in test.cpp
//
// Revision 1.5  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.4  2005/04/06 21:21:09  edn2065
// Have number of bugs with threads. Revamping GUI to VideoSourceFile call
//
// Revision 1.3  2005/04/02 18:42:32  edn2065
// hardcoded width and height
//
// Revision 1.2  2005/04/02 15:38:46  edn2065
// Entering into CVS
//
// ----------------------------------------------------------------------
//
