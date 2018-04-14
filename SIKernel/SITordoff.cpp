#include <stdio.h>
#include <math.h>
#include "SITordoff.h"


/*****************************************************************
 * SITordoff ctor
 *****************************************************************
 */
SITordoff::SITordoff()
{
  init();
}

/*****************************************************************
 * SITordoff dtor
 *****************************************************************
 */
SITordoff::~SITordoff() 
{
}

/*****************************************************************
 * SITordoff::init
 *****************************************************************
 */
void SITordoff::init( )
{
  init( 0.05, 0.01, 0.5, 0.25, 0.025 ); 
}      

/*****************************************************************
 * SITordoff::init
 *****************************************************************
 */
void SITordoff::init( FLOAT processNoise, FLOAT measurementNoise, 
  FLOAT myPsi, FLOAT myGamma1, FLOAT myGamma2)
{
  Matrix stateCov(2,2);
  FLOAT covValue = 0.5;
  stateCov(0,0) = covValue;
  stateCov(0,1) = covValue;
  stateCov(1,0) = covValue;
  stateCov(1,1) = covValue;
  init( processNoise, measurementNoise, myPsi, myGamma1, myGamma2, stateCov );
}

/*****************************************************************
 * SITordoff::init
 *****************************************************************
 */
void SITordoff::init( FLOAT processNoise, FLOAT measurementNoise, 
                      FLOAT myPsi, FLOAT myGamma1, FLOAT myGamma2,
                      Matrix &stateCovariance)
{
  //Initialize pan and tilt
  kfPan.init( processNoise, measurementNoise, stateCovariance );  
  kfTilt = kfPan;
  
  //Initialize nu_k
  nu_k.init(2,1);
  
  //Initialize covariances
  lastGamma1Cov.init(2,2);
  lastGamma2Cov.init(2,2);
  
  //Save psi and gammas
  psi = myPsi;
  gamma1 = myGamma1;
  gamma2 = myGamma2;
  
  //Reset the number of iterations
  iteration = 0;
}      

/*****************************************************************
 * SITordoff::setMeasurement
 *****************************************************************
 */
void SITordoff::setMeasurement(FLOAT x, FLOAT controlX, FLOAT y, 
                               FLOAT controlY, FLOAT zoom)
{

  FLOAT noiseScalar = zoom * zoom;

  #ifdef DEBUG_TORDOFF_MEAS
  printf( "Measurements: x: %1.4lf dx: %1.4lf y: %1.4lf dy: %1.4lf "
          "zoom: %1.4lf (%s @ %d)"CRLF,
          x, controlX, y, controlY, zoom, __FILE__, __LINE__ );
  #endif

  //Scale the process noise and measurement noise
  kfPan.scaleProcessNoise( 1/noiseScalar );
  kfTilt.scaleProcessNoise( 1/noiseScalar );
  kfPan.scaleMeasurementNoise( noiseScalar );
  kfTilt.scaleMeasurementNoise( noiseScalar );
  
  //Set the measurements
  kfPan.setMeasurement( x, controlX, zoom );
  kfTilt.setMeasurement( y, controlY, zoom );
    
  curZoom = zoom;
     
  iteration++;

}      

/*****************************************************************
 * SITordoff::getCommands
 *****************************************************************
 */
void SITordoff::getCommands(FLOAT &pan, FLOAT &tilt, FLOAT &zoom, 
  BOOL errOnZoomIn )
{

  pan = kfPan.getCommand();
  tilt = kfTilt.getCommand();
  if( iteration >= 3 )
  {
    zoom = chooseZoom(errOnZoomIn);
  }
  else
  {
    zoom = 1.0;
  }
  
  #ifdef DEBUG_TORDOFF_CMD
  printf( "Commands: pan: %1.4lf tilt: %1.4lf zoom: %1.4lf (%s @ %d)"CRLF,
          pan, tilt, zoom, __FILE__, __LINE__ );
  #endif        
}      


/*****************************************************************
 * SITordoff::chooseZoom
 *****************************************************************
 */
FLOAT SITordoff::chooseZoom(BOOL errOnZoomIn)
{
  //The desired focal length
  FLOAT f_kp1;
  
  //The 2-norm of the covariances
  FLOAT gamma1Var;
  FLOAT gamma2Var;
  FLOAT preferredVar;
   
  nu_k(0,0) = kfPan.getInnovation();
  nu_k(1,0) = kfTilt.getInnovation();
  
  //Find covariance
  get2NormCovarianceSquared( lastGamma1Cov, gamma1, 
                             lastGamma1Cov, gamma1Var );
  get2NormCovarianceSquared( lastGamma2Cov, gamma2, 
                             lastGamma2Cov, gamma2Var );
                              
  //Take the larger of the two
  if( errOnZoomIn )
  {
    preferredVar = SI_MIN(gamma1Var, gamma2Var );
  }
  else
  {
    preferredVar = SI_MAX(gamma1Var, gamma2Var );
  }  
  
  f_kp1 = sqrt(psi*psi/(24*sqrt(preferredVar)));
  
  return f_kp1;

}

/*****************************************************************
 * SITordoff::get2NormCovarianceSquared
 *****************************************************************
 */
void SITordoff::get2NormCovarianceSquared(Matrix lastCov, FLOAT gamma, 
                                    Matrix &newCov, FLOAT &normSquared )
{
  Matrix eigVals(2,1);
   
  //Calculate the covariance
  newCov = (nu_k * ~nu_k ) * gamma / (curZoom * curZoom ) +
          lastCov * (1-gamma);
          
  //Find the 2-norm squared, which is the largest eigenvalue        
  eigVals = newCov.eigVal();
  normSquared = eigVals(1,0) > eigVals(0,0) ? eigVals(1,0) : eigVals(0,0);
  
  
}

// File: $Id: SITordoff.cpp,v 1.10 2005/09/07 18:44:04 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SITordoff.cpp,v $
// Revision 1.10  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.9  2005/08/05 22:09:40  edn2065
// Moved kalman functionality into SIKalman
//
// Revision 1.8  2005/08/03 02:34:54  edn2065
// Added tordoff psi and gammas to menu
//
// Revision 1.7  2005/08/03 01:43:26  edn2065
// Implemented focal length selection
//
// Revision 1.6  2005/08/01 23:30:07  edn2065
// Added zoom invariance to KF
//
// Revision 1.5  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.4  2005/07/28 20:57:49  edn2065
// Kalman filter working
//
// Revision 1.3  2005/07/28 18:10:33  edn2065
// Added SIKalman
//
// Revision 1.2  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.1  2005/07/28 00:22:24  edn2065
// Added SITordoff to repository
//

