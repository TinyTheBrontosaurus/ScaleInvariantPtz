#include <stdio.h>
#include <math.h>
#include "PanTilt.h"
#include "ZoomOptical.h"
#include "CameraComms.h"
#include "CameraRxThread.h"
#include "CameraTxThread.h"

#include <unistd.h>
extern "C" {
#include "CommandLevel.h"
}

#define CYCLE_TIME 30000
#define RUNS 5
#define ZOOM_DELAY 165
//#define PRINT_EACH_ITERATION



#include "Time.h"

void panTiltTest(CameraComms *);
void panTiltTestReadings(UINT32, PanTilt *);
void zoomTest(CameraComms *);
void zoomTestReadings(UINT32, ZoomOptical *);
void ptZoomTest(CameraComms *cam);
void ptzTestReadings( UINT8 cam, UINT32 numReadings );
void rapidPtzTest( CameraComms *cam );

CHAR menuPTZ[] = 
"Test for PanTiltZoom"CRLF
"--------------------"CRLF
"1:   Test Pan/Tilt"  CRLF
"2:   Test Zoom"      CRLF
"3:   Test P/T and Z" CRLF
"4:   Test rapid P/T and Z" CRLF
"q:   Quit"           CRLF;


int main() 
{
  BOOL execPTZTest = true;
  CHAR menuPTZInput = '\n';
  CameraComms cam;
  printf( "Welcome to Test for PanTiltZoom"CRLF);
  
  //Initialize camera comms
  printf( "Initializing Sony EVI-D100 Camera..." );
  //Let user see that message
  fflush(stdout);
  
  if( cam.init( "/dev/ttyS0" ) == 0)
  {
    printf( "success." CRLF CRLF);
    //Run guts of program
    while(execPTZTest)
    {
      if( menuPTZInput == '\n' )
      {
        //Print the menu
        printf( menuPTZ );
      }
      
      //Get user response
      scanf("%c", &menuPTZInput );
      
      //Handle response
      switch( menuPTZInput )
      {
        //Pan/tilt test
        case '1':
          panTiltTest(&cam);   
          break;
        
        //Zoom test
        case '2':
          zoomTest(&cam);
          break;
          
        //PTZ test
        case '3':
          ptZoomTest(&cam);
          break;
          
        case '4':
          rapidPtzTest( &cam );
          break;
        
        //Quit
        case 'Q':
        case 'q':
          execPTZTest = false;
          break;
          
        default:
          break;
      
      }  
    }
  }
  else
  {
    printf( "failed." CRLF );
  }
     
  printf( "Goodbye!" CRLF );
  
  return 0;
}

void panTiltTest(CameraComms *cam)
{
  PanTilt pt;
  
  //Set the camera number
  pt.init( cam->getCameraNumber() );
  
  //Watch the readings
  panTiltTestReadings( 100, &pt );

  //Look to lower left
  printf( "Lower left..."CRLF );
  pt.move( -50,  24, 
          -10, 24);
  panTiltTestReadings( 100, &pt );

  //Look to lower right
  printf( "Lower right..."CRLF );
  pt.move( 50,  24, 
          -10, 24);
  panTiltTestReadings( 100, &pt );
  
  //Look to upper right
  printf( "Upper right..."CRLF );
  pt.move( 50,  24, 
          10, 24);
  panTiltTestReadings( 100, &pt );
  
  //Look to upper left
  printf( "Upper left..."CRLF );
  pt.move( -50,  24, 
          10, 24);
  panTiltTestReadings( 100, &pt );   
  
  //Look back at home
  printf( "Reset..." CRLF );
  pt.reset();
  panTiltTestReadings( 100, &pt );         
  
}

void panTiltTestReadings(UINT32 numReadings, PanTilt *pt )
{
  //Print out a certain number of readings
  FLOAT pan = 0.0, tilt = 0.0;
  for( UINT32 curReading = 0; curReading < numReadings; curReading++ )
  {
    pt->getPanTilt( &pan, &tilt);
    printf( "Pan: %2.3f   Tilt: %2.3f" CRLF, pan, tilt );
  }
}

void zoomTest(CameraComms *cam)
{
  //Set up camera
  ZoomOptical zoom;
  zoom.init(cam->getCameraNumber());
  zoomTestReadings(100, &zoom);
  
  //Zoom in fully
  printf( "Full zoom in..."CRLF);
  zoom.zoomRaw( 15800 );
  zoomTestReadings(100, &zoom);
  
  //Zoom out fully
  //printf( "Full zoom out..."CRLF);
  //zoom.zoomRaw( 0 );
  //zoomTestReadings(100, &zoom);
  
  //Zoom in partway
  printf( "Half zoom in..."CRLF);
  zoom.zoomRaw( 7000 );
  zoomTestReadings(100, &zoom);
  
  //Zoom in fully
  printf( "3/4 zoom in..."CRLF);
  zoom.zoomRaw( 11000 );
  zoomTestReadings(100, &zoom);
  
  //Reset
  printf( "Reset to home..."CRLF);
  zoom.reset();
  zoomTestReadings(100, &zoom);
}

void zoomTestReadings(UINT32 numReadings, ZoomOptical *zoom )
{
  //Print out a certain number of readings
  UINT16 zoomRawMag;
  FLOAT  zoomMag;
  for( UINT32 curReading = 0; curReading < numReadings; curReading++ )
  {
    zoom->inquiryBoth( &zoomRawMag, &zoomMag);
    printf( "Raw: %d   Mag: %f" CRLF, zoomRawMag, zoomMag );
    usleep(4000);
  }
}


void ptZoomTest(CameraComms *cam)
{
  PanTilt pt;
  ZoomOptical zoom;
  
  //Set the camera number
  zoom.init(cam->getCameraNumber());  
  pt.init( cam->getCameraNumber() );
  
  //Watch the readings
  ptzTestReadings( cam->getCameraNumber(), 100);

  //Look to lower left
  printf( "Lower left..."CRLF );
  pt.move( -50,  24, 
          -10, 24);
  zoom.zoomRaw( 15800 );
  ptzTestReadings( cam->getCameraNumber(), 100);

  //Look to lower right
  printf( "Lower right..."CRLF );
  pt.move( 50,  24, 
          -10, 24);
  zoom.zoomRaw( 7800 );          
  ptzTestReadings( cam->getCameraNumber(), 100);
  
  //Look to upper right
  printf( "Upper right..."CRLF );
  pt.move( 50,  24, 
          10, 24);
  zoom.zoomRaw( 1800 );          
  ptzTestReadings( cam->getCameraNumber(), 100);
  
  //Look to upper left
  printf( "Upper left..."CRLF );
  pt.move( -50,  24, 
          10, 24);
  zoom.zoomRaw( 15800 );          
  ptzTestReadings( cam->getCameraNumber(), 100);
  
  //Look back at home
  printf( "Reset..." CRLF );
  pt.reset();
  zoom.reset();
  ptzTestReadings( cam->getCameraNumber(), 100);
  
}

void ptzTestReadings( UINT8 cam, UINT32 numReadings )
{
  //Print out a certain number of readings
  int zoomRawMag;
  FLOAT pan;
  FLOAT tilt;
  
  UINT32 startTime;
  UINT32 endTime;
    
  
  
  for( UINT32 curReading = 0; curReading < numReadings; curReading++ )
  {
    startTime = Time::getTimestamp();
    //commandLevel.CAM_PTZInq( cam, &pan, &tilt, &zoomRawMag);
    printf( "Pan: %1.2f  Tilt %1.2f  Zoom: %d " CRLF, 
      pan, tilt, zoomRawMag);
    endTime = Time::getTimestamp();
    printf("PTZ inquiry time elapsed: %u (%s @ %d)" CRLF, 
      endTime - startTime, __FILE__, __LINE__);
      
    usleep(4000);
  }
}

void rapidPtzCommandReading( CameraRxThread *rxThread, CameraTxThread *txThread,
  Barrier *iterationBarrier,
  UINT32 numCmdRdg, FLOAT panMult, FLOAT tiltMult )
{

  FLOAT panAngle;
  FLOAT tiltAngle;
  UINT16 zoomMag;
  UINT32 ptTime;
  UINT32 zTime;
  UINT32   startTime;
  UINT32   endTime;

  for( SINT16 ctr = 0; ctr < numCmdRdg; ctr++ )
  {
    txThread->setPTAngle( ctr*panMult, ctr*tiltMult );
    if( panMult == 0.0 && tiltMult == 0.0 )
    {
      txThread->setZMag( 0 );
    }
    else
    {
      txThread->setZMag( ctr*100 );
    }
    iterationBarrier->wait();
    
    endTime = Time::getTimestamp();
    
    
    rxThread->getPTAngle( &panAngle, &tiltAngle, &ptTime);
    rxThread->getZMag( &zoomMag, &zTime);  
    printf( "Pan: %1.2f Tilt: %1.2f Zoom: %7d (PT: %3u Z: %3u DeltaT: %3u)"
      CRLF,
      panAngle, tiltAngle, zoomMag, endTime - ptTime, endTime - zTime,
        endTime - startTime );
    startTime = endTime;
    usleep(CYCLE_TIME); 
  }  

}


void stats( FLOAT *array, UINT32 length, FLOAT *mean, FLOAT *stdDev )
{

  FLOAT sum = 0;
  FLOAT squaredsSum = 0;
  
  for( UINT32 curVal = 0; curVal < length; curVal++ )
  {
    sum += array[curVal];
    squaredsSum += array[curVal]*array[curVal];
  }
  
  *mean = sum / length;
  //Absolute value only needed when stdDev is very close to 0 and
  //truncation makes the subtraction negative.
  *stdDev = sqrt(abs(squaredsSum / length - *mean * *mean));

}


void freqAnalysis(UINT32 maxResponseIteration, 
                  UINT32 maxCommandIteration,
                  UINT32 minResponseIteration, 
                  UINT32 minCommandIteration, 
                  FLOAT maxResponse,
                  FLOAT minResponse,
                  UINT32 N, FLOAT amp, FLOAT *gains,
                  FLOAT *delays, FLOAT freqHz, 
                  UINT32 *resultsCounter, CHAR *desc )                  
{
  FLOAT zMaxDelay = (UINT32)((FLOAT) maxResponseIteration-
                    (FLOAT)maxCommandIteration + N )%N*33.0;
  FLOAT zMinDelay = (UINT32)((FLOAT) minResponseIteration-
                    (FLOAT)minCommandIteration + N )%N*33.0;
  FLOAT zAvgDelay = (zMaxDelay + zMinDelay)/2;
  FLOAT zAvgDelayDeg = zAvgDelay * freqHz * .001 * 360;
  FLOAT zAvgDelayRad = zAvgDelay * freqHz * .001 * 2*M_PI;
  
  gains[*resultsCounter] = 
    ( maxResponse - minResponse ) / (amp * 2.0);
  delays[*resultsCounter] = zAvgDelayDeg;
  
  (*resultsCounter)++;
      
  #ifdef PRINT_EACH_ITERATION    
  printf( "%s Command Amp ( %4.2f Hz): %5.2f" CRLF, desc, freqHz,
    amp );
  
  printf( "   %s Response Mag: max %5.2f min %5.2f "
          "p-p %5.2f gain %4.2f" CRLF, desc,
    maxResponse, minResponse, 
    maxResponse - minResponse, 
    ( maxResponse - minResponse ) / (amp * 2.0)
    );
  printf( "   %s Response Delay(ms): max %4.2f min %4.2f avg"  
          " %4.2f deg %4.2f rad %4.2f" CRLF, desc,
    zMaxDelay, zMinDelay, zAvgDelay, zAvgDelayDeg, zAvgDelayRad);
  #endif
}

//Tests and outputs the frequency response for pan/tilt/zoom and also
//does analysis (gain & phase)
BOOL rapidPtzFreqAnal( CameraRxThread *rxThread, CameraTxThread *txThread,
  Barrier *iterationBarrier, FLOAT freqHz,
  FLOAT panAmp, FLOAT tiltAmp, UINT16 zoomAmp )
{

  UINT32 N = (UINT32)(1 / ( freqHz * .033 ));
  UINT32 n = 0;
  UINT32 ptTime;
  UINT32 zTime;
  UINT32 startTime;
  UINT32 endTime;
  UINT32 iteration = 0;
  FLOAT  panAngle;
  FLOAT  tiltAngle;
  UINT16 zoomMag;
  UINT16 oldZoomMag;
  FLOAT  oldPanAngle;
  FLOAT  oldTiltAngle;
  FLOAT  multCommand;
  FLOAT lastMultCommand = 0;
  FLOAT maxCommand;
  FLOAT minCommand;
  UINT32 maxCommandIteration = 0;
  UINT32 minCommandIteration = 0;
  BOOL increasingCommand = true;
  BOOL firstRun = true;
  BOOL success;
  
  FLOAT  maxPanResponse;
  FLOAT  maxTiltResponse;
  UINT16 maxZoomResponse;
  FLOAT  minPanResponse;
  FLOAT  minTiltResponse;
  UINT16 minZoomResponse;
    
  UINT32 maxPanIteration;
  UINT32 minPanIteration;  
  UINT32 maxTiltIteration;
  UINT32 minTiltIteration;  
  UINT32 maxZoomIteration;
  UINT32 minZoomIteration;  
  
  UINT8 consecutiveZoomIncreases = 0;
  UINT8 consecutivePanIncreases = 0;
  UINT8 consecutiveTiltIncreases = 0;
  
  maxPanResponse = -1000.0;
  minPanResponse = 1000.0;       
  maxTiltResponse = -1000.0;
  minTiltResponse = 1000.0;
  maxZoomResponse = 0;
  minZoomResponse = 15000;
  
  FLOAT zoomGains[RUNS-1];
  FLOAT zoomDelays[RUNS-1];
  UINT32 zoomResultsCounter = 0;
  FLOAT zoomGainMean;
  FLOAT zoomGainStdDev;
  FLOAT zoomDelayMean;
  FLOAT zoomDelayStdDev;
  
  FLOAT panGains[RUNS-1];
  FLOAT panDelays[RUNS-1];
  UINT32 panResultsCounter = 0;
  FLOAT panGainMean;
  FLOAT panGainStdDev;
  FLOAT panDelayMean;
  FLOAT panDelayStdDev;
  
  FLOAT tiltGains[RUNS-1];
  FLOAT tiltDelays[RUNS-1];
  UINT32 tiltResultsCounter = 0;
  FLOAT tiltGainMean;
  FLOAT tiltGainStdDev;
  FLOAT tiltDelayMean;
  FLOAT tiltDelayStdDev;
  
  
  for( n = 0; n < RUNS*N; n++ )
  {
    multCommand = sin(2*M_PI*n/N);
    if( lastMultCommand <= multCommand )
    {
      //Sine wave is increasing
      if( !increasingCommand )
      {
        //Just changed directions
        minCommand = lastMultCommand;
        increasingCommand = true;
        minCommandIteration = iteration - 1;
        
        firstRun = false;    
      }
        
    }
    else
    {
      //Sine wave is decreasing
      if( increasingCommand )
      {
        //Just changed directions
        maxCommand = lastMultCommand;
        increasingCommand = false;
        maxCommandIteration = iteration - 1;
      
      }        
    }
    
    if( oldZoomMag <= zoomMag )
    {
      consecutiveZoomIncreases++;
    }
    else
    {
      consecutiveZoomIncreases = 0;
    }

    if( oldPanAngle <= panAngle )
    {
      consecutivePanIncreases++;
    }
    else
    {
      consecutivePanIncreases = 0;
    }

    if( oldTiltAngle <= tiltAngle )
    {
      consecutiveTiltIncreases++;
    }
    else
    {
      consecutiveTiltIncreases = 0;
    }        
        
    //Do zoom
    #ifdef DO_ZOOM
    if( oldZoomMag < (16384 >> 1) && zoomMag >= (16384 >> 1) &&
      consecutiveZoomIncreases >= 3)
    {
      if( !firstRun )
      {
        freqAnalysis( maxZoomIteration, 
                  maxCommandIteration,
                  minZoomIteration, 
                  minCommandIteration, 
                  (FLOAT)maxZoomResponse,
                  (FLOAT)minZoomResponse,
                  N, zoomAmp, zoomGains,
                  zoomDelays, freqHz, 
                  &zoomResultsCounter, "Zoom" );
        
      }
      maxZoomResponse = 0;
      minZoomResponse = 15000;         
    }
    #endif
    
    //Do Pan
    #ifdef DO_PAN
    if( oldPanAngle < (0.0) && panAngle >= (0.0) &&
      consecutivePanIncreases >= 3)
    {
      if( !firstRun )
      {
        freqAnalysis( maxPanIteration, 
                  maxCommandIteration,
                  minPanIteration, 
                  minCommandIteration, 
                  (FLOAT)maxPanResponse,
                  (FLOAT)minPanResponse,
                  N, panAmp, panGains,
                  panDelays, freqHz, 
                  &panResultsCounter, "Pan" );
        
      }
      maxPanResponse = -1000.0;
      minPanResponse = 1000.0;
    }
    #endif   
      
    //Do tilt
    #ifdef DO_TILT
    if( oldTiltAngle < (0.0) && tiltAngle >= (0.0) &&
      consecutiveTiltIncreases >= 3)
    {
      if( !firstRun )
      {
        freqAnalysis( maxTiltIteration, 
                  maxCommandIteration,
                  minTiltIteration, 
                  minCommandIteration, 
                  (FLOAT)maxTiltResponse,
                  (FLOAT)minTiltResponse,
                  N, tiltAmp, tiltGains,
                  tiltDelays, freqHz, 
                  &tiltResultsCounter, "Tilt" );
        
      }
      maxTiltResponse = -1000.0;
      minTiltResponse = 1000.0;
    }
    #endif    
    
    txThread->setPTAngle( panAmp*multCommand, tiltAmp*multCommand);
    txThread->setZMag( (UINT16)(zoomAmp*multCommand + (16384 >> 1)) );

    iterationBarrier->wait();
    
    endTime = Time::getTimestamp();    
    
    oldZoomMag = zoomMag;
    oldPanAngle = panAngle;
    oldTiltAngle = tiltAngle;
    rxThread->getPTAngle( &panAngle, &tiltAngle, &ptTime);
    rxThread->getZMag( &zoomMag, &zTime);  
    
    if( panAngle > maxPanResponse )
    {
      maxPanResponse = panAngle;
      maxPanIteration = iteration;
    }
    
    if( panAngle < minPanResponse )
    {
      minPanResponse = panAngle;
      minPanIteration = iteration;
    }
    
    if( tiltAngle > maxTiltResponse )
    {
      maxTiltResponse = tiltAngle;
      maxTiltIteration = iteration;
    }
    
    if( tiltAngle < minTiltResponse )
    {
      minTiltResponse = tiltAngle;
      minTiltIteration = iteration;
    }
    
    if( zoomMag > maxZoomResponse )
    {
      maxZoomResponse = zoomMag;
      maxZoomIteration = iteration;
    }
    
    if( zoomMag < minZoomResponse )
    {
      minZoomResponse = zoomMag;
      minZoomIteration = iteration;
    }
    
    
    
    startTime = endTime;
    iteration++;
    lastMultCommand = multCommand;
    usleep(CYCLE_TIME);     
    
  }
  
  #ifdef DO_ZOOM
    stats( zoomGains, RUNS-1, &zoomGainMean, &zoomGainStdDev );
    stats( zoomDelays, RUNS-1, &zoomDelayMean, &zoomDelayStdDev );   
    success = zoomGainMean / 2 > zoomGainStdDev;
              
    if( success )  
    {
      printf( "Freq: %4.2f Gain: avg %4.3f s.d. %4.3f  "
            "Delay (degrees): avg %4.3f s.d. %4.3f " CRLF,
            freqHz, zoomGainMean, zoomGainStdDev, 
            zoomDelayMean, zoomDelayStdDev );
    }
  #endif
  
  #ifdef DO_PAN
    stats( panGains, RUNS-1, &panGainMean, &panGainStdDev );
    stats( panDelays, RUNS-1, &panDelayMean, &panDelayStdDev );   
    success = panGainMean / 2 > panGainStdDev;
              
    if( success )  
    {
      printf( "Freq: %4.2f Gain: avg %4.3f s.d. %4.3f  "
            "Delay (degrees): avg %4.3f s.d. %4.3f " CRLF,
            freqHz, panGainMean, panGainStdDev, 
            panDelayMean, panDelayStdDev );
    }
  #endif

  #ifdef DO_TILT
    stats( tiltGains, RUNS-1, &tiltGainMean, &tiltGainStdDev );
    stats( tiltDelays, RUNS-1, &tiltDelayMean, &tiltDelayStdDev );   
    success = tiltGainMean / 2 > tiltGainStdDev;
              
    if( success )  
    {
      printf( "Freq: %4.2f Gain: avg %4.3f s.d. %4.3f  "
            "Delay (degrees): avg %4.3f s.d. %4.3f " CRLF,
            freqHz, tiltGainMean, tiltGainStdDev, 
            tiltDelayMean, tiltDelayStdDev );
    }
  #endif
  
  
    
  return success;
  
  
}

//Tests and outputs the frequency response for pan/tilt/zoom 
void rapidPtzSin( CameraRxThread *rxThread, CameraTxThread *txThread,
  Barrier *iterationBarrier, FLOAT freqHz,
  FLOAT panAmp, FLOAT tiltAmp, UINT16 zoomAmp )
{

  UINT32 N = (UINT32)(1 / ( freqHz * .033 ));
  UINT32 n = 0;
  UINT32 ptTime;
  UINT32 zTime;
  UINT32 startTime;
  UINT32 endTime;
  FLOAT  panAngle;
  FLOAT  tiltAngle;
  UINT16 zoomMag;
  FLOAT  sinMult;
  
  
  for( n = 0; n < N; n++ )
  {
    sinMult = sin(2*M_PI*n/N);
    txThread->setPTAngle( panAmp*sinMult, tiltAmp*sinMult );
    txThread->setZMag( (UINT16)(zoomAmp*sinMult + (16384 >> 1)) );

    iterationBarrier->wait();
    
    endTime = Time::getTimestamp();    
    
    rxThread->getPTAngle( &panAngle, &tiltAngle, &ptTime);
    rxThread->getZMag( &zoomMag, &zTime);  
    printf( "Pan: %3.2f %3.2f Tilt: %2.2f %2.2f "
            "Zoom: %7d %7d (PT: %3u Z: %3u DeltaT: %3u)"
      CRLF,
      panAngle, panAmp*sinMult, tiltAngle, tiltAmp*sinMult, 
      zoomMag, (UINT16)(zoomAmp*sinMult + (16384 >> 1)), 
      endTime - ptTime, endTime - zTime,
        endTime - startTime );
    startTime = endTime;
    usleep(CYCLE_TIME);     
  }
  
}

//Tests and outputs the step response for pan/tilt/zoom 
void rapidPtzStep( CameraRxThread *rxThread, CameraTxThread *txThread,
  Barrier *iterationBarrier, FLOAT pan, FLOAT tilt, 
  UINT16 zoom, UINT32 waitTime )
{
  UINT32 n;
  UINT32 ptTime;
  UINT32 zTime;
  UINT32 startTime;
  UINT32 endTime;
  FLOAT  panAngle;
  FLOAT  tiltAngle;
  UINT16 zoomMag;
  

  for( n = 0; n <= waitTime; n++ )
  {
    txThread->setPTAngle( pan, tilt );
    txThread->setZMag( zoom );

    iterationBarrier->wait();
    
    endTime = Time::getTimestamp();    
    
    rxThread->getPTAngle( &panAngle, &tiltAngle, &ptTime);
    rxThread->getZMag( &zoomMag, &zTime);  
    printf( "Pan: %3.2f %3.2f Tilt: %2.2f %2.2f "
            "Zoom: %7d %7d (PT: %3u Z: %3u DeltaT: %3u)"
      CRLF,
      panAngle, pan, tiltAngle, tilt, 
      zoom, zoomMag, endTime - ptTime, endTime - zTime,
        endTime - startTime );
    startTime = endTime;
    usleep(CYCLE_TIME);     
  }
  
}



//Main function for rapid PTZ tests
void rapidPtzTest( CameraComms *cam )
{
  CameraRxThread rxThread;
  CameraTxThread txThread;
  Barrier  iterationBarrier;  
  Barrier  endIterationBarrier;
  Semaphore socketsTakenSem;
  UINT8 socketsTaken = 0;
  Semaphore clearToSendSem;
  UINT32 repeats;
  Semaphore fillerSem;
  BOOL fooBool;
  
  
  clearToSendSem.init(2);
  iterationBarrier.init(2);
  endIterationBarrier.init(1);
  rxThread.init(cam->getCameraNumber(), cam->getCommandLevel()->getFd(), &socketsTaken,
    &socketsTakenSem, &clearToSendSem, &fillerSem, &fillerSem, &fooBool);
  txThread.init(cam->getCameraNumber(), cam->getCommandLevel()->getFd(), &iterationBarrier, 
    &endIterationBarrier,
    &socketsTaken, &socketsTakenSem, &clearToSendSem, &fillerSem, &fillerSem,
    &fooBool);
  
  rxThread.start(0);
  txThread.start(0);
  
  //Reset the P/T/Z
  txThread.setPTAngle( 0.0, 0.0 );
  txThread.setZMag(16384 >> 1 );
  
  for( UINT16 ctr = 0; ctr < 60; ctr++ )
  {
    iterationBarrier.wait();
    usleep(CYCLE_TIME);
  }
  
  for( FLOAT freq = .1; freq < 7.5; freq += 0.05 )
  {
    UINT32 ctr = 0;
    while( !rapidPtzFreqAnal( &rxThread, &txThread,
      &iterationBarrier, freq, 20, 20, 800 ) && ctr++ < 5);
      
    #ifdef PRINT_EACH_ITERATION
    printf( CRLF CRLF );
    #endif
  
  }
  
  
  /*for( repeats = 0; repeats < 10; repeats++ )
  {  
  
    /*rapidPtzSin( &rxThread, &txThread, &iterationBarrier, 
      0.0625, 100, 20, 7000 );  
    rapidPtzSin( &rxThread, &txThread, &iterationBarrier, 
      0.125, 100, 20, 7000 );  
    rapidPtzSin( &rxThread, &txThread, &iterationBarrier, 
      0.25, 100, 20, 7000 );
    rapidPtzSin( &rxThread, &txThread, &iterationBarrier, 
      0.5, 100, 20, 7000 );
    rapidPtzSin( &rxThread, &txThread, &iterationBarrier, 
      1, 100, 20, 7000 );
    rapidPtzSin( &rxThread, &txThread, &iterationBarrier, 
      7.5, 100, 20, 7000 );*/    
  
  
    /*rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      -100, -20, 0, 10);
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      100, 20, 700, 10);
  
  }
  /*
  for( repeats = 0; repeats < 10; repeats++ )
  {  
  
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      -100, -20, 0, 10);
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      100, 20, 600, 10); 
  }  
  
  for( repeats = 0; repeats < 10; repeats++ )
  {  
  
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      -100, -20, 0, 10);
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      100, 20, 500, 10); 
  }  
  
  for( repeats = 0; repeats < 10; repeats++ )
  {  
  
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      -100, -20, 0, 10);
    rapidPtzStep(&rxThread, &txThread, &iterationBarrier, 
      100, 20, 400, 10); 
  }  
     */
  /*rapidPtzCommandReading( &rxThread, &txThread, &iterationBarrier, 
    100, 1.0, -0.2 );
  rapidPtzCommandReading( &rxThread, &txThread, &iterationBarrier, 
    100, -1.0, -0.2 );
  rapidPtzCommandReading( &rxThread, &txThread, &iterationBarrier, 
    100, -1.0, 0.2 );
  rapidPtzCommandReading( &rxThread, &txThread, &iterationBarrier, 
    100, 1.0, 0.2 );
  rapidPtzCommandReading( &rxThread, &txThread, &iterationBarrier, 
    100, 0.0, 0.0 );*/
    
  usleep( CYCLE_TIME); //b/c of race condition
  rxThread.stop();
  txThread.stop();
  iterationBarrier.wait();
  socketsTakenSem.release();
  clearToSendSem.release();
  
}
