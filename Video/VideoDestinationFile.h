
#ifndef VIDEODESTINATIONFILE_H
#define VIDEODESTINATIONFILE_H

#include <string.h>
#include "cv.h"
#include "VideoDestination.h"
#include "types.h"

/**************************************************************
 *
 * VideoDestinationFile class
 * Description: Outputs video to a series of image files.
 **************************************************************
 */
class VideoDestinationFile : public VideoDestination {

  public:
    /******************************************
    * ctor
    * Description: Sets defaults
    ******************************************
    */
    VideoDestinationFile();

   /******************************************
    * dtor
    *
    ******************************************
    */
    ~VideoDestinationFile();

   /******************************************
    * setFile
    * Description: Sets prefix and extension
    * Parameters:  myPrefix    -The file name
    *              myExtension -The file extension
    ******************************************
    */
    void setFile(CHAR *myPrefix, CHAR *myExtension);

   /******************************************
    * sendFrame
    * Description: Saves the image to file
    ******************************************
    */
    virtual void sendFrame();

  protected:
  private:
    //The file name
    CHAR prefix[VD_TEXT_LENGTH];
    //The file extension
    CHAR extension[VD_TEXT_LENGTH];
    //The current frame being output
    UINT32 numFrame;


};

#endif

// File: $Id: VideoDestinationFile.h,v 1.2 2005/04/23 14:26:32 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestinationFile.h,v $
// Revision 1.2  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.1  2005/04/02 21:53:28  edn2065
// Adding
//
// ----------------------------------------------------------------------
//
