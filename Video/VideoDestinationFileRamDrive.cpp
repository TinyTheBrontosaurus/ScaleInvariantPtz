#include <stdio.h>
#include "highgui.h"
#include "VideoDestinationFileRamDrive.h"

/*****************************************************************
 * VideoDestinationFileRamDrive ctor
 *****************************************************************
 */
VideoDestinationFileRamDrive::VideoDestinationFileRamDrive()   
{
  UINT32 imageCtorIterator;
  
  for( imageCtorIterator = 0; 
       imageCtorIterator < VID_DST_RAM_DRIVE_NUM_IMAGES;
       imageCtorIterator++ )
  {
    imageBuffer[imageCtorIterator] = NEW_IMAGE;
  }
  numImagesSaved = 0;
  
}

/*****************************************************************
 * VideoDestinationFileRamDrive dtor
 *****************************************************************
 */
VideoDestinationFileRamDrive::~VideoDestinationFileRamDrive()
{  

  UINT32 imageDtorIterator;
  
  //Get rid of anything in the buffer
  dump();
  
  for( imageDtorIterator = 0; 
       imageDtorIterator < VID_DST_RAM_DRIVE_NUM_IMAGES;
       imageDtorIterator++ )
  {
    cvReleaseImage(&imageBuffer[imageDtorIterator]);
  }    

}

/*****************************************************************
 * VideoDestinationFileRamDrive::setFile
 *****************************************************************
 */
void VideoDestinationFileRamDrive::setFile( char *myPrefix, char *myExtension )
{
  //Copy the prefix and extension and reset the file number
  vidDstFile.setFile(myPrefix, myExtension);
  
  //Get rid of anything in the buffer
  dump();  
}

/*****************************************************************
 * VideoDestinationFileRamDrive::sendFrame
 *****************************************************************
 */
void VideoDestinationFileRamDrive::sendFrame()
{
  //Place text, boxes, etc.
  prepareImage();

  cvCopyImage(imgToDisp, imageBuffer[numImagesSaved]);
  numImagesSaved++;
  
  if( numImagesSaved >= VID_DST_RAM_DRIVE_NUM_IMAGES - 1)
  {
    dump();
  }
  
}
  
/*****************************************************************
 * VideoDestinationFileRamDrive::dump
 *****************************************************************
 */
void VideoDestinationFileRamDrive::dump()
{
  printf( "Dumping... (%s @ %d)" CRLF, __FILE__, __LINE__ );
  UINT32 imageSaveIterator;
  
  //Save each image to file
  for( imageSaveIterator = 0; 
       imageSaveIterator < numImagesSaved;
       imageSaveIterator++ )
  {
  
    vidDstFile.setImg(imageBuffer[imageSaveIterator]);
    vidDstFile.sendFrame();    
  }
  
  //Reset the number of images saved
  numImagesSaved = 0;
  
  printf( "Done dumping. (%s @ %d)" CRLF, __FILE__, __LINE__ );  
}



// File: $Id: VideoDestinationFileRamDrive.cpp,v 1.1 2005/09/08 19:07:49 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationFileRamDrive.cpp,v $
// Revision 1.1  2005/09/08 19:07:49  edn2065
// Created RAM drive for file storage
//
