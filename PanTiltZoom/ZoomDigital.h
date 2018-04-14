#ifndef ZOOMDIGITAL_H
#define ZOOMDIGITAL_H

#include <stdio.h>
#include "cv.h"
#include "Zoom.h"
#include "VideoStream.h"


/**************************************************************
 *
 * ZoomDigital class
 * Description: Zooms in digitally on an image.
 *
 **************************************************************
 */
class ZoomDigital : public Zoom {

  public:  
   /******************************************
    * ctor
    * Description: Sets defaults. Allocates and registers zoomed image.
    ******************************************
    */  
    ZoomDigital();
    
   /******************************************
    * dtor
    * Description: Deallocates memory. Released zoomed image.
    ******************************************
    */      
    ~ZoomDigital();

   /******************************************
    * setPrincipalPoint
    * Description: Sets the principal point of zooming. 
    *              Defaults to the center of the image (0, 0).
    * Parameters:  ppX - the X principal point. The number of pixels to the 
    *                    right that the center of the zooming should occur.
    *              ppY - the Y principal point. The number of pixels to the 
    *                    bottom that the center of the zooming should occur.
    ******************************************
    */       
    void setPrincipalPoint( SINT16 ppX, SINT16 ppY );
   
   /******************************************
    * savePrincipalPoint
    * Description: Saves the principal point of zooming to a file.
    * Parameters:  filename - The file in which to save the principal point
    ******************************************
    */  
    void savePrincipalPoint(CHAR *filename);
   
   /******************************************
    * loadPrincipalPoint
    * Description: Loads the principal point of zooming from a file.
    * Parameters:  filename - The file from which to load the principal point
    ******************************************
    */  
    void loadPrincipalPoint(CHAR *filename);    
     
   /******************************************
    * setSpliceImage
    * Description: Sets the option of whether to splice the background and original images
    *              on zoom out.
    * Parameters:  mySpliceImage - True if the images are to be spliced.
    ******************************************
    */       
    void setSpliceOutput( BOOL mySpliceImage );   
   
   /******************************************
    * setOrigImg
    * Description: Sets the pointer to the captured images.
    * Parameters:  myOrigImg - the Image that will be zoomed on. Note that the
    *                          ROI of the image is changed every time zoom
    *                          is called.
    *              myBgImg   - the background image used on zoom outs when splicing is
    *                          enabled.
    ******************************************
    */           
    void setImages( IplImage *myOrigImg, IplImage *myBgImg );    
    void setBgImage(IplImage *myBgImg);
    
   /******************************************
    * getZoomedImg
    * Description: Retrieves the pointer to the zoomed image. Useful for display.
    * Returns:     The zoomed image.
    ******************************************
    */               
    IplImage *getZoomedImg();
    IplImage *getSplicedImg();
    
   /******************************************
    * registerVideo
    * Description: Tells the VideoStream object about the zoomed image
    * Parameters:  vStream - the VideoStream object
    ******************************************
    */                   
    void registerVideo( VideoStream *vStream );
  
   /******************************************
    * zoom
    * Description: Zooms the lens to a certain magnification (does 
    *              nothing). 
    * Parameters:  magnification - The new target magnification
    ******************************************
    */               
    virtual void zoom( FLOAT magnification );

   /******************************************
    * zoomImage
    * Description: Performs digital zoom on image of "magnification"X. Maintains
    *              the principal point of the zoom lens. This function is used for
    *              simulating the zoom lens.
    *              Changes ROI of origImg.
    * Parameters:  magnification - The new target magnification
    *              opticalZoom   - The amount of optical zoom
    *              in the latest frame (unused).
    ******************************************
    */   
    virtual void zoomImage( FLOAT magnification, FLOAT opticalZoom );
    
   /******************************************
    * zoomImage
    * Description: Performs digital zoom on image of "magnification"X. Ignores 
    *              principal point; places object at center of zoomed image. This
    *              function is used for digital fixation with digital zoom.
    *              Changes ROI of origImg.
    * Parameters:  magnification - The new target magnification
    *              opticalZoom   - The amount of optical zoom
    *              in the latest frame (unused).
    *              imgCenterX - The desired center of the digitally zoomed image
    *              imgCenterY - The desired center of the digitally zoomed image
    ******************************************
    */       
    virtual void zoomImage( FLOAT magnification, FLOAT opticalZoom, 
                             FLOAT imgCenterX, FLOAT imgCenterY );
   
   /******************************************
    * inquiry
    * Description: Returns the current zoom level
    * Parameters:  myZoomMag - Returned as the magnification
    * Returns:     True if inquiry successful
    ******************************************
    */
    virtual BOOL inquiry( FLOAT *myZoomMag );    
    
   /******************************************
    * reset
    * Description: Returns magnification to the default
    ******************************************
    */       
    virtual void reset();         

    
  private:
   /******************************************
    * reset
    * Description: Pastes one image within another image.
    * Parameters:  subImage - The image that will be inserted
    *              dest - The image that will have the inserting done to it.
    *              x - The x position of the insertion
    *              y - The y position of the insertion
    ******************************************
    */         
    void customInsert( const IplImage *subImage, IplImage *dest,
                                     SINT32 x, SINT32 y );
    void customInsert( const IplImage *subImage, IplImage *dest1, IplImage *dest2,
                                     SINT32 x, SINT32 y );

  protected:
    //The amount of digital magnification
    FLOAT digitalMag;
    
    //The pointer to the original image, pre-zoom
    IplImage *origImg;
    
    //The pointer to the panoramic camera's image, pre-zoom
    IplImage *bgImg;
     
    //The zoomed image. This object maintains this image.
    IplImage *zoomedImg;
    IplImage *splicedImg;
     
    //The region to be zoomed on in the image
    CvRect zoomROI;
    //What to reset the ROI to after zooming
    CvRect noROI;
    //The ROI when doing digital tracking
    CvRect trackingROI;
    
    //The principal point of the zoom; the center of the zoomed image w.r.t.
    //the center of the original image.  
    SINT16 principalPointX;
    SINT16 principalPointY;
    
    //True if the background image and the original image should be spliced together when zooming
    //out.
    BOOL spliceOutput;
  
};

#endif

// File: $Id: ZoomDigital.h,v 1.14 2005/09/06 20:41:09 edn2065 Exp $
// Author: Eric D Nelson
// Description: Zooms in digitally
// Revisions:
// $Log: ZoomDigital.h,v $
// Revision 1.14  2005/09/06 20:41:09  edn2065
// Added dead zone to digital zoom
//
// Revision 1.13  2005/09/06 19:44:17  edn2065
// Moved HZO barriers to Zoom to help parallelism
//
// Revision 1.12  2005/08/26 20:27:13  edn2065
// Pickoff points work. Implemented HZO camera setup
//
// Revision 1.11  2005/08/26 19:04:14  edn2065
// Pickoff points work, but framerate dropped
//
// Revision 1.10  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.9  2005/08/10 20:07:07  edn2065
// Digital zoom in fixation works. zoom out doesnt
//
// Revision 1.8  2005/06/21 14:56:15  edn2065
// Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
//
// Revision 1.7  2005/06/14 18:22:40  edn2065
// Implemented saving for calibration
//
// Revision 1.6  2005/06/14 16:06:41  edn2065
// Extended calibration algorithm
//
// Revision 1.5  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.4  2005/06/09 18:16:27  edn2065
// Created ZoomHybrid as subclass of optical and digital
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
// Revision 1.7  2005/03/03 23:43:11  edn2065
//  Hybrid zoom works with princple point adjustment. removed debug code.
//
// Revision 1.6  2005/03/03 23:00:57  edn2065
// Hybrid zoom out works. Still has debug code.
//
// Revision 1.5  2005/03/02 22:04:59  edn2065
// Added principal point adjustment when zooming
//
// Revision 1.4  2005/03/02 19:23:29  edn2065
// Added adjustment for principal point
//
// Revision 1.3  2005/03/02 00:29:44  edn2065
// Digital zoom works
//
// Revision 1.2  2005/03/01 22:54:14  edn2065
// Compiles. Zoom not implemented.
//
// Revision 1.1  2005/03/01 22:41:11  edn2065
// Initial revision
//
