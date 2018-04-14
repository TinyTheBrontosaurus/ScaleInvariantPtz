#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include "types.h"
#include "cv.h"

/**************************************************************
 *
 * VideoStream class 
 * Description: Keeps track of a number of different video streams
 *
 *
 **************************************************************
 */

class VideoStream {
 
  public:
   /******************************************
    * ctor
    * Description: 
    ******************************************
    */
    VideoStream();
    

   /******************************************
    * dtor
    * Description: Deallocates memory
    ******************************************
    */        
    ~VideoStream();
   
   /******************************************
    * registerVideo
    * Description: Keeps track of a new video stream 
    * Parameters:  frame - A pointer to the video stream
    *              description - the description of the video stream
    ******************************************
    */  
    void registerVideo( IplImage *myFrame, CHAR *myDescription ); 

   /******************************************
    * getNumStreams
    * Description: Keeps track of a new video stream 
    * Parameters:  frame - A pointer to the video stream
    *              description - the description of the video stream
    ******************************************
    */        
    UINT8 getNumStreams();
       
   /******************************************
    * getStream
    * Description: Get the pointer to a certain video stream
    * Parameters:  streamID - The number associated with a stream. This 
    *                         corresponds to the number send in getDescription.
    * Returns:     The pointer to the image representing the stream
    ******************************************
    */        
    IplImage *getStream( UINT8 streamID );
   
   /******************************************
    * getDescription
    * Description: Retrieves the string describing a video stream
    * Parameters:  streamID - The number associated with a stream. This 
    *                         corresponds to the number send in getStream.
    * Returns:     The string describing the streams
    ******************************************
    */    
    CHAR *getDescription( UINT8 streamID );
    
   
  private:
  
    //The number of streams that have been registered.
    UINT8 numStreams;
    
    //The streams
    struct {
      //The image for the stream
      IplImage *frame;
      //Description of the stream
      CHAR description[VIDSTR_MAX_DESC_LENGTH];
    } streams[VIDSTR_MAX_STREAMS];

};
#endif

/*
 * Author: Eric D Nelson
 * Version:
 *     $Id: VideoStream.h,v 1.1 2005/05/26 17:45:21 edn2065 Exp $
 *
 * Revisions:
 *     $Log: VideoStream.h,v $
 *     Revision 1.1  2005/05/26 17:45:21  edn2065
 *     Added VideoStream. Untested.
 *
 *
 */
