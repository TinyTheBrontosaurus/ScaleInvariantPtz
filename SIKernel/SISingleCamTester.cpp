#include <stdio.h>
#include "SISingleCam.h"

static SISingleCam *siKernel;
void mainMenu();
void videoSrcMenu();
void videoDstMenu();

int main() {

  printf( "Function obsolete. Use Menu class." CRLF );
  //mainMenu();

}
/*
void mainMenu()
{

  int c;
  bool execute = true;

  controller = new Controller();
  controller->start(0);

  while( execute )
  {
    printf( "Controller class tester Menu.\n");
    printf( "\n" );
    printf( "1: Set video source\n" );
    printf( "2: Set video destination\n" );
    printf( "3: Apply changes\n" );
    printf( "4: Run application\n" );
    printf( "--------------------------------\n");
    printf( "q: Quit \n" );
    printf( "Enter choice: " );
    do
    {
      c = getchar();
    }while ( c == '\n' );
    printf( "\n" );
    switch( c )
    {
      case '1':
        videoSrcMenu();
        break;
      case '2':
        videoDstMenu();
        break;
      case '3':
        controller->applyOptions();
  printf( "Changes applied.\n" );
        break;
      case '4':
        printf( "Application " );
        if( controller->toggleApp() )
  {
    printf( "running\n");
  }
  else
  {
    printf( "stopped\n");
  }
        break;
      case 'Q':
      case 'q':
        execute = false;
        break;
      default:
        break;
    }

  }

  printf( "Exiting application...\n" );
  delete controller;
  printf( "Goodbye!\n");

}

void videoSrcMenu()
{
  bool inSrcMenu = true;
  int c;

  while( inSrcMenu )
  {
    printf( "Video source menu\n" );
    printf( "1: File\n" );
    printf( "2: Frame Grabber\n" );
    printf( "3: None\n" );
    printf( "Select source: " );
    do
    {
      c = getchar();
    }while ( c == '\n' );

    inSrcMenu = false;
    switch( c )
    {
      case '1':
        controller->options.vidSrc = VID_SRC_FILE;
        strcpy(controller->options.vidSrcPrefix,"/home/edn2065/temp/video/test" );
        controller->options.vidSrcNumDigits = 5;
        strcpy(controller->options.vidSrcSuffix, "jpg");
        controller->options.vidSrcStart = 0;
        controller->options.vidSrcLoop = false;
  break;
      case '2':
        controller->options.vidSrc = VID_SRC_FRAME_GRABBER;
        strcpy(controller->options.vidSrcDirectory, "/dev/v4l/");
        strcpy(controller->options.vidSrcFile, "video0");
  break;
      case '3':
        controller->options.vidSrc = VID_SRC_NONE;
  break;
      default:
        inSrcMenu = true;
  break;
    }
  }
}

void videoDstMenu()
{
  bool inDstMenu = true;
  char c;

  while( inDstMenu )
  {
    printf( "Video destination menu\n" );
    printf( "1: File\n" );
    printf( "2: Screen\n" );
    printf( "3: None\n" );
    printf( "Select destination: " );
    do
    {
      c = getchar();
    }while ( c == '\n' );
    inDstMenu = false;

    switch( c )
    {
      case '1':
        controller->options.vidDst = VID_DST_FILE;
        strcpy(controller->options.vidDstDirectory,"/home/edn2065/temp/video/" );
        strcpy(controller->options.vidDstPrefix, "test" );
        strcpy(controller->options.vidDstSuffix, "jpg" );
  break;
      case '2':
  controller->options.vidDst = VID_DST_SCREEN;
  break;
      case '3':
        controller->options.vidDst = VID_DST_NONE;
  break;
      default:
        inDstMenu = true;
  break;
    }
  }
}
*/

// File: $Id: SISingleCamTester.cpp,v 1.7 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SISingleCamTester.cpp,v $
// Revision 1.7  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.6  2005/06/03 12:49:46  edn2065
// Changed Controller to SIKernel
//
// Revision 1.5  2005/05/24 15:36:37  edn2065
//
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.3  2005/04/07 15:21:46  edn2065
// ControllerTester works with pthread.
//
// Revision 1.3  2005/04/06 19:02:06  edn2065
// Added functionality for creation and destruction of input and output video streams.
//
//
// ----------------------------------------------------------------------
//
