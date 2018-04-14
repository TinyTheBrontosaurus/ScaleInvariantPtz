#include "SISingleCamOptions.h"
#include <fstream>
#include <time.h>
using namespace std;

/*****************************************************************
 * SISingleCamOptions ctor
 *****************************************************************
 */
SISingleCamOptions::SISingleCamOptions()
{
  //Set defaults //////////

  //Video destination
  vidDst = DST_NONE;
  strcpy( vidDstDirectory, "/home/edn2065/SI/vid/" );
  strcpy( vidDstPrefix, "vid" );
  vidDstNumDigits = 5;
  strcpy( vidDstSuffix, "jpg" );
  vidDstPreventLine = false;

  //Video Source
  vidSrc = SRC_NONE;
  strcpy( vidSrcDirectory, "/dev/v4l/" );
  strcpy( vidSrcFile, "video0" );
  strcpy( vidSrcPrefix, "/home/edn2065/SI/vid/vid" );
  vidSrcNumDigits = 5;
  strcpy( vidSrcSuffix, "jpg" );
  vidSrcStart = 0;
  vidSrcLoop = false;

  //Display options
  showFPS = false;
  showTrackingBoxes = false;
  numTrackingBoxes = MAX_ROIS;
  videoStreamOutput = 0;

  //Tracking options
  trackingTrkr = TRKR_NONE;
  trackingTrkrColor = TRKR_COLOR_RED;
  track = false;
  fixate = false;
  scaleInv = false;
  opticalZoom = false;
  digitalZoom = false;
  spliceZoomOut = false;
  fixateDigital = false;
  fixationAlg = FIX_P;
  kalmanProcessNoise = 0.03;
  kalmanMeasurementNoise = 0.02;
  fixationGain = 1.0;
  tordoffPsi = 0.5;
  tordoffGamma1 = 0.25;
  tordoffGamma2 = 0.025;
  zoomCoeff = CAM_ZOOM_COEFF;
  zoomDelayCoeff = 1.0;
  targetAreaOpticalZoom = 10000;
  targetAreaDigitalZoom = 10000;
  tordoffErrToHigherZoom = false;
    
  //MFS options
  mfsTrkr = TRKR_NONE;
  mfsTrkrColor = TRKR_COLOR_RED;
  mfs = false;
  strcpy( mfsDirectory, "/home/edn2065/SI/mfsLog/");
  strcpy( mfsFile, "mfsData");

  //Camera comms
  strcpy( serialPort, "/dev/ttyS0" );
  //Pan options
  panPos1 = -10.2;
  panPos2 = 72.3;
  panIterations = 5;
  panPulseFrames = PAN_PULSE_FRAMES_DEFAULT;
  panInquiry = false;

  //Tilt options
  tiltPos1 = 71.17;
  tiltPos2 = -31.71;
  tiltIterations = 7;
  tiltPulseFrames = TILT_PULSE_FRAMES_DEFAULT;
  tiltInquiry = false;

  //Zoom options
  zoomPos1 = 1.2;
  zoomPos2 = 8.9;
  zoomIterations = 9;
  zoomPulseFrames = ZOOM_PULSE_FRAMES_DEFAULT;
  zoomInquiry = false;

  //Calibration options
  strcpy( calDirectory, "/home/edn2065/SI/calibrate/");
  strcpy( calFile, "lookTable" );
  calTableLength = 10;

  //Config options
  strcpy( conDirectory, "/home/edn2065/SI/config/");
  strcpy( conFile, "last" );
}

/*****************************************************************
 * SISingleCamOptions::loadOptions
 *****************************************************************
 */
void SISingleCamOptions::loadOptions( CHAR *fileName )
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
  SISingleCamOptions newOptions;

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
 * SISingleCamOptions::setVariable
 *****************************************************************
 */
BOOL SISingleCamOptions::setVariable( CHAR *var, CHAR *value,
      SISingleCamOptions *newOptions )
{
  BOOL valid = true;
  //Determine the validity of the variable
  // Video Output /////////////
  if( strcmp( var, "vidDst" ) == 0 )
  {
    newOptions->vidDst = (VideoDstType) atoi( value );
  }
  else if( strcmp( var, "vidDstDirectory" ) == 0 )
  {
    strcpy(newOptions->vidDstDirectory, value );
  }
  else if( strcmp( var, "vidDstPrefix" ) == 0 )
  {
    strcpy(newOptions->vidDstPrefix, value );
  }
  else if( strcmp( var, "vidDstNumDigits" ) == 0 )
  {
    newOptions->vidDstNumDigits = (UINT8) atoi( value );
  }
  else if( strcmp( var, "vidDstSuffix" ) == 0 )
  {
    strcpy(newOptions->vidDstSuffix, value );
  }
  else if( strcmp( var, "vidDstPreventLine" ) == 0 )
  {
    newOptions->vidDstPreventLine = (BOOL) atoi( value );
  }

  //Video Input //////////////
  else if( strcmp( var, "vidSrc" ) == 0 )
  {
    newOptions->vidSrc = (VideoSrcType) atoi( value );
  }
  else if( strcmp( var, "vidSrcDirectory" ) == 0 )
  {
    strcpy(newOptions->vidSrcDirectory, value );
  }
  else if( strcmp( var, "vidSrcFile" ) == 0 )
  {
    strcpy(newOptions->vidSrcFile, value );
  }
  else if( strcmp( var, "vidSrcPrefix" ) == 0 )
  {
    strcpy(newOptions->vidSrcPrefix, value );
  }
  else if( strcmp( var, "vidSrcNumDigits" ) == 0 )
  {
    newOptions->vidSrcNumDigits = (UINT8) atoi( value );
  }
  else if( strcmp( var, "vidSrcSuffix" ) == 0 )
  {
    strcpy(newOptions->vidSrcSuffix, value );
  }
  else if( strcmp( var, "vidSrcStart" ) == 0 )
  {
    newOptions->vidSrcStart = (UINT32) atoi( value );
  }
  else if( strcmp( var, "vidSrcLoop" ) == 0 )
  {
    newOptions->vidSrcLoop = (BOOL) atoi( value );
  }

  // Misc Display
  else if( strcmp( var, "showFPS" ) == 0 )
  {
    newOptions->showFPS = (BOOL) atoi( value );
  }
  else if( strcmp( var, "showTrackingBoxes" ) == 0 )
  {
    newOptions->showTrackingBoxes = (BOOL) atoi( value );
  }
  else if( strcmp( var, "numTrackingBoxes" ) == 0 )
  {
    newOptions->numTrackingBoxes = (UINT8) atoi( value );
  }  
  else if( strcmp( var, "videoStreamOutput" ) == 0 )
  {
    newOptions->videoStreamOutput = (UINT8) atoi( value );
  }  
  //Tracking options
  else if( strcmp( var, "trackingTrkr" ) == 0 )
  {
    newOptions->trackingTrkr = (TrackerType) atoi( value );
  }
  else if( strcmp( var, "trackingTrkrColor" ) == 0 )
  {
    newOptions->trackingTrkrColor = (TrackerColorType) atoi( value );
  }  
  else if( strcmp( var, "track" ) == 0 )
  {
    newOptions->track = (BOOL) atoi( value );
  }
  else if( strcmp( var, "fixate" ) == 0 )
  {
    newOptions->fixate = (BOOL) atoi( value );
  }
  else if( strcmp( var, "scaleInv" ) == 0 )
  {
    newOptions->scaleInv = (BOOL) atoi( value );
  }
  else if( strcmp( var, "opticalZoom" ) == 0 )
  {
    newOptions->opticalZoom = (BOOL) atoi( value );
  }
  else if( strcmp( var, "digitalZoom" ) == 0 )
  {
    newOptions->digitalZoom = (BOOL) atoi( value );
  }
  else if( strcmp( var, "spliceZoomOut" ) == 0 )
  {
    newOptions->spliceZoomOut = (BOOL) atoi( value );
  }
  else if( strcmp( var, "fixateDigital" ) == 0 )
  {
    newOptions->fixateDigital = (BOOL) atoi( value );
  }
  else if( strcmp( var, "fixationAlg" ) == 0 )
  {
    newOptions->fixationAlg = (FixationType) atoi( value );
  }
  else if( strcmp( var, "kalmanProcessNoise" ) == 0 )
  {
    newOptions->kalmanProcessNoise = atof( value );
  }
  else if( strcmp( var, "kalmanMeasurementNoise" ) == 0 )
  {
    newOptions->kalmanMeasurementNoise = atof( value );
  }  
  else if( strcmp( var, "fixationGain" ) == 0 )
  {
    newOptions->fixationGain = atof( value );
  }  
  else if( strcmp( var, "tordoffPsi" ) == 0 )
  {
    newOptions->tordoffPsi = atof( value );
  }  
  else if( strcmp( var, "tordoffGamma1" ) == 0 )
  {
    newOptions->tordoffGamma1 = atof( value );
  }  
  else if( strcmp( var, "tordoffGamma2" ) == 0 )
  {
    newOptions->tordoffGamma2 = atof( value );
  }  
  else if( strcmp( var, "zoomCoeff" ) == 0 )
  {
    newOptions->zoomCoeff = atof( value );
  }     
  else if( strcmp( var, "zoomDelayCoeff" ) == 0 )
  {
    newOptions->zoomDelayCoeff = atof( value );
  }     
  else if( strcmp( var, "targetAreaOpticalZoom" ) == 0 )
  {
    newOptions->targetAreaOpticalZoom = (UINT32)atoi( value );
  }     
  else if( strcmp( var, "targetAreaDigitalZoom" ) == 0 )
  {
    newOptions->targetAreaDigitalZoom = (UINT32)atoi( value );
  }     
  else if( strcmp( var, "tordoffErrToHigherZoom" ) == 0 )
  {
    newOptions->tordoffErrToHigherZoom = (BOOL)atoi( value );
  }       
      
  //Measure final size options
  else if( strcmp( var, "mfs" ) == 0 )
  {
    newOptions->mfs = (BOOL) atoi( value );
  }
  else if( strcmp( var, "mfsTrkr" ) == 0 )
  {
    newOptions->mfsTrkr = (TrackerType) atoi( value );
  }
  else if( strcmp( var, "mfsTrkrColor" ) == 0 )
  {
    newOptions->mfsTrkrColor = (TrackerColorType) atoi( value );
  }  
  else if( strcmp( var, "mfsDirectory" ) == 0 )
  {
    strcpy(newOptions->mfsDirectory, value );
  }
  else if( strcmp( var, "mfsFile" ) == 0 )
  {
    strcpy(newOptions->mfsFile, value );
  }

  //Camera communication
  else if( strcmp( var, "serialPort" ) == 0 )
  {
    strcpy(newOptions->serialPort, value );
  }
  
  //Pan options
  else if( strcmp( var, "panPos1" ) == 0 )
  {
    newOptions->panPos1 = atof( value );
  }
  else if( strcmp( var, "panPos2" ) == 0 )
  {
    newOptions->panPos2 = atof( value );
  }
  else if( strcmp( var, "panIterations" ) == 0 )
  {
    newOptions->panIterations = (UINT32) atoi( value );
  }
  else if( strcmp( var, "panPulseFrames" ) == 0 )
  {
    newOptions->panPulseFrames = (UINT32) atoi( value );
  }    
  else if( strcmp( var, "panInquiry" ) == 0 )
  {
    newOptions->panInquiry = (BOOL) atoi( value );
  }
  
  //Tilt options
  else if( strcmp( var, "tiltPos1" ) == 0 )
  {
    newOptions->tiltPos1 = atof( value );
  }
  else if( strcmp( var, "tiltPos2" ) == 0 )
  {
    newOptions->tiltPos2 = atof( value );
  }
  else if( strcmp( var, "tiltIterations" ) == 0 )
  {
    newOptions->tiltIterations = (UINT32) atoi( value );
  }
  else if( strcmp( var, "tiltPulseFrames" ) == 0 )
  {
    newOptions->tiltPulseFrames = (UINT32) atoi( value );
  }      
  else if( strcmp( var, "tiltInquiry" ) == 0 )
  {
    newOptions->tiltInquiry = (BOOL) atoi( value );
  }
  
  //Zoom options
  else if( strcmp( var, "zoomPos1" ) == 0 )
  {
    newOptions->zoomPos1 = atof( value );
  }
  else if( strcmp( var, "zoomPos2" ) == 0 )
  {
    newOptions->zoomPos2 = atof( value );
  }
  else if( strcmp( var, "zoomIterations" ) == 0 )
  {
    newOptions->zoomIterations = (UINT32) atoi( value );
  }
  else if( strcmp( var, "zoomPulseFrames" ) == 0 )
  {
    newOptions->zoomPulseFrames = (UINT32) atoi( value );
  }      
  else if( strcmp( var, "zoomInquiry" ) == 0 )
  {
    newOptions->zoomInquiry = (BOOL) atoi( value );
  }

  //Calibration options
  else if( strcmp( var, "calDirectory" ) == 0 )
  {
    strcpy(newOptions->calDirectory, value );
  }
  else if( strcmp( var, "calFile" ) == 0 )
  {
    strcpy(newOptions->calFile, value );
  }
  else if( strcmp( var, "calTableLength" ) == 0 )
  {
    newOptions->calTableLength = (UINT32) atoi( value );
  }

  //Load/Save configuration options
  else if( strcmp( var, "conDirectory" ) == 0 )
  {
    strcpy(newOptions->conDirectory, value );
  }
  else if( strcmp( var, "conFile" ) == 0 )
  {
    strcpy(newOptions->conFile, value );
  }
  else
  {
    printf( "Unrecognized variable: %s" CRLF, var );
    //valid = false;
  }

  return valid;
}

/*****************************************************************
 * SISingleCamOptions::saveOptions
 *****************************************************************
 */
void SISingleCamOptions::saveOptions( CHAR *fileName )
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
      saveFile << "vidDst" << CO_DELIM << vidDst << endl;
      saveFile << "vidDstDirectory" << CO_DELIM << vidDstDirectory << endl;
      saveFile << "vidDstPrefix" << CO_DELIM << vidDstPrefix << endl;
      saveFile << "vidDstNumDigits" << CO_DELIM << (UINT16)vidDstNumDigits << endl;
      saveFile << "vidDstSuffix" << CO_DELIM << vidDstSuffix << endl;
      saveFile << "vidDstPreventLine" << CO_DELIM << vidDstPreventLine << endl;

      //Video Input //////////////
      saveFile << "vidSrc" << CO_DELIM << vidSrc << endl;
      saveFile << "vidSrcDirectory" << CO_DELIM << vidSrcDirectory << endl;
      saveFile << "vidSrcFile" << CO_DELIM << vidSrcFile << endl;
      saveFile << "vidSrcPrefix" << CO_DELIM << vidSrcPrefix << endl;
      saveFile << "vidSrcNumDigits" << CO_DELIM << (UINT16)vidSrcNumDigits << endl;
      saveFile << "vidSrcSuffix" << CO_DELIM << vidSrcSuffix << endl;
      saveFile << "vidSrcStart" << CO_DELIM << vidSrcStart << endl;
      saveFile << "vidSrcLoop" << CO_DELIM << vidSrcLoop << endl;

      // Misc Display
      saveFile << "showFPS" << CO_DELIM << showFPS << endl;
      saveFile << "showTrackingBoxes" << CO_DELIM << showTrackingBoxes << endl;
      saveFile << "numTrackingBoxes" << CO_DELIM << (UINT16)numTrackingBoxes << endl;
      saveFile << "videoStreamOutput" << CO_DELIM << (UINT16)videoStreamOutput << endl;

      //Tracking options
      saveFile << "trackingTrkr" << CO_DELIM << trackingTrkr << endl;
      saveFile << "trackingTrkrColor" << CO_DELIM << trackingTrkrColor << endl;
      saveFile << "track" << CO_DELIM << track << endl;
      saveFile << "fixate" << CO_DELIM << fixate << endl;
      saveFile << "scaleInv" << CO_DELIM << scaleInv << endl;
      saveFile << "opticalZoom" << CO_DELIM << opticalZoom << endl;
      saveFile << "digitalZoom" << CO_DELIM << digitalZoom << endl;
      saveFile << "spliceZoomOut" << CO_DELIM << spliceZoomOut << endl;
      saveFile << "fixateDigital" << CO_DELIM << fixateDigital << endl;
      saveFile << "fixationAlg" << CO_DELIM << fixationAlg << endl;
      saveFile << "kalmanProcessNoise" << CO_DELIM << kalmanProcessNoise << endl;
      saveFile << "kalmanMeasurementNoise" << CO_DELIM 
               << kalmanMeasurementNoise << endl;
      saveFile << "fixationGain" << CO_DELIM << fixationGain << endl;           
      saveFile << "tordoffPsi" << CO_DELIM << tordoffPsi << endl;           
      saveFile << "tordoffGamma1" << CO_DELIM << tordoffGamma1 << endl;
      saveFile << "tordoffGamma2" << CO_DELIM << tordoffGamma2 << endl;   
      saveFile << "zoomCoeff" << CO_DELIM << zoomCoeff << endl;    
      saveFile << "zoomDelayCoeff" << CO_DELIM << zoomDelayCoeff << endl;    
      saveFile << "targetAreaOpticalZoom" << CO_DELIM 
               << targetAreaOpticalZoom << endl;
      saveFile << "targetAreaDigitalZoom" << CO_DELIM 
               << targetAreaDigitalZoom << endl; 
      saveFile << "tordoffErrToHigherZoom" << CO_DELIM 
               << tordoffErrToHigherZoom << endl;  
            
      //Measure final size options
      saveFile << "mfs" << CO_DELIM << mfs << endl;
      saveFile << "mfsTrkr" << CO_DELIM << mfsTrkr << endl;
      saveFile << "mfsTrkrColor" << CO_DELIM << mfsTrkrColor << endl;
      saveFile << "mfsDirectory" << CO_DELIM << mfsDirectory << endl;
      saveFile << "mfsFile" << CO_DELIM << mfsFile << endl;

      //Camera communication
      saveFile << "serialPort" << CO_DELIM << serialPort << endl;
      
      //Pan options
      saveFile << "panPos1" << CO_DELIM << panPos1 << endl;
      saveFile << "panPos2" << CO_DELIM << panPos2 << endl;
      saveFile << "panIterations" << CO_DELIM << panIterations << endl;
      saveFile << "panPulseFrames" << CO_DELIM << panPulseFrames << endl;
      saveFile << "panInquiry" << CO_DELIM << panInquiry << endl;

      //Tilt options
      saveFile << "tiltPos1" << CO_DELIM << tiltPos1 << endl;
      saveFile << "tiltPos2" << CO_DELIM << tiltPos2 << endl;
      saveFile << "tiltIterations" << CO_DELIM << tiltIterations << endl;
      saveFile << "tiltPulseFrames" << CO_DELIM << tiltPulseFrames << endl;
      saveFile << "tiltInquiry" << CO_DELIM << tiltInquiry << endl;
      
      //Zoom options
      saveFile << "zoomPos1" << CO_DELIM << zoomPos1 << endl;
      saveFile << "zoomPos2" << CO_DELIM << zoomPos2 << endl;
      saveFile << "zoomIterations" << CO_DELIM << zoomIterations << endl;
      saveFile << "zoomPulseFrames" << CO_DELIM << zoomPulseFrames << endl;
      saveFile << "zoomInquiry" << CO_DELIM << zoomInquiry << endl;

      //Calibration options
      saveFile << "calDirectory" << CO_DELIM << calDirectory << endl;
      saveFile << "calFile" << CO_DELIM << calFile << endl;
      saveFile << "calTableLength" << CO_DELIM << calTableLength << endl;

      //Load/Save configuration options
      saveFile << "conDirectory" << CO_DELIM << conDirectory << endl;
      saveFile << "conFile" << CO_DELIM << conFile << endl;

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
 * SISingleCamOptions::copy
 *****************************************************************
 */
void SISingleCamOptions::copy(SISingleCamOptions const *that )
{
  // Video Output
  this->vidDst = that->vidDst;
  strcpy( this->vidDstDirectory, that->vidDstDirectory );
  strcpy( this->vidDstPrefix, that->vidDstPrefix );
  this->vidDstNumDigits = that->vidDstNumDigits;
  strcpy( this->vidDstSuffix, that->vidDstSuffix );
  this->vidDstPreventLine = that->vidDstPreventLine;

  //Video Input
  this->vidSrc = that->vidSrc;
  strcpy( this->vidSrcDirectory, that->vidSrcDirectory );
  strcpy( this->vidSrcFile, that->vidSrcFile );
  strcpy( this->vidSrcPrefix, that->vidSrcPrefix );
  this->vidSrcNumDigits = that->vidSrcNumDigits;
  strcpy( this->vidSrcSuffix, that->vidSrcSuffix );
  this->vidSrcStart = that->vidSrcStart;
  this->vidSrcLoop = that->vidSrcLoop;

  //I/O misc
  this->showFPS = that->showFPS;
  this->showTrackingBoxes = that->showTrackingBoxes;
  this->numTrackingBoxes = that->numTrackingBoxes;
  this->videoStreamOutput = that->videoStreamOutput;

  //Tracking
  this->trackingTrkr = that->trackingTrkr;
  this->trackingTrkrColor = that->trackingTrkrColor;
  this->track = that->track;
  this->fixate = that->fixate;
  this->scaleInv = that->scaleInv;
  this->opticalZoom = that->opticalZoom;
  this->digitalZoom = that->digitalZoom;
  this->spliceZoomOut = that->spliceZoomOut;
  this->fixateDigital = that->fixateDigital;
  this->fixationAlg = that->fixationAlg;
  this->kalmanProcessNoise = that->kalmanProcessNoise;
  this->kalmanMeasurementNoise = that->kalmanMeasurementNoise;
  this->fixationGain = that->fixationGain;
  this->tordoffPsi = that->tordoffPsi;  
  this->tordoffGamma1 = that->tordoffGamma1;  
  this->tordoffGamma2 = that->tordoffGamma2;  
  this->zoomCoeff = that->zoomCoeff;    
  this->zoomDelayCoeff = that->zoomDelayCoeff;    
  this->targetAreaOpticalZoom = that->targetAreaOpticalZoom;
  this->targetAreaDigitalZoom = that->targetAreaDigitalZoom;
  this->tordoffErrToHigherZoom = that->tordoffErrToHigherZoom;    

  //Measure final size options
  this->mfs = that->mfs;
  this->mfsTrkr = that->mfsTrkr;
  this->mfsTrkrColor = that->mfsTrkrColor;
  strcpy( this->mfsDirectory, that->mfsDirectory );
  strcpy( this->mfsFile, that->mfsFile );

  //Camera communication
  strcpy( this->serialPort, that->serialPort );
  
  //Pan options
  this->panPos1 = that->panPos1;
  this->panPos2 = that->panPos2;
  this->panIterations = that->panIterations;
  this->panPulseFrames = that->panPulseFrames;
  this->panInquiry = that->panInquiry;

  //Tilt options
  this->tiltPos1 = that->tiltPos1;
  this->tiltPos2 = that->tiltPos2;
  this->tiltIterations = that->tiltIterations;
  this->tiltPulseFrames = that->tiltPulseFrames;
  this->tiltInquiry = that->tiltInquiry;

  //Zoom options
  this->zoomPos1 = that->zoomPos1;
  this->zoomPos2 = that->zoomPos2;
  this->zoomIterations = that->zoomIterations;
  this->zoomPulseFrames = that->zoomPulseFrames;
  this->zoomInquiry = that->zoomInquiry;

  //Calibration options
  strcpy( this->calDirectory, that->calDirectory );
  strcpy( this->calFile, that->calFile );
  this->calTableLength = that->calTableLength;

  //Load/Save configuration options
  strcpy( this->conDirectory, that->conDirectory );
  strcpy( this->conFile, that->conFile );

}


// File: $Id: SISingleCamOptions.cpp,v 1.28 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: SISingleCamOptions.cpp,v $
// Revision 1.28  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.27  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.26  2005/09/06 01:57:12  edn2065
// Added zoom delay handling
//
// Revision 1.25  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.24  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.23  2005/08/09 21:59:27  edn2065
// Added easy serialPort switching via SISingleCamOptions files
//
// Revision 1.22  2005/08/03 02:34:54  edn2065
// Added tordoff psi and gammas to menu
//
// Revision 1.21  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.20  2005/06/28 15:09:40  edn2065
// Added multiple tracker color option to kernel
//
// Revision 1.19  2005/06/09 20:51:22  edn2065
// Added pulsing to zoom and pt
//
// Revision 1.18  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.17  2005/06/08 15:08:00  edn2065
// Add P/T/Z inquiry
//
// Revision 1.16  2005/06/08 13:44:18  edn2065
// Overnight checkin
//
// Revision 1.14  2005/06/03 12:49:46  edn2065
// Changed Controller to SIKernel
//
// Revision 1.13  2005/05/27 13:20:53  edn2065
// Added ability to change number of ROIs in output
//
// Revision 1.12  2005/05/26 20:14:59  edn2065
// Fixed saveOptions bug
//
// Revision 1.11  2005/05/26 19:15:02  edn2065
// Added VideoStream. tested. Still need to get tracker working
//
// Revision 1.10  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
//
// Revision 1.9  2005/05/24 16:02:40  edn2065
// Changed directory structure
//
// Revision 1.8  2005/04/22 20:52:48  edn2065
// Implemented load and save of controller options
//
// Revision 1.4  2005/04/21 13:47:39  edn2065
// Added Menus and fully commented. commented ControllerOptions.
//
// Revision 1.3  2005/04/07 14:28:36  edn2065
// Implemented tester. untested tester.
//
// Revision 1.2  2005/04/06 21:21:09  edn2065
// Have number of bugs with threads. Revamping GUI to VideoSourceFile call
//
// ----------------------------------------------------------------------
//
