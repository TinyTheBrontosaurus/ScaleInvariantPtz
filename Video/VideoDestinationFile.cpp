#include <stdio.h>
#include "highgui.h"
#include "VideoDestinationFile.h"

/*****************************************************************
 * VideoDestinationFile ctor
 *****************************************************************
 */
VideoDestinationFile::VideoDestinationFile()
{
  //Copy the prefix and extension
  strcpy( prefix, "" );
  strcpy( extension, "" );
  //Reset the frame
  numFrame = 0;
}

/*****************************************************************
 * VideoDestinationFile dtor
 *****************************************************************
 */
VideoDestinationFile::~VideoDestinationFile()
{

}

/*****************************************************************
 * VideoDestinationFile::setFile
 *****************************************************************
 */
void VideoDestinationFile::setFile( char *myPrefix, char *myExtension )
{
  //Copy the prefix and extension
  strcpy( prefix, myPrefix );
  strcpy( extension, myExtension );
  //Reset the frame
  numFrame = 0;
}

/*****************************************************************
 * VideoDestinationFile::sendFrame
 *****************************************************************
 */
void VideoDestinationFile::sendFrame()
{
  //Place text, boxes, etc.
  prepareImage();

  //Send the image to a file
  char filename[VD_TEXT_LENGTH];
  sprintf(filename, "%s%05d.%s", prefix, numFrame, extension);
  cvSaveImage(filename, imgToDisp );

  //Indicate the save
  //printf("Saved: %s\n", filename);
  //Increment the frame number
  numFrame++;
}


// File: $Id: VideoDestinationFile.cpp,v 1.5 2005/06/10 17:03:47 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationFile.cpp,v $
// Revision 1.5  2005/06/10 17:03:47  edn2065
// Added MFS
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
// Revision 1.1  2005/04/02 20:13:34  edn2065
//
// Created.
//
// ----------------------------------------------------------------------
//
