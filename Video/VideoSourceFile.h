
#ifndef VIDEOSOURCEFILE_H
#define VIDEOSOURCEFILE_H

#include "cv.h"
#include "types.h"
#include "VideoSource.h"

/**************************************************************
 *
 * VideoSourceFile class
 * Description: Retrieves video from a series of image files.
 **************************************************************
 */
class VideoSourceFile: public VideoSource {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults.
    ******************************************
    */
    VideoSourceFile();

   /******************************************
    * setFile
    * Description: Sets the file characteristics
    *              Expects files of type "<myPrefix><####(myNumDigits of them)>.<myExtension>
    *              for example test0024.jpg would have myPrefix = 'test', myNumDigits = 4,
    *              myExtension = 'jpg', start = 24.
    * Parameters:  myPrefix - The file directory and prefix
    *              myExtension - The file extension
    *              myNumDigits - The number of digits between prefix
    *                            and extension
    *              start - The number of the first image
    *              myLoop - Whether to start over at the beginning
    *                       of the images when the last image
    *                       is reached
    ******************************************
    */
    void setFile(CHAR *myPrefix, CHAR *myExtension,
      UINT32 myNumDigits, UINT32 start, BOOL myLoop);

   /******************************************
    * getFrame
    * Description: Retrieves an image from the a file.
    * Parameters:  newImg - Where the new image is saved
    * Returns:     True if successful
    ******************************************
    */
    virtual BOOL getFrame(IplImage *newImg);

 protected:

 private:
   //The file name, in syntax used by sprintf
   CHAR fileTemplate[VS_TEXT_LENGTH];

   //The number of the first file in the stream
   UINT32 firstFrame;

   //The number of the current file being read
   UINT32 curFrame;

   //Whether to loop at the end of the stream or not
   BOOL loop;

};

#endif

// File: $Id: VideoSourceFile.h,v 1.2 2005/04/23 14:26:32 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: VideoSourceFile.h,v $
// Revision 1.2  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.1  2005/04/02 21:32:11  edn2065
// Adding to repository
//
// ----------------------------------------------------------------------
//
