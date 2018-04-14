#include "SIDualCamOptions.h"
#include <fstream>
#include <time.h>
using namespace std;

/*****************************************************************
 * SIDualCamOptions ctor
 *****************************************************************
 */
SIDualCamOptions::SIDualCamOptions()
{
  //Set defaults //////////
  arbitrateControl = false;
  arbitrateDisplay = false;
  minZoomMagZoomCam = 0.0;
  strcpy( panoramicCamOptions, "../../config/panoramic" );
  strcpy( zoomCamOptions, "../../config/panoramic" );    
  
  strcpy(vidDstDirectory, "../../vid/" );
  strcpy(vidDstPrefix, "def" );
  strcpy(vidDstSuffix, ".bmp" );
  vidDstSaveOrigStream = false;
  vidDstSaveDigStream = false;
  vidDstSaveSplicedStream = false;
  
  
  saveStats = false;
  
}

/*****************************************************************
 * SIDualCamOptions::loadOptions
 *****************************************************************
 */
void SIDualCamOptions::loadOptions( CHAR *fileName )
{
  printf( "Attempting load from %s" CRLF, fileName );

  //Indicate the validity of the file
  BOOL validFile = true;
  //Open the file
  ifstream loadFile( fileName, ios::in );
  // The file line buffer
  CHAR line[CO_BUFFER_LENGTH];
  //The variable from the line
  CHAR *varTok;
  //The value of the variable from the line
  CHAR *valueTok;
  //The delimiters for the token
  CHAR delim[] = {CO_DELIM, 0};
  //The new options to be saved
  SIDualCamOptions newOptions;

  //Set the new options so they are the same as this set of
  //options.
  newOptions.copy(this);

  //Make sure it can be opened
  if( !loadFile )
  {
    printf( "File could not be opened." CRLF );
  }
  else
  {
    try
    {
      //Set exceptions to be thrown so they can be caught
      loadFile.exceptions(ifstream::failbit | ifstream::badbit);

      //Read from the stream
      while( !loadFile.eof() && validFile )
      {
        //Get the next line
        loadFile.getline( line, CO_BUFFER_LENGTH );                

        //Only process if line is not a comment
        if( line[0] != CO_COMMENT )
        {
          //Get the variable
          varTok = strtok( line, delim );
          //Get the value
          valueTok = strtok( NULL, delim );
          //Save the variable
          validFile = setVariable( varTok, valueTok, &newOptions );
        }
      }
    }
    //Check for errors
    catch( ifstream::failure e )
    {
      if( !loadFile.eof() )
      {
        printf("File read error (%s)" CRLF, fileName );
        validFile = false;
      }
    }
  }
  //Clean up
  if( loadFile ) {
    if( loadFile.is_open() ) {
      loadFile.close();
      printf( "File closed." CRLF );
    }
  }

  //Copy the new options, if valid
  if( validFile )
  {
    copy(&newOptions);
    printf( "File successfully loaded." CRLF );
  }
  else
  {
    printf( "Error in file. Not loaded." CRLF );
  }
}

/*****************************************************************
 * SIDualCamOptions::setVariable
 *****************************************************************
 */
BOOL SIDualCamOptions::setVariable( CHAR *var, CHAR *value,
      SIDualCamOptions *newOptions )
{
  BOOL valid = true;
  //Determine the validity of the variable
  // Video Output /////////////
  if( strcmp( var, "arbitrateControl" ) == 0 )
  {
    newOptions->arbitrateControl = (BOOL) atoi( value );
  }
  else if( strcmp( var, "arbitrateDisplay" ) == 0 )
  {
    newOptions->arbitrateDisplay = (BOOL) atoi( value );
  }
  else if( strcmp( var, "minZoomMagZoomCam" ) == 0 )
  {
    newOptions->minZoomMagZoomCam = atof( value );
  }
  else if( strcmp( var, "panoramicCamOptions" ) == 0 )
  {
    strcpy(newOptions->panoramicCamOptions, value );
  }
  else if( strcmp( var, "zoomCamOptions" ) == 0 )
  {
    strcpy(newOptions->zoomCamOptions, value );
  }  
  
  else if( strcmp( var, "vidDstDirectory" ) == 0 )
  {
    strcpy(newOptions->vidDstDirectory, value );
  }    
  else if( strcmp( var, "vidDstPrefix" ) == 0 )
  {
    strcpy(newOptions->vidDstPrefix, value );
  }  
  else if( strcmp( var, "vidDstSuffix" ) == 0 )
  {
    strcpy(newOptions->vidDstSuffix, value );
  }  
  else if( strcmp( var, "vidDstSaveOrigStream" ) == 0 )
  {
    newOptions->vidDstSaveOrigStream = (BOOL) atoi( value );
  }
  else if( strcmp( var, "vidDstSaveDigStream" ) == 0 )
  {
    newOptions->vidDstSaveDigStream = (BOOL) atoi( value );
  }
  else if( strcmp( var, "vidDstSaveSplicedStream" ) == 0 )
  {
    newOptions->vidDstSaveSplicedStream = (BOOL) atoi( value );
  }      
  
  else if( strcmp( var, "saveStats" ) == 0 )
  {
    newOptions->saveStats = (BOOL) atoi( value );
  }      
    
  else
  {
    printf( "Unrecognized variable: %s" CRLF, var );
  }

  return valid;
}

/*****************************************************************
 * SIDualCamOptions::saveOptions
 *****************************************************************
 */
void SIDualCamOptions::saveOptions( CHAR *fileName )
{
  printf( "Attempting save to %s" CRLF, fileName );
  //Open the file
  ofstream saveFile( fileName, ios::out );

  //Make sure it can be opened
  if( !saveFile )
  {
    printf( "File could not be opened." CRLF );
  }
  else
  {
    try
    {
      //Set exceptions to be thrown so they can be caught
      saveFile.exceptions(ofstream::eofbit |
        ofstream::failbit | ofstream::badbit);

      //Write the header
      time_t rawtime;
      struct tm * timeinfo;
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );

      saveFile << CO_COMMENT
               << "This file generated on " << asctime (timeinfo);
      saveFile << CO_COMMENT
               << "by the Scale Invariant Tracker v" << VERSION << CRLF;
      saveFile << CO_COMMENT
               << "compiled on " << __DATE__ << " at " << __TIME__ << CRLF;


      //Save each parameter
      // Video Output /////////////
      saveFile << "arbitrateControl" << CO_DELIM << arbitrateControl << endl;
      saveFile << "arbitrateDisplay" << CO_DELIM << arbitrateDisplay << endl;
      saveFile << "minZoomMagZoomCam" << CO_DELIM << minZoomMagZoomCam << endl;      
      saveFile << "panoramicCamOptions" << CO_DELIM << panoramicCamOptions << endl;
      saveFile << "zoomCamOptions" << CO_DELIM << zoomCamOptions << endl;
      
      saveFile << "vidDstDirectory" << CO_DELIM << vidDstDirectory << endl;
      saveFile << "vidDstPrefix" << CO_DELIM << vidDstPrefix << endl;
      saveFile << "vidDstSuffix" << CO_DELIM << vidDstSuffix << endl;      
      saveFile << "vidDstSaveOrigStream" << CO_DELIM << vidDstSaveOrigStream << endl;
      saveFile << "vidDstSaveDigStream" << CO_DELIM << vidDstSaveDigStream << endl;
      saveFile << "vidDstSaveSplicedStream" << CO_DELIM 
               << vidDstSaveSplicedStream << endl;      
      
      saveFile << "saveStats" << CO_DELIM 
               << saveStats << endl;      
                                 
      printf( "File successfully saved." CRLF );

    }
    //Check for errors
    catch( ofstream::failure e )
    {
      printf("File write error (%s)" CRLF, fileName );

    }
  }
  //Clean up
  if( saveFile ) {
    if( saveFile.is_open() ) {
      saveFile.close();
      printf( "File closed." CRLF );
    }
  }
}

/*****************************************************************
 * SIDualCamOptions::copy
 *****************************************************************
 */
void SIDualCamOptions::copy(SIDualCamOptions const *that )
{
  this->arbitrateControl = that->arbitrateControl;
  this->arbitrateDisplay = that->arbitrateDisplay;
  this->minZoomMagZoomCam = that->minZoomMagZoomCam;  
  strcpy( this->panoramicCamOptions, that->panoramicCamOptions);
  strcpy( this->zoomCamOptions, that->zoomCamOptions); 
  
  strcpy(this->vidDstDirectory, that->vidDstDirectory );
  strcpy(this->vidDstPrefix, that->vidDstPrefix );
  strcpy(vidDstSuffix, that->vidDstSuffix );
  this->vidDstSaveOrigStream = that->vidDstSaveOrigStream;
  this->vidDstSaveDigStream = that->vidDstSaveDigStream;
  this->vidDstSaveSplicedStream = that->vidDstSaveSplicedStream;
  this->saveStats = that->saveStats;
}


// File: $Id: SIDualCamOptions.cpp,v 1.5 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: SIDualCamOptions.cpp,v $
// Revision 1.5  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.4  2005/09/10 02:42:48  edn2065
// Added minimum zoom option. Added timer to apply in menu.
//
// Revision 1.3  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.2  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.1  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
//
// ----------------------------------------------------------------------
//
