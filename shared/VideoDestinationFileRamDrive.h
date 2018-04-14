
#ifndef VIDEODESTINATIONFILERAMDRIVE_H
#define VIDEODESTINATIONFILERAMDRIVE_H

#include <string.h>
#include "cv.h"
#include "VideoDestination.h"
#include "VideoDestinationFile.h"
#include "types.h"

/**************************************************************
 *
 * VideoDestinationFile class
 * Description: Outputs video to a series of image files. Only writes
 *              to file when buffer is full or dump is called
 **************************************************************
 */
class VideoDestinationFileRamDrive : public VideoDestination {

  public:
    /******************************************
    * ctor
    * Description: Sets defaults
    ******************************************
    */
    VideoDestinationFileRamDrive();

   /******************************************
    * dtor
    *
    ******************************************
    */
    ~VideoDestinationFileRamDrive();

   /******************************************
    * sendFrame
    * Description: Saves the image to file a buffer in memory
    ******************************************
    */
    virtual void sendFrame();

   /******************************************
    * setFile
    * Description: Sets prefix and extension of the file names and resets
    *              the file number.
    * Parameters:  myPrefix    -The file name
    *              myExtension -The file extension
    ******************************************
    */
    void setFile( char *myPrefix, char *myExtension );
    
   /******************************************
    * dump
    * Description: Saves the images to a physical file
    ******************************************
    */        
    void dump();

  protected:
  private:
  
    //The RAM drive holding all images
    IplImage *imageBuffer[VID_DST_RAM_DRIVE_NUM_IMAGES];
    
    //The number of images saved in the buffer.
    UINT32 numImagesSaved;         
    
    //Used to physically save files to disk
    VideoDestinationFile vidDstFile;       

};

#endif

// File: $Id: VideoDestinationFileRamDrive.h,v 1.1 2005/09/08 19:07:49 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationFileRamDrive.h,v $
// Revision 1.1  2005/09/08 19:07:49  edn2065
// Created RAM drive for file storage
//
// Revision 1.2  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.1  2005/04/02 21:53:28  edn2065
// Adding
//
// ----------------------------------------------------------------------
//
