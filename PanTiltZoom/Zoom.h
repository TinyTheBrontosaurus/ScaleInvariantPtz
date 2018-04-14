
#ifndef ZOOM_H
#define ZOOM_H

#include "types.h"
#include "Barrier.h"

/**************************************************************
 *
 * Zoom class (abstract)
 * Description: Serves as an interface for the various types of zoom.
 *
 **************************************************************
 */
class Zoom {
    
  public:
  
     void initBarriers( Barrier *myHzoPreZoomPrePickoffBarrier,
           Barrier *myHzoPreZoomPostPickoffBarrier)
     {
       hzoPreZoomPrePickoffBarrier = myHzoPreZoomPrePickoffBarrier;
       hzoPreZoomPostPickoffBarrier = myHzoPreZoomPostPickoffBarrier;
     }
  
   /******************************************
    * zoom
    * Description: Zooms the lens to a certain magnification
    * Parameters:  magnification - The new target magnification
    ******************************************
    */   
    virtual void zoom( FLOAT magnification ) = 0;
    
   /******************************************
    * zoomImage
    * Description: Performs digital zoom on image
    * Parameters:  magnification - The new target magnification
    *              opticalZoom   - The amount of optical zoom
    *              in the latest frame.
    ******************************************
    */   
    virtual void zoomImage( FLOAT magnification, FLOAT opticalZoom )
    {
      hzoPreZoomPrePickoffBarrier->wait();
      hzoPreZoomPostPickoffBarrier->wait();
    }   
    virtual void zoomImage( FLOAT magnification, FLOAT opticalZoom, 
                             FLOAT imgCenterX, FLOAT imgCenterY )
    {
      hzoPreZoomPrePickoffBarrier->wait();
      hzoPreZoomPostPickoffBarrier->wait();
    } 
  
   /******************************************
    * inquiry
    * Description: Returns the current zoom level
    * Parameters:  myZoomMag - Returned as the magnification
    * Returns:     True if inquiry successful
    ******************************************
    */
    virtual BOOL inquiry( FLOAT *myZoomMag ) = 0; 

   /******************************************
    * reset
    * Description: Returns magnification to the default
    ******************************************
    */       
    virtual void reset() = 0;
    
  protected:
  
    Barrier *hzoPreZoomPrePickoffBarrier;
    Barrier *hzoPreZoomPostPickoffBarrier;  
};

#endif

/*
 * Zoom.h
 *
 * Description: Serves as an interface for the various types of zoom.
 *
 * Authors: Eric D Nelson
 *
 * Version:
 *     $Id: Zoom.h,v 1.7 2005/09/06 19:44:17 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Zoom.h,v $
 *     Revision 1.7  2005/09/06 19:44:17  edn2065
 *     Moved HZO barriers to Zoom to help parallelism
 *
 *     Revision 1.6  2005/08/18 03:07:22  edn2065
 *     Fixed digital zoom and digital fixate bug
 *
 *     Revision 1.5  2005/08/10 20:07:07  edn2065
 *     Digital zoom in fixation works. zoom out doesnt
 *
 *     Revision 1.4  2005/06/21 14:56:15  edn2065
 *     Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
 *
 *     Revision 1.3  2005/06/09 18:16:27  edn2065
 *     Created ZoomHybrid as subclass of optical and digital
 *
 *     Revision 1.2  2005/06/02 15:58:44  edn2065
 *     Getting ready to create CameraComms class
 *
 *     Revision 1.1  2005/06/01 18:46:23  edn2065
 *     Create Zoom, now need to edit LookupTable before testing
 *
 *
 *
 */
