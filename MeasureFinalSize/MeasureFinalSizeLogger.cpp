#include "MeasureFinalSizeLogger.h"

/*****************************************************************
 * MeasureFinalSizeLogger ctor
 *****************************************************************
 */
MeasureFinalSizeLogger::MeasureFinalSizeLogger()
{
  //Nothing
}

/*****************************************************************
 * MeasureFinalSizeLogger dtor
 *****************************************************************
 */
MeasureFinalSizeLogger::~MeasureFinalSizeLogger()
{
  endFile();
}

/*****************************************************************
 * MeasureFinalSizeLogger::startFile
 *****************************************************************
 */
void MeasureFinalSizeLogger::startFile( CHAR *fileName)
{
  //Reset the number of frames
  currentFrameNumber = 0;

  //Close the file if it is already open
  if( logFile.is_open() )
  {
    endFile();
  }

  //Open the file
  logFile.open( fileName, ios::out );
  
  if( !logFile )
  {
    printf( "File could not be opened: %s (%s @ %d)"CRLF, fileName, 
      __FILE__, __LINE__ ); 
  }
  else
  {
    //Set exceptions to be thrown so they can be caught
    logFile.exceptions(ofstream::eofbit |
      ofstream::failbit | ofstream::badbit);
    
    try
    {
      logFile << "Frame #"                            << MFS_DELIM
      
              << "X-fix error (pan)"                  << MFS_DELIM
              << "Y-fix error (pan)"                  << MFS_DELIM
              << "X-Kalman est (pan)"                 << MFS_DELIM
              << "Y-Kalman est (pan)"                 << MFS_DELIM
              << "Z-Kalman est (pan)"                 << MFS_DELIM
              << "Width (pan)"                        << MFS_DELIM
              << "Height (pan)"                       << MFS_DELIM
              << "Pan angle (pan)"                    << MFS_DELIM
              << "Tilt angle (pan)"                   << MFS_DELIM
              << "Opt raw zoom mag (pan)"             << MFS_DELIM
              << "Opt zoom mag (pan)"                 << MFS_DELIM              
              << "Dig zoom mag (pan)"                 << MFS_DELIM
                                                      << MFS_DELIM
              << "X-fix error (zoom)"                 << MFS_DELIM
              << "Y-fix error (zoom)"                 << MFS_DELIM
              << "X-Kalman est (zoom)"                << MFS_DELIM
              << "Y-Kalman est (zoom)"                << MFS_DELIM
              << "Z-Kalman est (zoom)"                << MFS_DELIM
              << "Width (zoom)"                       << MFS_DELIM
              << "Height (zoom)"                      << MFS_DELIM
              << "Pan angle (zoom)"                   << MFS_DELIM
              << "Tilt angle (zoom)"                  << MFS_DELIM
              << "Opt raw zoom mag (zoom)"            << MFS_DELIM
              << "Opt zoom mag (zoom)"                << MFS_DELIM              
              << "Dig zoom mag (zoom)"                << MFS_DELIM
                                                      << MFS_DELIM
              << "Zoom Cam Used"                      << MFS_DELIM              
              << "Opt zoom mag (final)"               << MFS_DELIM              
              << "Dig zoom mag (final)"        
              << CRLF;              
              
    }    
    //Check for errors
    catch( ofstream::failure e )
    {
      printf("MFS File write error." CRLF );
      endFile();  
    }      
  }               
}

/*****************************************************************
 * MeasureFinalSizeLogger::endFile
 *****************************************************************
 */
void MeasureFinalSizeLogger::endFile()
{
  //Clean up
  if( logFile ) {
    if( logFile.is_open() ) {
      logFile.close();
      printf( "MFS file closed." CRLF );
    }
  }
}

  
/*****************************************************************
 * MeasureFinalSizeLogger::log
 *****************************************************************
 */
void MeasureFinalSizeLogger::log(FLOAT xFixPan, FLOAT yFixPan, FLOAT xFixKalPan, 
  FLOAT yFixKalPan, FLOAT zFixKalPan, FLOAT widPan, FLOAT hgtPan, FLOAT panAnglePan, 
  FLOAT tiltAnglePan, UINT16 optZoomRawPan, FLOAT optZoomPan, FLOAT digZoomPan,
  FLOAT xFixZoom, FLOAT yFixZoom, FLOAT xFixKalZoom, FLOAT yFixKalZoom, 
  FLOAT zFixKalZoom, FLOAT widZoom, FLOAT hgtZoom, FLOAT panAngleZoom, 
  FLOAT tiltAngleZoom, UINT16 optZoomRawZoom, FLOAT optZoomZoom, FLOAT digZoomZoom,
  BOOL zoomCamUsed, FLOAT finalOptZoom, FLOAT finalDigZoom )
{
 
  try
  {
    logFile << currentFrameNumber++ << MFS_DELIM 
            << xFixPan              << MFS_DELIM
            << yFixPan              << MFS_DELIM
            << xFixKalPan           << MFS_DELIM
            << yFixKalPan           << MFS_DELIM
            << zFixKalPan           << MFS_DELIM
            << widPan               << MFS_DELIM
            << hgtPan               << MFS_DELIM
            << panAnglePan          << MFS_DELIM
            << tiltAnglePan         << MFS_DELIM
            << optZoomRawPan        << MFS_DELIM
            << optZoomPan           << MFS_DELIM
            << digZoomPan           << MFS_DELIM
                                    << MFS_DELIM
            << xFixZoom             << MFS_DELIM
            << yFixZoom             << MFS_DELIM
            << xFixKalZoom          << MFS_DELIM
            << yFixKalZoom          << MFS_DELIM
            << zFixKalZoom          << MFS_DELIM
            << widZoom              << MFS_DELIM
            << hgtZoom              << MFS_DELIM
            << panAngleZoom         << MFS_DELIM
            << tiltAngleZoom        << MFS_DELIM
            << optZoomRawZoom       << MFS_DELIM
            << optZoomZoom          << MFS_DELIM
            << digZoomZoom          << MFS_DELIM
                                    << MFS_DELIM
            << zoomCamUsed          << MFS_DELIM
            << finalOptZoom         << MFS_DELIM
            << finalDigZoom                   
            << CRLF;
  
  }
  //Check for errors
  catch( ofstream::failure e )
  {
    printf("MFS File write error." CRLF );
    endFile();  
  }

}

  
/*****************************************************************
 * MeasureFinalSizeLogger::log
 *****************************************************************
 */
void MeasureFinalSizeLogger::log( UINT16 width, UINT16 height, FLOAT xPosition, FLOAT yPosition, 
  FLOAT pan, FLOAT tilt, FLOAT zoomOptical, UINT16 zoomOpticalRaw, FLOAT zoomDigital)
{
 
  printf( "Function obsolete! (%s @ %d)" CRLF, __FILE__, __LINE__ );
  
  try
  {
    logFile << currentFrameNumber++ << MFS_DELIM 
            << width              << MFS_DELIM 
            << height             << MFS_DELIM 
            << width*height       << MFS_DELIM 
            << xPosition          << MFS_DELIM 
            << yPosition          << MFS_DELIM 
            << sqrt( (xPosition)*(xPosition) + (yPosition)*(yPosition))  << MFS_DELIM 
            << pan                << MFS_DELIM 
            << tilt               << MFS_DELIM 
            << zoomOptical        << MFS_DELIM 
            << zoomOpticalRaw     << MFS_DELIM 
            << zoomDigital        << CRLF;
  
  }
  //Check for errors
  catch( ofstream::failure e )
  {
    printf("MFS File write error." CRLF );
    endFile();  
  }

}
   
/*****************************************************************
 * MeasureFinalSizeLogger::noLog
 *****************************************************************
 */
void MeasureFinalSizeLogger::noLog()
{

  try
  {
    logFile << currentFrameNumber++ << CRLF; 
  }
  //Check for errors
  catch( ofstream::failure e )
  {
    printf("MFS File write error." CRLF );
    endFile();  
  }

}

// File: $Id: MeasureFinalSizeLogger.cpp,v 1.7 2005/09/10 01:48:01 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: MeasureFinalSizeLogger.cpp,v $
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
