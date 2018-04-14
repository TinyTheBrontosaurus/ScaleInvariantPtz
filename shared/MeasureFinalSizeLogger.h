#ifndef MEASUREFINALSIZELOGGER_H
#define MEASUREFINALSIZELOGGER_H

#include "types.h"
#include <fstream>
using namespace std;

#define MFS_DELIM ","

/**************************************************************
 *
 * MeasureFinalSizeLogger class
 * Description: Logs to a file the different parameters associated with 
 *              size an position for ever frame.
 *
 **************************************************************
 */
class MeasureFinalSizeLogger {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults. 
    ******************************************
    */ 
    MeasureFinalSizeLogger();
    
   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */     
    ~MeasureFinalSizeLogger();
  
  
   /******************************************
    * startFile
    * Description: Open the file. Closes the old file, if applicable.
    * Parameter:   fileName - The name of the file to be created
    ******************************************
    */     
    void startFile( CHAR *fileName );
               
   /******************************************
    * endFile
    * Description: Closes the file
    ******************************************
    */                
    void endFile();
  
   /******************************************
    * log
    * Description: Logs a frame to file
    * Parameter:   width - the width of the object
    *              height - the height of the object
    *              xPosition - The x-position of the center of the object
    *              yPosition - The y-position of the center of the object
    *              pan - The pan angle of the camera
    *              tilt - The tilt angle of the camera
    *              zoomOptical - The optical magnification of the camera
    *              zoomOpticalRaw - The optical magnification of the camera as 
    *                               reported by the camera.
    *              zoomDigital - The digital magnification of the camera
    ******************************************
    */   
    void log( UINT16 width, UINT16 height, FLOAT xPosition, FLOAT yPosition, 
      FLOAT pan, FLOAT tilt, FLOAT zoomOptical, UINT16 zoomOpticalRaw, FLOAT zoomDigital);
    void log(FLOAT xFixPan, FLOAT yFixPan, FLOAT xFixKalPan, 
      FLOAT yFixKalPan, FLOAT zFixKalPan, FLOAT widPan, FLOAT hgtPan, FLOAT panAnglePan, 
      FLOAT tiltAnglePan, UINT16 optZoomRawPan, FLOAT optZoomPan, FLOAT digZoomPan,
      FLOAT xFixZoom, FLOAT yFixZoom, FLOAT xFixKalZoom, FLOAT yFixKalZoom, 
      FLOAT zFixKalZoom, FLOAT widZoom, FLOAT hgtZoom, FLOAT panAngleZoom, 
      FLOAT tiltAngleZoom, UINT16 optZoomRawZoom, FLOAT optZoomZoom, FLOAT digZoomZoom,
      BOOL zoomCamUsed, FLOAT finalOptZoom, FLOAT finalDigZoom );
      
   /******************************************
    * noLog
    * Description: Increments the number of frames passed and outputs that to file,
    *              but does not output any parameters.
    ******************************************
    */               
    void noLog();
   
  private:
    
  private:
    //The file to be saved
    fstream logFile;
    
    //The number of frames logged
    UINT32 currentFrameNumber;
};

#endif

// File: $Id: MeasureFinalSizeLogger.h,v 1.7 2005/09/10 01:48:01 edn2065 Exp $
// Author: Eric D Nelson
// Description: Creates a lookup table from int's to FLOATs.
// Revisions:
// $Log: MeasureFinalSizeLogger.h,v $
// Revision 1.7  2005/09/10 01:48:01  edn2065
// Expanded MFS again to include final image stats
//
// Revision 1.6  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.5  2005/06/14 12:19:58  edn2065
// Made PT inquiry a float instead of int. Camera resolution was being thrown away
//
// Revision 1.4  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.3  2005/06/10 17:03:47  edn2065
// Added MFS
//
// Revision 1.2  2005/06/10 15:26:04  edn2065
// Created and debugged MFS
//
// Revision 1.1  2005/06/10 14:28:47  edn2065
// Adding to repository
//

