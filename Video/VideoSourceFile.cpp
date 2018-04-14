#include <stdio.h>
#include "highgui.h"
#include "VideoSourceFile.h"

/*****************************************************************
 * VideoSourceFile ctor
 *****************************************************************
 */
VideoSourceFile::VideoSourceFile()
{
  //Set defaults
  strcpy(fileTemplate, "");
  curFrame = 0;
  firstFrame = 0;
  loop = false;
}

/*****************************************************************
 * VideoSourceFile::setFile
 *****************************************************************
 */
void VideoSourceFile::setFile(CHAR *myPrefix, CHAR *myExtension,
  UINT32 myNumDigits, UINT32 start, BOOL myLoop)
{
  //Set template
  sprintf( fileTemplate, "%s%%0%dd.%s",
    myPrefix, myNumDigits, myExtension );

  //Debug notification
  printf( "Template: %s\n", fileTemplate );

  //Save other settings
  curFrame = start;
  firstFrame = start;
  loop = myLoop;
}

/*****************************************************************
 * VideoSourceFile::getFrame
 *****************************************************************
 */
BOOL VideoSourceFile::getFrame(IplImage *newImg)
{
  BOOL success = true;
  IplImage *imgFromFile;

  //Setup the file to be read
  CHAR filename[VS_TEXT_LENGTH];
  sprintf(filename, fileTemplate, curFrame);
  //printf( "Attempting to load file %s" CRLF, filename );
  imgFromFile = cvLoadImage( filename, 1 );

  //Check if file was found.
  if( imgFromFile == (IplImage *) 0 )
  {
    //If it was the first file that was not found,
    // then it's an error. Return NULL. Otherwise, try
    // looping.
    if( curFrame != firstFrame && loop )
    {
      //Try looping.
      curFrame = firstFrame;
      success = getFrame(newImg);
    }
    else
    {
      success = false;
      printf( "End of VideoSoureFile stream.\n" );
    }
  }
  else
  {
    //File successfully loaded.
    curFrame++;

    //Copy image information
    cvCopyImage( imgFromFile, newImg );
    
    //Get rid of loaded file. 
    cvReleaseImage(&imgFromFile);
  }

  return success;
}


// File: $Id: VideoSourceFile.cpp,v 1.6 2005/05/27 14:57:27 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: VideoSourceFile.cpp,v $
// Revision 1.6  2005/05/27 14:57:27  edn2065
// Fix VideoSource memory leak and possible OBOE in CSEG
//
// Revision 1.5  2005/04/25 16:13:22  edn2065
// Tested Video subsystem in lab. Changed cvCopyImage calls to be (src, dst) instead of (dst, src). Inverted boolean getFrame() in test.cpp
//
// Revision 1.4  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.3  2005/04/06 21:21:09  edn2065
// Have number of bugs with threads. Revamping GUI to VideoSourceFile call
//
// Revision 1.2  2005/04/02 21:59:13  edn2065
// Cleaned up make process and removed excess outputs
//
// Revision 1.1  2005/04/02 21:32:11  edn2065
// Adding to repository
//
// ----------------------------------------------------------------------
//
