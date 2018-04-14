#include <stdio.h>
#include "SIKalman.h"


/*****************************************************************
 * SIKalman ctor
 *****************************************************************
 */
SIKalman::SIKalman()
{
  kf_ph_kgk.init(2,1);
  kf_ph_kp1gk.init(2,1);
  kf_ph_kp1gkp1.init(2,1);
  kf_Ph_kgk.init(2,2);
  kf_Ph_kp1gk.init(2,2);  
  kf_Ph_kp1gkp1.init(2,2);  
  kf_F_k.init(2,2);
  kf_u_k.init(2,1);
  kf_Q.init(2,2);
  kf_Q_k.init(2,2);
  kf_mh_kgk.init(1,1);
  kf_mh_kp1gk.init(1,1);
  kf_m_kp1.init(1,1);
  kf_Mh_kp1.init(1,1);  
  kf_H_kp1.init(1,2);
  kf_R.init(1,1);
  kf_R_kp1.init(1,1);  
  kf_nu_kp1.init(1,1);
  kf_W_kp1.init(2,1);
}

/*****************************************************************
 * SIKalman::init
 *****************************************************************
 */
void SIKalman::init( )
{
  init( 0.05, 0.01 ); 
}  

/*****************************************************************
 * SIKalman::init
 *****************************************************************
 */
void SIKalman::init( FLOAT processNoise, FLOAT measurementNoise )
{
  Matrix stateCov(2,2);
  FLOAT covValue = 0.5;
  stateCov(0,0) = covValue;
  stateCov(0,1) = covValue;
  stateCov(1,0) = covValue;
  stateCov(1,1) = covValue;
  init( processNoise, measurementNoise, stateCov ); 
}  

/*****************************************************************
 * SIKalman::init
 *****************************************************************
 */
void SIKalman::init( FLOAT processNoise, FLOAT measurementNoise,
               Matrix &stateCovariance)
{
  //Set the process noise covariance
  kf_Q(0,0) = 1e-6/3.0;
  kf_Q(0,1) = 1e-6/2.0;
  kf_Q(1,0) = kf_Q(0,1);
  kf_Q(1,1) = 1e-6;

  //Scale it with Tordoff's defaults since I don't know how he gets
  //the value of Q_k                   
  kf_Q = kf_Q * (processNoise/0.03);
  kf_Q_k = kf_Q;
  
  //Set the measurement noise
  kf_R(0,0) = measurementNoise * measurementNoise;
  kf_R_kp1 = kf_R;
  
  //Set the initial state covariance
  kf_Ph_kp1gkp1 = stateCovariance;
  
  //Set the state transition matrix
  kf_F_k(0,0) = 1;
  kf_F_k(0,1) = 1;
  kf_F_k(1,0) = 0;
  kf_F_k(1,1) = 1;
  
  //Set the measurement transition matrix
  kf_H_kp1(0,0) = 1;
  kf_H_kp1(0,1) = 0;
  
  //Reset the number of iterations
  iteration = 0;
}               

/*****************************************************************
 * SIKalman::scaleProcessNoise
 *****************************************************************
 */
void SIKalman::scaleProcessNoise( FLOAT scalar )
{
  kf_Q_k = kf_Q * scalar;
}
   
/*****************************************************************
 * SIKalman::scaleMeasurementNoise
 *****************************************************************
 */
void SIKalman::scaleMeasurementNoise( FLOAT scalar )
{
  kf_R_kp1 = kf_R * scalar;
}
               
/*****************************************************************
 * SIKalman::setMeasurement
 *****************************************************************
 */
void SIKalman::setMeasurement(FLOAT x, FLOAT controlX, FLOAT zoom)
{

  //Set the measurement
  kf_m_kp1(0,0) = x;
  //and control inputs
  kf_u_k(0,0) = controlX;
  //Set the measurement transition matrix
  kf_H_kp1(0,0) = zoom;
      
  if( iteration == 0 )
  {
    //Save initial position    
    kf_ph_kp1gkp1(0,0) = x/zoom;
  }
  else
  {
    if( iteration == 1 )
    {
      //Save intitial position and velocity. Now initial conditions are set.
      kf_ph_kp1gkp1(1,0) = x/zoom - kf_ph_kp1gkp1(0,0);
      kf_ph_kp1gkp1(0,0) = x/zoom;
     
      //Do the first prediction 
      nextIteration();
      prediction();
  
      
    }
    
    //Run the Kalman filter
    runSIKalman();    
  
  }
    
  iteration++;

}      

/*****************************************************************
 * SIKalman::getCommand
 *****************************************************************
 */
FLOAT SIKalman::getCommand()
{

  return kf_ph_kp1gk(0,0);
  
}

/*****************************************************************
 * SIKalman::prediction
 *****************************************************************
 */
void SIKalman::prediction()
{

  //Update state prediction
  kf_ph_kp1gk = kf_F_k * kf_ph_kgk + kf_u_k;
  
  //Update state prediction covariance
  kf_Ph_kp1gk = kf_F_k * kf_Ph_kgk * (~kf_F_k) + kf_Q_k;
  
  //Update measurement prediction
  kf_mh_kp1gk = kf_H_kp1 * kf_ph_kp1gk;
  
  //Update measurement prediction covariance
  kf_Mh_kp1 = kf_H_kp1 * kf_Ph_kp1gk * (~kf_H_kp1) + kf_R_kp1;

}

/*****************************************************************
 * SIKalman::measurement
 *****************************************************************
 */
void SIKalman::measurement()
{

  kf_nu_kp1 = kf_m_kp1 - kf_mh_kp1gk;

}

/*****************************************************************
 * SIKalman::correction
 *****************************************************************
 */
void SIKalman::correction()
{

  kf_W_kp1 = kf_Ph_kp1gk * (~(kf_H_kp1)) * kf_Mh_kp1.inv();
  kf_ph_kp1gkp1 = kf_ph_kp1gk + kf_W_kp1 * kf_nu_kp1;
  kf_Ph_kp1gkp1 = kf_Ph_kp1gk - 
                      kf_W_kp1 * kf_Mh_kp1 * (~(kf_W_kp1));

}

/*****************************************************************
 * SIKalman::nextIteration
 *****************************************************************
 */
void SIKalman::nextIteration()
{

  //Move the k+1 | k+1 state estimate to the k|k estimate
  kf_ph_kgk = kf_ph_kp1gkp1;
  //Also for the state covariance estimate 
  kf_Ph_kgk = kf_Ph_kp1gkp1;

}

/*****************************************************************
 * SIKalman::runSIKalman
 *****************************************************************
 */
void SIKalman::runSIKalman()
{
  
  measurement();
  correction();
  nextIteration();
  prediction();

}

/*****************************************************************
 * SIKalman::getInnovation
 *****************************************************************
 */
FLOAT SIKalman::getInnovation()
{
  return kf_nu_kp1(0,0);
}

// File: $Id: SIKalman.cpp,v 1.5 2005/08/09 04:23:42 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SIKalman.cpp,v $
// Revision 1.5  2005/08/09 04:23:42  edn2065
// Scaled measurement noise to go with zoom
//
// Revision 1.4  2005/08/05 23:08:09  edn2065
// Made kalman filter for zooming
//
// Revision 1.3  2005/08/05 22:09:40  edn2065
// Moved kalman functionality into SIKalman
//
// Revision 1.2  2005/08/01 23:30:07  edn2065
// Added zoom invariance to KF
//
// Revision 1.1  2005/07/28 18:10:33  edn2065
// Added SIKalman
//

