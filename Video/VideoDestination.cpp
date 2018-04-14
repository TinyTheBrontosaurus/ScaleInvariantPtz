#include "VideoDestination.h"
#include <stdio.h>

/*****************************************************************
 * VideoDestination ctor
 *****************************************************************
 */
 VideoDestination::VideoDestination()
{
  //Set pointers to null
  strcpy( text, "" );
  boxes = 0;
  imgOrig = 0;
  imgToDisp = 0;

  //Create display image
  imgSuperimposedFullColor = NEW_IMAGE;
  imgSuperimposedFullBW = NEW_IMAGE_BW;
  imgSuperimposedSmallDM = NEW_IMAGE_DM;

  //Indicate that the boxes array is empty
  numBoxes = 0;

  //Turn off superimposing
  doBoxes = false;
  doText = false;

  //Initialize color and fonts
  cvInitFont( &font, CV_FONT_HERSHEY_DUPLEX, 1.0f, 1.0f);
  color = CV_RGB(0, 0, 0);
}

/*****************************************************************
 * VideoDestination dtor
 *****************************************************************
 */
VideoDestination::~VideoDestination()
{
  cvReleaseImage(&imgSuperimposedFullColor);
  cvReleaseImage(&imgSuperimposedFullBW);
}

/*****************************************************************
 * VideoDestination::setText
 *****************************************************************
 */
void VideoDestination::setText( CHAR *myText )
{
  strcpy( text, myText );
  doText = (strlen(text) != 0);
}

/*****************************************************************
 * VideoDestination::setBoxes
 *****************************************************************
 */
void VideoDestination::setBoxes( CvRect *myBoxes, UINT8 myNumBoxes )
{
  boxes = myBoxes;
  numBoxes = myNumBoxes;
  doBoxes = numBoxes != 0;
}

/*****************************************************************
 * VideoDestination::setImg
 *****************************************************************
 */
void VideoDestination::setImg( IplImage *myImg )
{
  //Set image
  imgOrig = myImg;
  //Reset image to be displayed
  imgToDisp = imgOrig;
}

/*****************************************************************
 * VideoDestination::prepareImage
 *****************************************************************
 */
void VideoDestination::prepareImage()
{

  BOOL superimposeError = false;  

  //See if text and/or boxes need to be added
  if( doText || doBoxes )
  {
    //Check the size of the image needed
    if( imgOrig->width == IM_WIDTH )
    {
      //Must be full size, no need to check height
      //Now check number of channels
      if( imgOrig->nChannels == IM_CHANNELS )
      {
        imgSuperimposed = imgSuperimposedFullColor;
      }
      else if( imgOrig->nChannels == 1 )
      {
        imgSuperimposed = imgSuperimposedFullBW;
      }
      else
      {
        superimposeError = true;         
      }
    }
    else if( imgOrig->width == IM_WIDTH / CSEG_DENSITYMAP &&
      imgOrig->nChannels == 1 )
    {
      imgSuperimposed = imgSuperimposedSmallDM;
    }
    else
    {
      superimposeError = true;
    }
    
      
    if(superimposeError)
    {
      printf( "Error in %s @ %d" CRLF, __FILE__, __LINE__ );
      printf( "imgOrig size: W:%d H:%d D:%d C:%d Size:%d"CRLF,
        imgOrig->width, imgOrig->height, imgOrig->depth, 
        imgOrig->nChannels, imgOrig->imageSize);       
    } 
    else
    {
      
      //Make a copy of the image
      cvCopyImage( imgOrig, imgSuperimposed );
  
      //Add text (if necessary)
      if( doText )
      {
        addText(imgSuperimposed, text, cvPoint(50,50));
      }
  
      //Add boxes (if necessary)
      if( doBoxes )
      {
        addRects(imgSuperimposed, boxes, numBoxes);
      }
  
      //Set image to be displayed to superimposed image
      imgToDisp = imgSuperimposed;
    }
  }

}

/*****************************************************************
 * VideoDestination::addText
 *****************************************************************
 */
void VideoDestination::addText(IplImage *img, CHAR *text, CvPoint pt) {
  if(img->nChannels > 1) {
    cvPutText(img, text, pt, &font, CV_RGB(0,255,0));
  } else {
    cvPutText(img, text, pt, &font, CV_RGB(255,255,255));
  }
}

/*****************************************************************
 * VideoDestination::addRects
 *****************************************************************
 */
void VideoDestination::addRects(IplImage *img, CvRect rects[], UINT8 num) {
  CvScalar color;
  int x_top, y_top, x_bot, y_bot;

  //Set the color of the rectangles based on the image color
  if(img->nChannels >1) {
    color = CV_RGB(0,255,0);
  }
  else {
    color = CV_RGB(255,255,255);
  }

  //Add each individual rectangle
  for(int i = 0; i < num; i++) {

    x_top = rects[i].x;
    y_top = rects[i].y;
    x_bot = x_top + rects[i].width;
    y_bot = y_top + rects[i].height;
    cvRectangle(img, cvPoint(x_top, y_top), cvPoint(x_bot, y_bot),
    color, 1, 8, 0);
  }
}

// File: $Id: VideoDestination.cpp,v 1.5 2005/06/09 13:48:03 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestination.cpp,v $
// Revision 1.5  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.4  2005/04/25 16:13:22  edn2065
// Tested Video subsystem in lab. Changed cvCopyImage calls to be (src, dst) instead of (dst, src). Inverted boolean getFrame() in test.cpp
//
// Revision 1.3  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.2  2005/04/02 18:45:12  edn2065
// fixed ctor.
//
// ----------------------------------------------------------------------
//
