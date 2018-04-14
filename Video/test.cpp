#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include "Video.h"
#include "highgui.h"
#include "cv.h"
#include "types.h"

int main()
{
  CHAR title[100];
  
  printf( "Camera capture / GUI display tester.\n" );
  
  VideoDestinationScreen videoDestinationScreen1;
  VideoDestinationScreen videoDestinationScreen2;
  VideoSourceFrameGrabber videoSourceFrameGrabber1;
  VideoSourceFrameGrabber videoSourceFrameGrabber2;
  
  IplImage *img1 = NEW_IMAGE;
  IplImage *img2 = NEW_IMAGE;

  CHAR c = ' ';
  BOOL vTstExecute = true;
  CHAR text[30];

  //Do setup
  
  videoSourceFrameGrabber1.setPath("/dev/v4l/video0");
  videoSourceFrameGrabber2.setPath("/dev/v4l/video1");
  
  //Do program
  //Capture
  UINT32 frame = 0;
  while( vTstExecute )
  {
    frame++;
    sprintf(text, "Frame #%d Test", frame);
    videoDestinationScreen1.setText( text );
    videoDestinationScreen2.setText( text );
    if( !videoSourceFrameGrabber1.getFrame(img1) )
    {
      break;
    }
    if( !videoSourceFrameGrabber2.getFrame(img2) )
    {
      break;
    }
    videoDestinationScreen1.setImg(img1);
    videoDestinationScreen1.sendFrame();
    videoDestinationScreen2.setImg(img2);
    videoDestinationScreen2.sendFrame();
    //otherDst->setImg(img);
    //otherDst->sendFrame();

    //c = getchar();
        
    //printf( "Character c: %c" CRLF, c );
    if( frame > 600 )
    {
          vTstExecute = false;
    }
        
    switch( c )
    {
      case 'Q':
      case 'q':
        vTstExecute = false;
        break;
      default:
        if( frame > 1000 )
        {
          vTstExecute = false;
        }
        break;
    }

  }
  cvReleaseImage( &img1 );
  cvReleaseImage( &img2 );

  printf( "Goodbye!\n");
}

// File: $Id: test.cpp,v 1.11 2005/08/24 19:44:29 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: test.cpp,v $
// Revision 1.11  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.10  2005/07/01 16:43:05  edn2065
// Created sine wave generator for camera
//
// Revision 1.9  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
//
// Revision 1.8  2005/04/25 16:13:22  edn2065
// Tested Video subsystem in lab. Changed cvCopyImage calls to be (src, dst) instead of (dst, src). Inverted boolean getFrame() in test.cpp
//
// Revision 1.7  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.6  2005/04/02 21:59:13  edn2065
// Cleaned up make process and removed excess outputs
//
// Revision 1.4  2005/04/02 21:32:11  edn2065
// Adding to repository
//
// Revision 1.3  2005/04/02 20:13:34  edn2065
//
// Created.
//
// Revision 1.2  2005/04/02 18:42:04  edn2065
// Function creates window and counts frames.
//
// Revision 1.1  2005/04/02 15:38:46  edn2065
// Entering into CVS
//
// ----------------------------------------------------------------------
//
