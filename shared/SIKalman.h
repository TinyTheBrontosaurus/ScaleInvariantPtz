#ifndef SIKALMAN_H
#define SIKALMAN_H

#include "types.h"
#include "Matrix.h"

/**************************************************************
 *
 * SITordoff class
 * Description: A structure that holds all the matrices needed for a 
 *              Kalman filter in a single dimension
 **************************************************************
 */
class SIKalman
{
  public:
   
   /******************************************
    * ctor
    * Description: Sets defaults for all options.
    ******************************************
    */  
    SIKalman();

   /******************************************
    * init
    * Description: Initializes the class.
    * Parameters:  processNoise - The magnitude of the measurement noise
    *              measurementNoise - The magnitude of the measurement noise
    *              stateCovariance - The initial value of the state covariance
    ******************************************
    */        
    void init();
    void init( FLOAT processNoise, FLOAT measurementNoise );
    void init( FLOAT processNoise, FLOAT measurementNoise,
               Matrix &stateCovariance);
               
   /******************************************
    * scaleProcessNoise
    * Description: Multiplies the orginal process noise by a scalar to produce
    *              a new process noise.
    * Parameters:  scalar - the multiplier
    ******************************************
    */            
    void scaleProcessNoise( FLOAT scalar );
   
   /******************************************
    * scaleMeasurementNoise
    * Description: Multiplies the orginal measurement noise by a scalar to produce
    *              a new process noise.
    * Parameters:  scalar - the multiplier
    ******************************************
    */             
    void scaleMeasurementNoise( FLOAT scalar );
    
   /******************************************
    * setMeasurement
    * Description: Registers a measurement corresponding to a frame.
    * Parameters:  x - The measurement of the state
    *              controlX - The expected change in the state
    *              zoom - The zoom magnification at the time 
    *                     of measurement
    ******************************************
    */        
    void setMeasurement(FLOAT x, FLOAT controlX, FLOAT zoom);
    
   /******************************************
    * getCommands
    * Description: Retrieves the commands to be sent to the camera.
    * Return:      The estimate of the state
    **********************************************************
    */            
    FLOAT getCommand();   
    
   /******************************************
    * getInnovation
    * Description: Retrieves the innovation.
    * Return:      The innovation
    **********************************************************
    */            
    FLOAT getInnovation();   
     
  private: 
  
   /******************************************
    * runSIKalman
    * Description: Runs the measurement, correction, then prediction (Tordoff 143)
    ******************************************
    */           
    void runSIKalman();  

   /******************************************
    * prediction
    * Description: Does the prediction calculations as detailed in (Tordoff 143)
    * Parameters:  kf - The data on which the calculations are run
    ******************************************
    */
    void prediction();
   /******************************************
    * measurement
    * Description: Does the measurement calculations as detailed in (Tordoff 143)
    * Parameters:  kf - The data on which the calculations are run
    ******************************************
    */    
    void measurement();
   /******************************************
    * correction
    * Description: Does the correction calculations as detailed in (Tordoff 143)
    * Parameters:  kf - The data on which the calculations are run
    ******************************************
    */    
    void correction();

   /******************************************
    * nextIteration
    * Description: Changes the iteration as far as k+1 and k goes. To 
    *              be called before prediction and after correction (Tordoff 143)
    * Parameters:  kf - The data on which the calculations are run
    ******************************************
    */           
    void nextIteration();
    
  private:
    //Predicted state - k | k
    Matrix kf_ph_kgk;
    //Predicted state - k+1 | k
    Matrix kf_ph_kp1gk;
    //Predicted state - k+1 | k+1
    Matrix kf_ph_kp1gkp1;
    //Predicted state covariance - k | k
    Matrix kf_Ph_kgk;
    //Predicted state covariance - k+1 | k
    Matrix kf_Ph_kp1gk;  
    //Predicted state covariance - k+1 | k+1
    Matrix kf_Ph_kp1gkp1;  
    //State transition matrix
    Matrix kf_F_k;
    //Control input
    Matrix kf_u_k;
    //Process noise in general 
    Matrix kf_Q;
    //Process noise per iteration
    Matrix kf_Q_k;
    
    //Predicted measurement - k | k
    Matrix kf_mh_kgk;
    //Predicted measurement - k+1 | k
    Matrix kf_mh_kp1gk;
    //Actual measurement - k+1
    Matrix kf_m_kp1;
    //Predicted measurement covariance - k+1
    Matrix kf_Mh_kp1;  
    //Measurement transition matrix
    Matrix kf_H_kp1;
    //Measurement noise in general
    Matrix kf_R;
    //Measurement noise per iteration
    Matrix kf_R_kp1;
    
    //Innovation
    Matrix kf_nu_kp1;
    
    //Kalman gain - k+1
    Matrix kf_W_kp1;    
    
    //The number of the current iteration 
    UINT32 iteration;

};

#endif


// File: $Id: SIKalman.h,v 1.4 2005/08/05 23:08:09 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SIKalman.h,v $
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
