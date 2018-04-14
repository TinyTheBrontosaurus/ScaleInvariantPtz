#include <stdio.h>
#include "VideoStream.h"

/*****************************************************************
 * VideoStream ctor
 *****************************************************************
 */
VideoStream::VideoStream()
{
  numStreams = 0;
}

/*****************************************************************
 * VideoStream dtor
 *****************************************************************
 */
VideoStream::~VideoStream()
{  
} 

/*****************************************************************
 * VideoStream::registerVideo
 *****************************************************************
 */
void VideoStream::registerVideo( IplImage *myFrame, CHAR *myDescription )
{
  //Make sure there is room for the new stream
  if( numStreams < VIDSTR_MAX_STREAMS )
  {
    //Save the stream
    streams[numStreams].frame = myFrame;
    strncpy( streams[numStreams].description, myDescription, 
      VIDSTR_MAX_DESC_LENGTH );
          
    //Increment the number of streams
    numStreams++; 
  }
  else
  {
    printf( "Warning: Too many video streams registered. Omitting: %s" CRLF,
      myDescription ); 
  }
}

/*****************************************************************
 * VideoStream::getNumStreams
 *****************************************************************
 */
UINT8 VideoStream::getNumStreams()
{
  return numStreams;
}
       
/*****************************************************************
 * VideoStream::getStream
 *****************************************************************
 */
IplImage *VideoStream::getStream( UINT8 streamID )
{
  //Make sure it's a legal ID
  if( streamID < numStreams )
  {
    return streams[streamID].frame;
  }
  else
  {
    printf( "VideoStream Error! Bad stream access: %s @ %d" CRLF, 
      __FILE__, __LINE__ );
    return 0;
  }
}
   
/*****************************************************************
 * VideoStream::getDescription
 *****************************************************************
 */
CHAR *VideoStream::getDescription( UINT8 streamID )
{
  //Make sure it's a legal ID
  if( streamID < numStreams )
  {
    return streams[streamID].description;
  }
  else
  {
    printf( "VideoStream Error! Bad description access: %s @ %d" CRLF, 
      __FILE__, __LINE__ );
    return 0;
  }
}


/*
 * Author: Eric D Nelson
 *
 * Version:
 *     $Id: VideoStream.cpp,v 1.3 2005/08/10 02:48:03 edn2065 Exp $
 *
 * Revisions:
 *     $Log: VideoStream.cpp,v $
 *     Revision 1.3  2005/08/10 02:48:03  edn2065
 *     Changed to allow warning and error free ICC compile
 *
 *     Revision 1.2  2005/05/26 19:15:02  edn2065
 *     Added VideoStream. tested. Still need to get tracker working
 *
 *
 */
