
#ifndef VIDEODESTINATIONSCREENTHREAD_H
#define VIDEODESTINATIONSCREENTHREAD_H

#include <string.h>
#include "cv.h"
#include "Thread.h"
#include "Semaphore.h"
#include "types.h"

/**************************************************************
 *
 * VideoDestinationScreenThread class
 * Description: Outputs video to an openCV screen.
 **************************************************************
 */
class VideoDestinationScreenThread : public Thread{

  public:
    /******************************************
    * ctor
    * Description: Sets up OpenCV's highgui
    ******************************************
    */
    VideoDestinationScreenThread();

   /******************************************
    * dtor
    ******************************************
    */
    ~VideoDestinationScreenThread();

   /******************************************
    * createDisplay
    * Description: Creates a display for video
    * Parameters:  title - The title of the window
    * Returns:     The screen number for this window
    ******************************************
    */
    UINT8 createDisplay( CHAR *title );   
    
   /******************************************
    * sendFrame
    * Description: Displays a frame to the screen
    * Parameters:  imgToSend - the image to be displayed
    *              screenNum - the screen number representing
    *                          the target window
    ******************************************
    */
    void sendFrame(IplImage *imgToSend, UINT8 screenNum);
   
  protected:
    virtual void execute(void *);
  private:
    //The handle of the openCV screen
    CHAR *displayTitles[VID_DST_MAX_DISPLAYS];
    
    //The images to be displayed
    IplImage *displayImages[VID_DST_MAX_DISPLAYS];
    
    //True if a new image has been received
    BOOL newImage[VID_DST_MAX_DISPLAYS];
    
    //Semaphores to protect images
    Semaphore displaySems[VID_DST_MAX_DISPLAYS];
    
    //The number of displays ready to be open
    UINT8 numDisplays;
    //The number of displays open
    UINT8 numInitializedDisplays;       
    //Synchronizes the number of displays created
    Semaphore createDisplaySem;
    
    BOOL executeThread;

};

#endif

// File: $Id: VideoDestinationScreenThread.h,v 1.1 2005/08/24 19:44:29 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationScreenThread.h,v $
// Revision 1.1  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.4  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
//
// Revision 1.3  2005/05/24 15:37:17  edn2065
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.2  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.1  2005/04/02 16:49:19  edn2065
// Added to repository
//
// ----------------------------------------------------------------------
//
