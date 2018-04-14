#include <stdio.h>
#include "VideoDestinationScreen.h"
#include "highgui.h"

UINT8 VideoDestinationScreen::numInstances = 0;
VideoDestinationScreenThread VideoDestinationScreen::videoDestinationScreenThread;    

/*****************************************************************
 * VideoDestinationScreen ctor
 *****************************************************************
 */
VideoDestinationScreen::VideoDestinationScreen() : preventLine( false )
{
  numInstances++;
  
  sprintf( title,
    "Scale Invariant Tracker GUI %d v%s (Compiled %s @ %s)",
    numInstances, VERSION, __DATE__, __TIME__ ); 

  displayHandle = videoDestinationScreenThread.createDisplay( title );
}

/*****************************************************************
 * VideoDestinationScreen ctor
 *****************************************************************
 */
VideoDestinationScreen::VideoDestinationScreen(CHAR *title) : preventLine( false )
{
  numInstances++;
  
  displayHandle = videoDestinationScreenThread.createDisplay( title );
}

/*****************************************************************
 * VideoDestinationFile dtor
 *****************************************************************
 */
VideoDestinationScreen::~VideoDestinationScreen()
{
}

/*****************************************************************
 * VideoDestinationFile::destroyDisplay
 *****************************************************************
 */
void VideoDestinationScreen::destroyDisplay()
{
  printf( "Function obsolete (%s @ %d)"CRLF, __FILE__, __LINE__ );
}


/*****************************************************************
 * VideoDestinationScreen::sendFrame
 *****************************************************************
 */
void VideoDestinationScreen::sendFrame()
{

  //Place text, boxes, etc.
  prepareImage();

  videoDestinationScreenThread.sendFrame( imgToDisp, displayHandle );

}

/*****************************************************************
 * VideoDestinationScreen::setPreventLine
 *****************************************************************
 */
void VideoDestinationScreen::setPreventLine( BOOL myPreventLine)
{
  preventLine = myPreventLine;
}
    
    
// File: $Id: VideoDestinationScreen.cpp,v 1.12 2005/08/25 22:12:23 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationScreen.cpp,v $
// Revision 1.12  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
// Revision 1.11  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.10  2005/07/18 23:49:31  edn2065
// Fixed output window height and width so it is not too large
//
// Revision 1.9  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
//
// Revision 1.8  2005/05/25 14:50:58  edn2065
// Cleaned up unnecessary printfs and removed WX from Makefiles
//
// Revision 1.7  2005/05/24 15:37:17  edn2065
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.6  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.5  2005/04/07 16:43:30  edn2065
// Changed to wxThread
//
// Revision 1.3  2005/04/06 21:21:09  edn2065
// Have number of bugs with threads. Revamping GUI to VideoSourceFile call
//
// Revision 1.2  2005/04/02 18:43:51  edn2065
// Added wait time to ctor.
//
// ----------------------------------------------------------------------
//
