#ifndef ZOOMHYBRID_H
#define ZOOMHYBRID_H

#include "Zoom.h"
#include "ZoomDigital.h"
#include "ZoomOptical.h"

/**************************************************************
 *
 * ZoomHybrid class
 * Description: Zooms in digitally and optically on an image.
 *
 **************************************************************
 */
class ZoomHybrid : public Zoom {

  public:  
   /******************************************
    * ctor
    * Description: Sets defaults. Allocates and registers zoomed image.
    ******************************************
    */  
    ZoomHybrid();
    
   /******************************************
    * dtor
    * Description: Deallocates memory. Released zoomed image.
    ******************************************
    */      
    ~ZoomHybrid();
    
   /******************************************
    * init
    * Description: Sets up the optical and digital pieces of the zoom.
    * Parameters:  myOptical - pointer to the optical zoom lens control.
    *              myDigital - pointer to the digital zoom control.
    ******************************************
    */      
    void init( ZoomOptical *myOptical, ZoomDigital *myDigital );    
    
   /******************************************
    * zoom
    * Description: Zooms the lens to a certain magnification by sending
    *              command to camera.    
    * Parameters:  magnification - The new target magnification
    ******************************************
    */               
    virtual void zoom( FLOAT magnification );

   /******************************************
    * zoomImage
    * Description: Performs digital zoom on image of 
    *              "magnification"/"optical zoom"X.
    *              Changes ROI of origImg.
    * Parameters:  magnification - The new target magnification
    *              myOpticalZoomMag - The amount of optical zoom
    *              in the latest frame.
    ******************************************
    */   
    virtual void zoomImage( FLOAT magnification, FLOAT myOpticalZoomMag );
    virtual void zoomImage( FLOAT magnification, FLOAT opticalZoom, 
                            FLOAT imgCenterX, FLOAT imgCenterY );
    
   /******************************************
    * reset
    * Description: Returns magnification to the default
    ******************************************
    */       
    virtual void reset();  
    
   /******************************************
    * inquiry
    * Description: Returns the current zoom level
    * Parameters:  myZoomMag - Returned as the magnification
    * Returns:     True if inquiry successful
    ******************************************
    */
    virtual BOOL inquiry( FLOAT *myZoomMag );        

  private:
    //Only true if the digital and optical zoom lenses are set up
    BOOL initialized;
    
    //The digital part of hybrid zoom
    ZoomDigital *zoomDigital;
    
    //The optical part of hybrid zoom
    ZoomOptical *zoomOptical;
      
};

#endif

// File: $Id: ZoomHybrid.h,v 1.5 2005/09/06 19:44:17 edn2065 Exp $
// Author: Eric D Nelson
// Description: Zooms in digitally and optically
// Revisions:
// $Log: ZoomHybrid.h,v $
// Revision 1.5  2005/09/06 19:44:17  edn2065
// Moved HZO barriers to Zoom to help parallelism
//
// Revision 1.4  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.3  2005/06/21 14:56:15  edn2065
// Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
//
// Revision 1.2  2005/06/09 18:35:18  edn2065
// ZoomHybrid compiles. Is no longer a subclass of DigitalZoom and OpticalZoom
//
