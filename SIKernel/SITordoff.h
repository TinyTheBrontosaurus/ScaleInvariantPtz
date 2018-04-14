#ifndef SITORDOFF_H
#define SITORDOFF_H

#include "types.h"
#include "SIKalman.h"

/**************************************************************
 *
 * SITordoff class
 * Description: Implements Kalman filter and other algorithms described
 *              in Chapter 6 of Benjamin Tordoff's PhD dissertation.
 **************************************************************
 */
class SITordoff {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults for all options.
    ******************************************
    */
    SITordoff();
     
   /******************************************
    * dtor
    ******************************************
    */
    ~SITordoff();

   /******************************************
    * init
    * Description: Initializes the class.
    ******************************************
    */
    void init();
    
   /******************************************
    * init
    * Description: Initializes the class.
    * Parameters:  processNoise - The magnitude of the measurement noise
    *              measurementNoise - The magnitude of the measurement noise
    *              stateCovariance - The initial value of the state covariance
    ******************************************
    */
    void init( FLOAT processNoise, FLOAT measurementNoise, 
               FLOAT myPsi, FLOAT myGamma1, FLOAT myGamma2,
               Matrix &stateCovariance); 
    void init( FLOAT processNoise, FLOAT measurementNoise, 
               FLOAT myPsi, FLOAT myGamma1, FLOAT myGamma2);
   /******************************************
    * setMeasurement
    * Description: Registers a measurement corresponding to a frame.
    * Parameters:  x - The error in of the object in the image in the
    *                  x-direction. -0.5 is the left edge, 0.5 is the 
    *                  right edge.
    *              controlX - The control input in the X-direction
    *              y - The error in of the object in the image in the
    *                  y-direction. -0.5 is the top edge, 0.5 is the 
    *                  bottom edge.
    *              controlY - The control input in the Y-direction
    *              zoom - The zoom magnification at the time 
    *                     of measurement
    ******************************************
    */        
    void setMeasurement(FLOAT x, FLOAT controlX, FLOAT y, FLOAT controlY, FLOAT zoom);
    
   /******************************************
    * getCommands
    * Description: Retrieves the commands to be sent to the camera.
    * Parameters:  x - The error in of the object in the image in the
    *                  x-direction. -0.5 is the left edge, 0.5 is the 
    *                  right edge.
    *              y - The error in of the object in the image in the
    *                  y-direction. -0.5 is the top edge, 0.5 is the 
    *                  bottom edge.
    *              zoom - The desired zoom magnification 
    **********************************************************
    */            
    void getCommands(FLOAT &pan, FLOAT &tilt, FLOAT &zoom,
                     BOOL errOnZoomIn);
               
        
  protected:
  private:

       
   /******************************************
    * chooseZoom
    * Description: Choose the next zoom magnification (Tordoff 150)
    * Returns:     The desired focal length for the next iteration
    ******************************************
    */           
    FLOAT chooseZoom(BOOL errOnZoomIn);
    
   /******************************************
    * get2NormCovarianceSquared
    * Description: Get the squared 2-norm of the covariance (Tordoff 152)
    * Parameters:  lastCov - The value of the covariance last iteration
    *              gamma - The gamma value for calculation
    *              newCov - The newly calculated covariance
    *              eigValSquared - The 2-norm of the covariance squared
    * Returns:     The squared 2-norm of the covariance of nu_k
    ******************************************
    */            
    void get2NormCovarianceSquared( Matrix lastCov, FLOAT gamma, 
                                    Matrix &newCov, FLOAT &normSquared );
  
  private:
  
    //Keep track of the pan and tilt states and measurements 
    SIKalman kfPan;
    SIKalman kfTilt;
    
    //Count the number of iterations corresponding to when setMeasrement is called
    UINT32 iteration;
    
    //The current zoom value
    FLOAT curZoom;
    
    //The values of nu for pan and tilt
    Matrix nu_k;
    
    //Different values for focal length selection
    FLOAT gamma1;
    FLOAT gamma2;
    FLOAT psi;
    
    //The covariance of nu in the previous iteration for various values
    //of gamma
    Matrix lastGamma1Cov;
    Matrix lastGamma2Cov;
};

#endif


// File: $Id: SITordoff.h,v 1.7 2005/09/07 18:44:04 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: SITordoff.h,v $
// Revision 1.7  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.6  2005/08/05 22:09:40  edn2065
// Moved kalman functionality into SIKalman
//
// Revision 1.5  2005/08/03 02:34:54  edn2065
// Added tordoff psi and gammas to menu
//
// Revision 1.4  2005/08/03 01:43:26  edn2065
// Implemented focal length selection
//
// Revision 1.3  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.2  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.1  2005/07/28 00:22:24  edn2065
// Added SITordoff to repository
//
