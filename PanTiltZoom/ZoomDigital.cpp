#include "ZoomDigital.h"
#include <fstream>
using namespace std;

/*****************************************************************
 * ZoomDigital ctor
 *****************************************************************
 */
 ZoomDigital::ZoomDigital()
{
  //Allocate new image
  zoomedImg = NEW_IMAGE;
  splicedImg = NEW_IMAGE;
  bgImg = 0;
  
  //Set defaults
  digitalMag = ZOOM_DIGITAL_MAG_DEFAULT;
  principalPointX = 0;
  principalPointY = 0;
  spliceOutput = false;
  
  noROI.x = 0;
  noROI.y = 0;
  noROI.width = IM_WIDTH;
  noROI.height = IM_HEIGHT;
    
}

/*****************************************************************
 * ZoomDigital dtor
 *****************************************************************
 */
ZoomDigital::~ZoomDigital()
{
  //Release allocated data
  cvReleaseImage( &zoomedImg );
  cvReleaseImage( &splicedImg );
}

/*****************************************************************
 * ZoomDigital::setImages
 *****************************************************************
 */
void ZoomDigital::setImages( IplImage *myOrigImg, IplImage *myBgImg )
{
  origImg = myOrigImg;
  bgImg = myBgImg;
}

/*****************************************************************
 * ZoomDigital::setBgImage
 *****************************************************************
 */
void ZoomDigital::setBgImage( IplImage *myBgImg )
{
  bgImg = myBgImg;
}


/*****************************************************************
 * ZoomDigital::getZoomedImg
 *****************************************************************
 */
IplImage *ZoomDigital::getZoomedImg()
{
  return zoomedImg;
}

/*****************************************************************
 * ZoomDigital::getSplicedImg
 *****************************************************************
 */
IplImage *ZoomDigital::getSplicedImg()
{
  return splicedImg;
}

/*****************************************************************
 * ZoomDigital::zoom
 *****************************************************************
 */
void ZoomDigital::zoom( FLOAT magnification )
{
  //Do nothing
}

/*****************************************************************
 * ZoomDigital::zoomImage
 *****************************************************************
 */
void ZoomDigital::zoomImage( FLOAT magnification, FLOAT opticalZoom )
{
  
  magnification = magnification / opticalZoom;
  
  //Save the new magnification
  digitalMag = magnification;

  if( magnification > ZOOM_IN_DIGITAL_MIN )
  {
      
    //The center of the image based on image size, principal point, 
    //and magnification
    FLOAT imgCenterY;
    FLOAT imgCenterX;
          
    //Calculate the center of zooming
    imgCenterY = IM_HEIGHT/2 + (digitalMag-1.0)/(digitalMag)*principalPointY;
    imgCenterX = IM_WIDTH/2 + (digitalMag-1.0)/(digitalMag)*principalPointX;
    
    //Set the ROI accordingly
    zoomROI.x = (int)( imgCenterX - IM_WIDTH / (2*digitalMag) );
    zoomROI.y = (int)( imgCenterY - IM_HEIGHT / (2*digitalMag) );
    zoomROI.width = (int)(IM_WIDTH/digitalMag);
    zoomROI.height = (int)(IM_HEIGHT/digitalMag);
      
    //This alters the ROI of the image. 
    cvSetImageROI( origImg, zoomROI );
    cvResize(origImg, zoomedImg, CV_INTER_NN );  
    
    //Wait for background to be ready
    hzoPreZoomPrePickoffBarrier->wait();
    hzoPreZoomPostPickoffBarrier->wait();
    if( bgImg != 0 )
    {
      cvCopy(zoomedImg, splicedImg );  
    }
    cvSetImageROI( origImg, noROI );
  }
  //Splice the panoramic and zoomed image
  else if( spliceOutput && magnification > ZOOM_OUT_DIGITAL_MIN 
           && magnification < ZOOM_OUT_DIGITAL_MAX
           //&& magnification != ZOOM_IN_DIGITAL_MIN 
           )
  {
    //Allocate memory for the subimage
    CvSize subImageSize;
    IplImage *subImage;
    subImageSize.height =(int)( (FLOAT)IM_HEIGHT * magnification);
    subImageSize.width = (int)((FLOAT)IM_WIDTH * magnification);
    subImage = cvCreateImage( subImageSize, IPL_DEPTH_8U, 3);
    
    //Copy the background to the final image
    cvZero(zoomedImg );      

    //Scale the image
    cvResetImageROI( origImg );
    cvResize( origImg, subImage, ZOOM_DIG_INTERPOLATION );

    
    //Wait for background to be ready   
    hzoPreZoomPrePickoffBarrier->wait();
    hzoPreZoomPostPickoffBarrier->wait();

    
    //Insert the subimage into the big image
    if( bgImg == 0 )
    {
      customInsert( subImage, zoomedImg,
                  (SINT32)((1-magnification)*(IM_WIDTH/2 + principalPointX)),
                  (SINT32)((1-magnification)*(IM_HEIGHT/2 + principalPointY) ));
    }
    else
    {      
      cvCopy(bgImg, splicedImg );
      customInsert( subImage, zoomedImg, splicedImg,
                  (SINT32)((1-magnification)*(IM_WIDTH/2 + principalPointX)),
                  (SINT32)((1-magnification)*(IM_HEIGHT/2 + principalPointY) ));
    }

    //Deallocate the subimage
    cvReleaseImage( &subImage );    
  }
  else
  {
    //There is no zooming needed
    cvCopy( origImg, zoomedImg );  
    
    cvCopy( origImg, splicedImg );
            
    //Wait for background to be ready
    hzoPreZoomPrePickoffBarrier->wait();
    hzoPreZoomPostPickoffBarrier->wait();
    
  }
  
}

/*****************************************************************
 * ZoomDigital::zoomImage
 *****************************************************************
 */
void ZoomDigital::zoomImage( FLOAT magnification, FLOAT opticalZoom, 
                             FLOAT imgCenterX, FLOAT imgCenterY )
{
  magnification = magnification / opticalZoom;
   
  //Save the new magnification
  digitalMag = magnification;   
    
  //Where to insert the intermediate image for customInsert
  SINT32 xInsert = 0;
  SINT32 yInsert = 0;
      
  //Create intermediate image
  CvSize intermediateResizeSize; 
  //Splice the panoramic and zoomed image
  if( spliceOutput && magnification > ZOOM_OUT_DIGITAL_MIN 
           && magnification < ZOOM_OUT_DIGITAL_MAX )
  {
    //Allocate memory for the subimage
    //See notes 8/15 pg 1-2 for derivation
    IplImage *subImageS2;
    CvSize subImageS2Size;
    CvSize subImageS1Size;
    CvRect initialImageToS2ROI;
    CvRect initialImageToS1ROI;
    CvPoint s1InsertionPoint;
    CvPoint s2InsertionPoint;
    
    //Set up S1 
    subImageS1Size.width = (int)(IM_WIDTH * magnification);
    subImageS1Size.height = (int)(IM_HEIGHT * magnification);
    initialImageToS1ROI.width = IM_WIDTH;
    initialImageToS1ROI.height = IM_HEIGHT;
    initialImageToS1ROI.x = 0;
    initialImageToS1ROI.y = 0;
    s1InsertionPoint.x = (int)(IM_WIDTH/2 - imgCenterX*magnification);
    s1InsertionPoint.y = (int)(IM_HEIGHT/2 - imgCenterY*magnification);
    
    //Deal with x-position
    if( subImageS1Size.width + s1InsertionPoint.x >= IM_WIDTH )
    {
    
      subImageS2Size.width = IM_WIDTH - s1InsertionPoint.x - 1;
      initialImageToS2ROI.x = initialImageToS1ROI.x;
      initialImageToS2ROI.width = (int)(
        (IM_WIDTH - s1InsertionPoint.x) / magnification);
      s2InsertionPoint.x = s1InsertionPoint.x;
      
    
    }
    else if( s1InsertionPoint.x < 0 )
    {
      subImageS2Size.width = subImageS1Size.width + s1InsertionPoint.x;
      initialImageToS2ROI.width = (int)(
        (subImageS1Size.width + s1InsertionPoint.x) / magnification);
      initialImageToS2ROI.x = IM_WIDTH - initialImageToS2ROI.width;
      s2InsertionPoint.x = 0;
    
    }
    else
    {
    
      //No problems
      subImageS2Size.width = subImageS1Size.width;
      initialImageToS2ROI.x = initialImageToS1ROI.x;
      initialImageToS2ROI.width = initialImageToS1ROI.width;
      s2InsertionPoint.x = s1InsertionPoint.x;
    }
    
    //Deal with y-position
    if( subImageS1Size.height + s1InsertionPoint.y >= IM_HEIGHT )
    {
    
      subImageS2Size.height = IM_HEIGHT - s1InsertionPoint.y - 1;
      initialImageToS2ROI.y = initialImageToS1ROI.y;
      initialImageToS2ROI.height = (int)(
        (IM_HEIGHT - s1InsertionPoint.y) / magnification);
      s2InsertionPoint.y = s1InsertionPoint.y;          
    }
    else if( s1InsertionPoint.y < 0 )
    {
      subImageS2Size.height = subImageS1Size.height + s1InsertionPoint.y;
      initialImageToS2ROI.height = (int)(
        (subImageS1Size.height + s1InsertionPoint.y) / magnification);
      initialImageToS2ROI.y = IM_HEIGHT - initialImageToS2ROI.height;
      s2InsertionPoint.y = 0;
    
    }    
    else
    {
    
      //No problems
      subImageS2Size.height = subImageS1Size.height;
      initialImageToS2ROI.y = initialImageToS1ROI.y;
      initialImageToS2ROI.height = initialImageToS1ROI.height;
      s2InsertionPoint.y = s1InsertionPoint.y;
    }
    
        
    subImageS2 = cvCreateImage( subImageS2Size, IPL_DEPTH_8U, 3);
  
    cvZero(zoomedImg );
    //Scale the image
    cvSetImageROI( origImg, initialImageToS2ROI );
    cvResize( origImg, subImageS2, ZOOM_DIG_INTERPOLATION );
    cvSetImageROI( origImg, noROI );
    
    //Wait for background to be ready
    hzoPreZoomPrePickoffBarrier->wait();
    hzoPreZoomPostPickoffBarrier->wait();
    
    //Insert the subimage into the big image
    if( bgImg == 0 )
    {
      customInsert( subImageS2, zoomedImg, s2InsertionPoint.x, s2InsertionPoint.y);
    }
    else
    {
      //Copy the background to the final image
      cvCopy(bgImg, splicedImg );
    
      customInsert( subImageS2, zoomedImg, splicedImg, s2InsertionPoint.x, s2InsertionPoint.y);
    }
    //Deallocate the subimage
    cvReleaseImage( &subImageS2 );    
  }
  else
  {
    if( magnification < ZOOM_IN_DIGITAL_MIN )
    {
      //Do unity magnification
      magnification = ZOOM_DIGITAL_MAG_DEFAULT;
    }
             
    BOOL overEdge = false;
           
    //Save the new magnification
    digitalMag = magnification;
    
    //Set the ROI accordingly
    zoomROI.x = (int)( imgCenterX - IM_WIDTH / (2*digitalMag) );
    zoomROI.y = (int)( imgCenterY - IM_HEIGHT / (2*digitalMag) );
    zoomROI.width = (int)(IM_WIDTH/digitalMag);
    zoomROI.height = (int)(IM_HEIGHT/digitalMag);
     
    //See if digital tracking goes off x-edge of image 
    if( zoomROI.x < 0 )
    {
      overEdge = true;
      //On left side
      trackingROI.x = 0;
      trackingROI.width = zoomROI.x + zoomROI.width;
      intermediateResizeSize.width = (int)(IM_WIDTH + zoomROI.x * magnification);
      xInsert = (SINT32)(-zoomROI.x*magnification);
    }
    else if( zoomROI.x + zoomROI.width > IM_WIDTH )
    {
      overEdge = true;
      //On right side
      trackingROI.x = zoomROI.x;
      trackingROI.width = IM_WIDTH - zoomROI.x;
      intermediateResizeSize.width = (int)((IM_WIDTH - zoomROI.x)*magnification);
      xInsert = 0;
    }
    else
    {
      trackingROI.x = zoomROI.x;
      trackingROI.width = zoomROI.width;
      intermediateResizeSize.width = IM_WIDTH;
      xInsert = 0;
      
    }
    
    //See if digital tracking goes off y-edge of image 
    if( zoomROI.y < 0 )
    {
      overEdge = true;
      //On left side
      trackingROI.y = 0;
      trackingROI.height = zoomROI.y + zoomROI.height;
      intermediateResizeSize.height = (int)(IM_HEIGHT + zoomROI.y * magnification);
      yInsert = (SINT32)(-zoomROI.y*magnification);
    }
    else if( zoomROI.y + zoomROI.height > IM_HEIGHT )
    {
      overEdge = true;
      //On right side
      trackingROI.y = zoomROI.y;
      trackingROI.height = IM_HEIGHT - zoomROI.y;
      intermediateResizeSize.height = (int)((IM_HEIGHT - zoomROI.y)*magnification);
      yInsert = 0;
    }
    else
    {
      trackingROI.y = zoomROI.y;
      trackingROI.height = zoomROI.height;
      intermediateResizeSize.height = IM_HEIGHT;
      yInsert = 0;
    }
    
    
    
    //Do the resizing depending on whether the ROI went over the edge of the image or
    //not.
    if( overEdge )
    {              
      
      IplImage *intermediateResizeImg = 
        cvCreateImage( intermediateResizeSize, IPL_DEPTH_8U, 3);
        
      //Do resize to intermediate image      
      cvSetImageROI( origImg, trackingROI );
      cvResize(origImg, intermediateResizeImg, CV_INTER_NN );  
      cvSetImageROI( origImg, noROI );
      cvZero( zoomedImg );
      
      //Wait for background to be ready
      hzoPreZoomPrePickoffBarrier->wait();
      hzoPreZoomPostPickoffBarrier->wait();
      
      //Insert the subimage into the big image
      if( bgImg == 0 )
      {
        customInsert( intermediateResizeImg, zoomedImg, xInsert, yInsert);
      }
      else
      {
        //Copy the background to the final image
        cvCopy(bgImg, splicedImg );
        customInsert( intermediateResizeImg, zoomedImg, splicedImg, xInsert, yInsert);
      }
                  
      cvReleaseImage( &intermediateResizeImg );
  
      
    }
    else
    {        
      //This alters the ROI of the image. 
      cvSetImageROI( origImg, zoomROI );
      cvResize(origImg, zoomedImg, CV_INTER_NN );  
            
      cvCopy(zoomedImg, splicedImg); 
      cvSetImageROI( origImg, noROI );
      
      //Wait for background to be ready
      hzoPreZoomPrePickoffBarrier->wait();
      hzoPreZoomPostPickoffBarrier->wait();
      
    }
  }      
}

/*****************************************************************
 * ZoomDigital::inquiry
 *****************************************************************
 */
BOOL ZoomDigital::inquiry(FLOAT *myZoomMag)
{
  *myZoomMag = digitalMag;
  return true;
}

/*****************************************************************
 * ZoomDigital::reset
 *****************************************************************
 */    
void ZoomDigital::reset()
{
  zoom(ZOOM_DIGITAL_MAG_DEFAULT);
}

/*****************************************************************
 * ZoomDigital::registerVideo
 *****************************************************************
 */  
void ZoomDigital::registerVideo( VideoStream *vStream )
{
  vStream->registerVideo(zoomedImg, "Digitally Zoomed Image");
}

/*****************************************************************
 * ZoomDigital::setPrincipalPoint
 *****************************************************************
 */  
void ZoomDigital::setPrincipalPoint( SINT16 ppX, SINT16 ppY )
{
  principalPointY = ppY;
  principalPointX = ppX;
}

/*****************************************************************
 * ZoomDigital::setSpliceOutput
 *****************************************************************
 */  
void ZoomDigital::setSpliceOutput( BOOL mySpliceOutput )
{
  spliceOutput = mySpliceOutput;
}

/*****************************************************************
 * ZoomDigital::customInsert
 *****************************************************************
 */  
void ZoomDigital::customInsert( const IplImage *subImage, IplImage *dest,
                                     SINT32 x, SINT32 y )
{
  
  UINT32 curSrcPtr = 0;
  UINT32 curDestPtr = 0;
  UINT8 *srcData =
    reinterpret_cast<UINT8 *>(subImage->imageData);
  UINT8 *destData =
    reinterpret_cast<UINT8 *>(dest->imageData);
 
  //Go through each row
  for( UINT32 curY = HZO_TOP_BORDER; curY < subImage->height-HZO_BOT_BORDER; curY++ )
  {
    //And through each element of the rows
    for( UINT32 curX = HZO_LT_BORDER; curX < subImage->width-HZO_RT_BORDER; curX++ )
    {
      curDestPtr = (curY+y)*dest->widthStep + (curX+x)*subImage->nChannels;
      curSrcPtr  = curY * subImage->widthStep + curX * subImage->nChannels;
      //Now cycle through each channel.
      for( UINT32 curChannel = 0; curChannel < subImage->nChannels; curChannel++)
      {
        destData[curDestPtr+curChannel]=srcData[curSrcPtr+curChannel];
      }
    }
  }
}


/*****************************************************************
 * ZoomDigital::customInsert
 *****************************************************************
 */  
void ZoomDigital::customInsert( const IplImage *subImage, IplImage *dest1, IplImage *dest2,
                                     SINT32 x, SINT32 y )
{
  
  UINT32 curSrcPtr = 0;
  UINT32 curDestPtr = 0;
  UINT8 *srcData =
    reinterpret_cast<UINT8 *>(subImage->imageData);
  UINT8 *dest1Data =
    reinterpret_cast<UINT8 *>(dest1->imageData);
  UINT8 *dest2Data =
    reinterpret_cast<UINT8 *>(dest2->imageData);
 
  //Go through each row
  for( UINT32 curY = HZO_TOP_BORDER; curY < subImage->height-HZO_BOT_BORDER; curY++ )
  {
    //And through each element of the rows
    for( UINT32 curX = HZO_LT_BORDER; curX < subImage->width-HZO_RT_BORDER; curX++ )
    {
      curDestPtr = (curY+y)*dest1->widthStep + (curX+x)*subImage->nChannels;
      curSrcPtr  = curY * subImage->widthStep + curX * subImage->nChannels;
      //Now cycle through each channel.
      for( UINT32 curChannel = 0; curChannel < subImage->nChannels; curChannel++)
      {
        dest1Data[curDestPtr+curChannel]=srcData[curSrcPtr+curChannel];
        dest2Data[curDestPtr+curChannel]=srcData[curSrcPtr+curChannel];
      }
    }
  }
}

/*****************************************************************
 * ZoomDigital::savePrincipalPoint
 *****************************************************************
 */  
void ZoomDigital::savePrincipalPoint(CHAR *filename)
{

  //Create the file that will be opened
  fstream file;
  
  try {
    //Open the file
    file.open(filename, ios::out);
    //Set the exceptions
    file.exceptions(ofstream::eofbit | ofstream::failbit | ofstream::badbit);
    
    // First send the table size to the file
    file << principalPointX << " " << principalPointY << endl;
    
    printf( "Principal Point successfully saved to %s (%s @ %d)"CRLF, 
      filename, __FILE__, __LINE__ );
      
  } catch( ofstream::failure e ) {
    printf("Principal Point file write error (%s) (%s @ %d)"CRLF, 
      filename, __FILE__, __LINE__ );     
  }
  
  //Close the file
  if( file.is_open() ) {
    file.close();  
  }
  
}

/*****************************************************************
 * ZoomDigital::loadPrincipalPoint
 *****************************************************************
 */  
void ZoomDigital::loadPrincipalPoint(CHAR *filename)
{
{
  //The file to be loaded  
  fstream file;
    
  try {
    //Open the file
    file.open(filename, ios::in);
    
    //Turn exceptions on
    file.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    
    // First get the table size from the file
    file >> principalPointX >> principalPointY;
    
    printf( "Principal point successfully loaded from %s (%s @ %d)"CRLF, 
      filename, __FILE__, __LINE__ );
    
  } catch( ifstream::failure e ) {
    printf( "Error in loading principal point from %s (%s @ %d)"CRLF, 
      filename, __FILE__, __LINE__ );
    
  }
  
  //Close the file
  if( file.is_open() ) {
    file.close();
  }
  
}
}

// File: $Id: ZoomDigital.cpp,v 1.25 2005/09/06 20:41:09 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: ZoomDigital.cpp,v $
// Revision 1.25  2005/09/06 20:41:09  edn2065
// Added dead zone to digital zoom
//
// Revision 1.24  2005/09/06 19:44:17  edn2065
// Moved HZO barriers to Zoom to help parallelism
//
// Revision 1.23  2005/09/06 00:54:01  edn2065
// removed erroneous comments
//
// Revision 1.22  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.21  2005/08/26 21:03:51  edn2065
// Put border around spliced image
//
// Revision 1.20  2005/08/26 20:27:13  edn2065
// Pickoff points work. Implemented HZO camera setup
//
// Revision 1.19  2005/08/26 19:04:14  edn2065
// Pickoff points work, but framerate dropped
//
// Revision 1.18  2005/08/19 01:32:26  edn2065
// Fixed digital mag problem
//
// Revision 1.17  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.16  2005/08/16 01:45:33  edn2065
// Added digital fixation of zoom out
//
// Revision 1.14  2005/08/09 04:23:42  edn2065
// Scaled measurement noise to go with zoom
//
// Revision 1.13  2005/06/21 15:04:22  edn2065
// Fixed Hybrid zoom out problem with principal point drift caused by width and height swapped
//
// Revision 1.12  2005/06/21 14:56:15  edn2065
// Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
//
// Revision 1.11  2005/06/21 14:25:47  edn2065
// Fixed Principal point load/save
//
// Revision 1.10  2005/06/14 18:22:40  edn2065
// Implemented saving for calibration
//
// Revision 1.9  2005/06/14 16:06:41  edn2065
// Extended calibration algorithm
//
// Revision 1.8  2005/06/10 13:40:46  edn2065
// Added splicing of zoom out
//
// Revision 1.7  2005/06/10 12:10:53  edn2065
// Changed interpolation
//
// Revision 1.6  2005/06/09 18:35:18  edn2065
// ZoomHybrid compiles. Is no longer a subclass of DigitalZoom and OpticalZoom
//
// Revision 1.5  2005/06/09 18:16:27  edn2065
// Created ZoomHybrid as subclass of optical and digital
//
// Revision 1.4  2005/06/09 16:39:59  edn2065
// Fixed digital zoom inaccuracy
//
// Revision 1.3  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.2  2005/06/02 19:41:59  edn2065
// Writted and compiled. not tested.
//
// Revision 1.1  2005/06/02 18:30:46  edn2065
// Adding to eProject repository
//
// Revision 1.6  2005/03/03 23:42:52  edn2065
// Hybrid zoom works with princple point adjustment. removed debug code.
//
// Revision 1.5  2005/03/03 23:00:15  edn2065
// Hybrid zoom out works. Still has debug code.
//
// Revision 1.4  2005/03/02 22:04:52  edn2065
// Added principal point adjustment when zooming
//
// Revision 1.3  2005/03/02 19:23:15  edn2065
// Added adjustment for principal point
//
// Revision 1.2  2005/03/02 00:29:36  edn2065
// Digital zoom works
//
// Revision 1.1  2005/03/01 22:53:34  edn2065
// Initial revision
//



