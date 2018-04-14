#include "ZoomHybrid.h"

/*****************************************************************
 * ZoomHybrid ctor
 *****************************************************************
 */
 ZoomHybrid::ZoomHybrid()
{
  initialized = false;  
}

/*****************************************************************
 * ZoomHybrid dtor
 *****************************************************************
 */
ZoomHybrid::~ZoomHybrid()
{
  //Do nothing. base dtors should take care of everything
}

/*****************************************************************
 * ZoomHybrid::init
 *****************************************************************
 */
void ZoomHybrid::init( ZoomOptical *myOptical, ZoomDigital *myDigital )
{
  initialized = true;
  zoomOptical = myOptical;
  zoomDigital = myDigital;
}

/*****************************************************************
 * ZoomHybrid::zoom
 *****************************************************************
 */
void ZoomHybrid::zoom( FLOAT magnification )
{
  if( initialized )
  {
    zoomOptical->zoom( magnification);
  }
  else
  {
    printf( "Error! ZoomHybrid not initialized (%s @ %d)"CRLF,
      __FILE__,__LINE__);
  }
}

/*****************************************************************
 * ZoomHybrid::zoomImage
 *****************************************************************
 */
void ZoomHybrid::zoomImage( FLOAT magnification, FLOAT myOpticalZoomMag )
{
  if( initialized )
  {
    zoomDigital->zoomImage( magnification,myOpticalZoomMag );
  }
  else
  {
    printf( "Error! ZoomHybrid not initialized (%s @ %d)"CRLF,
      __FILE__,__LINE__);
  }  
}

/*****************************************************************
 * ZoomHybrid::zoomImage
 *****************************************************************
 */
void ZoomHybrid::zoomImage( FLOAT magnification, FLOAT myOpticalZoomMag, 
                             FLOAT imgCenterX, FLOAT imgCenterY  )
{
  if( initialized )
  {
    zoomDigital->zoomImage( magnification, myOpticalZoomMag, 
                            imgCenterX, imgCenterY );
  }
  else
  {
    printf( "Error! ZoomHybrid not initialized (%s @ %d)"CRLF,
      __FILE__,__LINE__);
  }  
}

/*****************************************************************
 * ZoomHybrid::inquiry
 *****************************************************************
 */
BOOL ZoomHybrid::inquiry(FLOAT *myZoomMag)
{
  printf( "Error: inquiry of ZoomHybrid (%s @ %d)"CRLF, __FILE__, __LINE__);
  return false;
}

/*****************************************************************
 * ZoomDigital::reset
 *****************************************************************
 */    
void ZoomHybrid::reset()
{
  if( initialized )
  {
    zoomOptical->reset();
    zoomDigital->reset();  
  }
  else
  {
    printf( "Error! ZoomHybrid not initialized (%s @ %d)"CRLF,
      __FILE__,__LINE__);
  }  
}

// File: $Id: ZoomHybrid.cpp,v 1.7 2005/09/06 00:08:00 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: ZoomHybrid.cpp,v $
// Revision 1.7  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.6  2005/08/19 01:32:26  edn2065
// Fixed digital mag problem
//
// Revision 1.5  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.4  2005/06/21 14:56:15  edn2065
// Now checks for bad zoomOptical inquiry. Does no fixation if that is the case
//
// Revision 1.3  2005/06/09 18:47:40  edn2065
// Implemented Hybrid zoom
//
// Revision 1.2  2005/06/09 18:35:18  edn2065
// ZoomHybrid compiles. Is no longer a subclass of DigitalZoom and OpticalZoom
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



