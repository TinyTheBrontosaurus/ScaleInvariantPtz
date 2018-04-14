
#ifndef VIDEODESTINATIONSCREEN_H
#define VIDEODESTINATIONSCREEN_H

#include <string.h>
#include "cv.h"
#include "VideoDestination.h"
#include "VideoDestinationScreenThread.h"
#include "types.h"

/**************************************************************
 *
 * VideoDestinationScreen class
 * Description: Outputs video to an openCV screen.
 **************************************************************
 */
class VideoDestinationScreen : public VideoDestination {

  public:
    /******************************************
    * ctor
    * Description: Creates openCV screen and sets defaults
    ******************************************
    */
    VideoDestinationScreen();
    VideoDestinationScreen(CHAR *);

   /******************************************
    * dtor
    ******************************************
    */
    ~VideoDestinationScreen();

   /******************************************
    * destroyDisplay
    * Description: Destroys the display. Same as destructor but for
    *              when the object is static.
    ******************************************
    */
    void destroyDisplay();    
    
   /******************************************
    * sendFrame
    * Description: Sends the image to the openCV screen.
    ******************************************
    */
    virtual void sendFrame();
   
   /******************************************
    * setPreventLine
    * Description: Sets the value of preventLine accordingly
    * Parameters: myPreventLine - the new value of preventLine
    ******************************************
    */     
    void setPreventLine( BOOL );

  protected:
  private:
    //The handle of the openCV screen
    CHAR title[VD_TEXT_LENGTH];
    
    //Whether to get rid of the horizontal line appearing on the screen
    //or not.
    BOOL preventLine;
    
    UINT8 displayHandle;
    
    static UINT8 numInstances;
    
    static VideoDestinationScreenThread videoDestinationScreenThread;    

};

#endif

// File: $Id: VideoDestinationScreen.h,v 1.6 2005/08/25 22:12:23 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationScreen.h,v $
// Revision 1.6  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
// Revision 1.5  2005/08/24 19:44:29  edn2065
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
