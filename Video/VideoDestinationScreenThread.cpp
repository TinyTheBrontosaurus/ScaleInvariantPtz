#include <stdio.h>
#include "VideoDestinationScreenThread.h"
#include "highgui.h"

/*****************************************************************
 * VideoDestinationScreenThreadThread ctor
 *****************************************************************
 */
VideoDestinationScreenThread::VideoDestinationScreenThread() : Thread()
{

  //Initialize variables
  numDisplays = 0;
  numInitializedDisplays = 0;
  
  //Initialize display-specific variables
  for( UINT8 curDisplay = 0; curDisplay < VID_DST_MAX_DISPLAYS; curDisplay++ )
  {
    displayImages[curDisplay] = NEW_IMAGE;
    newImage[curDisplay] = false;    
  }
  
  //Start thread
  executeThread = true;
  start(0);

}

/*****************************************************************
 * VideoDestinationFile dtor
 *****************************************************************
 */
VideoDestinationScreenThread::~VideoDestinationScreenThread()
{
  executeThread = false;
  join(0);
  // destroy all the HighGUI windows
  cvDestroyAllWindows(); 
  
  //Initialize display-specific variables
  for( UINT8 curDisplay = 0; curDisplay < VID_DST_MAX_DISPLAYS; curDisplay++ )
  {
    cvReleaseImage( &displayImages[curDisplay] );    
  }
}

/*****************************************************************
 * VideoDestinationScreenThread::createDisplay
 *****************************************************************
 */
UINT8 VideoDestinationScreenThread::createDisplay( CHAR *title )
{
  createDisplaySem.acquire();
  UINT8 displayHandle = numDisplays;
  //Increment the number of displays
  numDisplays++;
  //Copy string pointer
  displayTitles[displayHandle] = title;
  createDisplaySem.release();
  
  return displayHandle;   

}


/*****************************************************************
 * VideoDestinationScreenThread::sendFrame
 *****************************************************************
 */
void VideoDestinationScreenThread::sendFrame(IplImage *imgToSend, UINT8 screenNum)
{

  //Copy the image and set the flag in a threadsafe fashion.
  displaySems[screenNum].acquire();  
  cvCopyImage( imgToSend, displayImages[screenNum] );
  newImage[screenNum] = true;
  displaySems[screenNum].release();
  
}


/*****************************************************************
 * VideoDestinationScreenThread::execute
 *****************************************************************
 */    
void VideoDestinationScreenThread::execute(void *)
{
  UINT8 curDisplay;
  
  //Initialize GUI
  cvInitSystem(0,NULL);
  printf( "Initializing CV system! (%s @ %d)" CRLF, __FILE__, __LINE__ );
  
  //Loop on variable
  while( executeThread )
  {
    //Initialize new displays
    createDisplaySem.acquire();
    if( numInitializedDisplays < numDisplays )
    { 
      //Initialize every new display
      for( curDisplay = numInitializedDisplays; curDisplay < numDisplays; 
           curDisplay++ )
      {
        // create window
        cvNamedWindow(displayTitles[curDisplay],0); 
        // resize main window 
        cvResizeWindow(displayTitles[curDisplay],IM_WIDTH,IM_HEIGHT);           
        // position main window
        cvMoveWindow(displayTitles[curDisplay],0,
          (IM_HEIGHT + 25)* (curDisplay));  
        numInitializedDisplays++;
      }      
    }
    createDisplaySem.release();
    
    
    //Display to all active displays
    for( curDisplay = 0; curDisplay < numInitializedDisplays; curDisplay++ )
    {
      //Get semaphore
      displaySems[curDisplay].acquire(); 
      //Check flag
      if( newImage[curDisplay] )
      {
        //Clear flag
        newImage[curDisplay] = false;    
        
        //Display image
        cvShowImage(displayTitles[curDisplay], displayImages[curDisplay]);        
      } 
      //Release semaphore
      displaySems[curDisplay].release();        
    }
  
    //Sleep
    cvWaitKey(VID_DST_SLEEP_TIME);
  
  }

}
    
// File: $Id: VideoDestinationScreenThread.cpp,v 1.2 2005/08/25 22:12:23 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationScreenThread.cpp,v $
// Revision 1.2  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
// Revision 1.1  2005/08/24 19:44:29  edn2065
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
