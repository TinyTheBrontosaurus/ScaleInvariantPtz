#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "structFlag.h"
extern "C" {
#include "2ndLevel.h"
}
#include "CommandLevel.h"

/*
 *
 * Version:
 *     $Id: CommandLevel.cpp,v 1.12 2005/08/25 22:12:23 edn2065 Exp $
 *
 * Revision:
 *     $Log: CommandLevel.cpp,v $
 *     Revision 1.12  2005/08/25 22:12:23  edn2065
 *     Made commandLevel a class. Added HZO pickoff point in SIKernel
 *
 *     Revision 1.11  2005/08/24 20:06:29  edn2065
 *     Put in band-aid for camera communications
 *
 *     Revision 1.10  2005/08/10 02:48:03  edn2065
 *     Changed to allow warning and error free ICC compile
 *
 *     Revision 1.9  2005/07/01 16:43:05  edn2065
 *     Created sine wave generator for camera
 *
 *     Revision 1.8  2005/06/30 14:46:28  edn2065
 *     overnight checkin
 *
 *     Revision 1.7  2005/06/27 18:49:55  edn2065
 *     added auto bps checking to amishs code
 *
 *     Revision 1.6  2005/06/22 19:48:16  edn2065
 *     Weekend commit
 *
 *     Revision 1.5  2005/06/22 16:57:40  edn2065
 *     Tied pantilt and zoom readings together to make them faster
 *
 *     Revision 1.4  2005/06/22 15:52:18  edn2065
 *     Created a new function that inquires both pan tilt and zoom in a single instruction
 *
 *     Revision 1.3  2005/06/14 14:40:21  edn2065
 *     Added better precision to pantilt commands as well
 *
 *     Revision 1.2  2005/06/14 12:19:58  edn2065
 *     Made PT inquiry a float instead of int. Camera resolution was being thrown away
 *
 *     Revision 1.1  2005/06/01 13:39:01  edn2065
 *     Adding Ameesh s code
 *
 *     Revision 1.1  2004/12/22 15:34:40  jar4773
 *     Initial revision
 *
 *
 * Author: Amish Rughoonundon
 * Company: Active vision lab
 *
 */

FILE *logfile;
struct timeval bt, et;
double time1, time2;
float delay1, delay2;
 
/* Gets optional parameters. */
#define GETPARAM \
va_list argp; \
int temp[4]; \
int num = 0; \
va_start(argp, name); \
while(num <=3) { \
temp[num]= va_arg(argp, int); \
num++; } \
va_end(argp);

/* Sony Command instruction headers */
#define CHEADER \
aCommand.send[0]= 0x80 | cam; \
aCommand.send[1]= 0x01; \
aCommand.send[2]= 0x04; \
aCommand.end = 0xFF;

/* Sony Inquiry instruction headers */
#define IHEADER \
aCommand.send[0]= 0x80 | cam; \
aCommand.send[1]= 0x09; \
aCommand.end = 0xFF;

/* Canon command header */
#define CCHEADER \
aCommand.send[0]= 0xFF; \
aCommand.send[1]= 0x30; \
aCommand.send[2]= 0x30; \
aCommand.end = 0xEF;

CommandLevel::CommandLevel()
{
  /* Global variables. */
  fd = 0; // The file descriptor for the serial port
  
  error = 0; // Error returned by functions.
  
  count = 3; // Count into the array.
  
  out = 0; // Return socket number
  
  speedPan = 1;
  speedTilt = 1; //Speed for pan and tilt

}

/******************************************************************************
 * ABSTRACTION LEVEL
 ******************************************************************************/
/*
 * Init:- Initializes the serial port and camera.
 *
 * PARAMS: serial:- A pointer to the serial port address.
 *
 * RETURN:- 0 if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::Init(char *serial, int cameranum) {
  
  // Initialize the array to be used for decimal to hexadecimal conversion.
  int i = 0;
  for(i=0; i<4;i++) {

    array[i] = 0x00;
  }
  
  // Initialize the serial port and camera.
  if((fd = Initialize(serial, &aFlag, cameranum)) < 0 ) return fd;
    
  return 0;
}

/*
 * Init:- Initializes the serial port and camera.
 *
 * PARAMS: serial:- A pointer to the serial port address.
 *
 * RETURN:- 0 if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::Init38400(char *serial, int cameranum) {
  
  // Initialize the array to be used for decimal to hexadecimal conversion.
  int i = 0;
  for(i=0; i<4;i++) {

    array[i] = 0x00;
  }
  
  // Initialize the serial port and camera.
  if((fd = Initialize38400(serial, &aFlag, cameranum)) < 0 ) return fd;
    
  return 0;
}

/*
 * ResetThePort:- Resets the port.
 *
 * PARAMS: serial:- A pointer to the serial port address.
 *
 * RETURN:- 0 if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::ResetThePort() {
  
  error = 0;

  if( (error = ResetPort(fd)) < 0) return error;
  
  return 0;
}

/*
 * SetManual:- Set everything in the camera to manual operation.
 *
 * PARAMS: cameranum: The number of the camera
 *
 * RETURN:- 0 if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::SetManual(int cameranum) {
  
  error = 0;

  //Check if sony is connected
  if( aFlag.sony_canon == 1) {
    
    if( (error = CAM_Focus(cameranum,1,ManualFocus)) < 0) return error;
    if( (error = CAM_WB(cameranum,1,Manual)) < 0) return error;
    if( (error = CAM_AE(cameranum,1,Manual)) < 0) return error;
    if( (error = CAM_SlowShutter(cameranum,1,Manual)) < 0) return error;
    if( (error = CAM_ExpComp(cameranum,1,OFF)) < 0) return error;
  }
  
  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {
    
    if( (error = PanMovableRangeAssignment(-100,100)) < 0) return error;
    if( (error = TiltMovableRangeAssignment(-30,90)) < 0) return error;
    if( (error = FocusManual()) < 0) return error;
    if( (error = BackLightCompensationOFF(1)) < 0) return error;
    if( (error = ExposureManual()) < 0) return error;
    if( (error = WhiteBalanceManualMode(1)) < 0) return error;
  }
  
  return 0;
}

/*
 * SetAuto:- Set everything in the camera to auto operation.
 *
 * PARAMS: cameranum: The number of the camera
 *
 * RETURN:- 0 if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::SetAuto(int cameranum) {
  
  error = 0;

  //Check if sony is connected
  if( aFlag.sony_canon == 1) {
   
    if( (error = CAM_Focus(cameranum,1,AutoFocus)) < 0) return error;
    printf("Autofocus on (%s @ %d)\n", __FILE__, __LINE__);
    if( (error = CAM_WB(cameranum,1,Auto)) < 0) return error;
    printf("White balance on (%s @ %d)\n", __FILE__, __LINE__);
    if( (error = CAM_AE(cameranum,1,FullAuto)) < 0) return error;
    printf("AE on (%s @ %d)\n", __FILE__, __LINE__);
    if( (error = CAM_SlowShutter(cameranum,1,Auto)) < 0) return error;
    printf("SlowShutter on (%s @ %d)\n", __FILE__, __LINE__);
    if( (error = CAM_ExpComp(cameranum,1,ON)) < 0) return error;
    printf("ExpComp on (%s @ %d)\n", __FILE__, __LINE__);
  }
  
  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {
    
    if( (error = CameraReset(1) < 0) ) return error;
  }

  return 0;
}

/*
 * PanAbsolutePosition:- Move camera to a certain position.
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous (0 for canon)
 *         pan: Pan position
 *         tilt: tilt position
 *         speedP: Speed of pan
 *         speedt: Speed of tilt
 *
 * RETURN:- 0 or socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::PanTiltAbsolutePosition(int cam, int sync, float pan,
			float tilt, float speedP, float speedT) {
  
  error = 0;

  //Check if sony is connected
  if( aFlag.sony_canon == 1) {

    if( (error = PanTiltDrive(cam, sync, AbsolutePosition,
			      (int)speedP, (int)speedT, pan, tilt)) < 0) return error;
  }
  
  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {
 
    if( (error = PanSpeedAssignment(speedP)) < 0) return error;
    
    if( (error = TiltSpeedAssignment(speedT)) < 0) return error;
    
    if( (error = PanTiltAngleAssignment((int)pan, (int)tilt, sync)) < 0) return error;
    
  }
  return error;
}

/*
 * PanRelativePosition:- Move camera to a certain position
 *                       relative to current position.
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *         pan: Pan position
 *         tilt: tilt position
 *         speedP: Speed of pan
 *         speedt: Speed of tilt
 *
 * RETURN:- 0 or socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::PanTiltRelativePosition(int cam, int sync, float pan,
			float tilt, float speedP, float speedT) {
  
  error = 0;
  float relArray[2];
  int old = 0;
  int rangeArray[2];

  //Check if sony is connected
  if( aFlag.sony_canon == 1) {
    
    if( (error = PanTiltDrive(cam, sync, RelativePosition,
			      (int) speedP, (int) speedT, pan, tilt)) < 0) return error;
  }
  
  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {
    
    //Set the pan and tilt speed
    if( (error = PanSpeedAssignment(speedP)) < 0) return error;
    if( (error = TiltSpeedAssignment(speedT)) < 0) return error;
    
    //Get the current position
    PanTiltAngleRequest(relArray);

    old = (int)relArray[0] + (int) pan;

    PanMovableRangeRequest(rangeArray);
   
    //If final position is over the limit set to limit position
    if( (old > rangeArray[1]) ||
	(old < rangeArray[0]) ) {

      if( pan < 0 ) {
	relArray[0] = rangeArray[0];
      }

      if( pan > 0 ) {
	relArray[0] = rangeArray[1];
      }
    }

    else relArray[0] = old;

    old = (int)relArray[1] + (int) tilt;
 
    TiltMovableRangeRequest(rangeArray);
  
    if( (old > rangeArray[1]) ||
	(old < rangeArray[0]) ) {

      if( tilt < 0 ) {
	relArray[1] = rangeArray[0];
      }

      if( tilt > 0 ) {
	relArray[1] = rangeArray[1];
      }
    }

    else relArray[1] = old;

    //Send to camera
    if( (error = PanTiltAngleAssignment((int)relArray[0], (int)relArray[1], sync)) < 0) return error;
  }
  
  return error;
}
 
/*
 * StopMoving:- Stop movement
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *         socket: The socket where the instruction was
 *                 stored in the sony camera (0 for canon)
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::StopMoving(int cam, int sync, int socket) {

  //Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = CommandCancel(cam,socket)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = PanTiltRunningStop()) < 0) return error;
  }
  
  return 0;
}

/*
 * PowerON:- Turn camera on
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::PowerON(int cam, int sync) {

//Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = CAM_Power(cam,sync,ON)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = CameraON(sync)) < 0) return error;
  }
  
  return error;
}

/*
 * PowerOFF:- Turn camera off
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::PowerOFF(int cam, int sync) {

//Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = CAM_Power(cam,sync,OFF)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = CameraOFF(sync)) < 0) return error;
  }
  
  return error;
}

/*
 * BackLightON:- Turn back light on
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::BackLightON(int cam, int sync) {

//Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = CAM_BackLight(cam,sync,ON)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = BackLightCompensationON(sync)) < 0) return error;
  }
  
  return error;
}

/*
 * BackLightOFF:- Turn back light off
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::BackLightOFF(int cam, int sync) {

//Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = CAM_BackLight(cam,sync,OFF)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = BackLightCompensationOFF(sync)) < 0) return error;
  }
  
  return error;
}

/*
 * SetMemory:- Set memory
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::SetMemory(int cam, int sync, int value) {

//Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = CAM_Memory(cam, sync, Set, value)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = PresetSet(value)) < 0) return error;
  }
  
  return error;
}

/*
 * RecallMemory:- Set camera to memory setting
 *
 * PARAMS: cam: The number of the camera
 *         sync: Synchronous or Asynchronous
 *
 * RETURN:- 0 or negative number
 *
 */
int CommandLevel::RecallMemory(int cam, int sync, int value) {
  
  //Check if sony is connected
  if( aFlag.sony_canon == 1) {
    
    if( (error = CAM_Memory(cam, sync, Recall, value)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {
    
    if( (error = PresetMove(value, sync)) < 0) return error;
  }
  
  return error;
}

/*
 * PositionRequest:- Get the current position in degrees
 *
 * PARAMS: cam: The number of the camera
 *         theArray: The array where to put the pana dn tilt positions
 *
 * RETURN:- 0 or socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::PositionRequest(int cam, float *theArray) {
  
  //Check if sony is connected
  if( aFlag.sony_canon == 1) {
  
    if( (error = PanTiltPositionInq(cam, theArray)) < 0) return error;
  }

  //Check if canon is connected
  else if( aFlag.sony_canon == 0) {

    if( (error = PanTiltAngleRequest(theArray)) < 0) return error;
  }
  
  return error;
}

int CommandLevel::ptCancel( int cam )
{
  return CommandCancel( cam, ptSocket );
}


/******************************************************************************
 * SONY COMMAND LIST
 ******************************************************************************/
/*
 * CommandCancel:- Cancels a previously sent command
 *
 * PARAMS: cam:- The camera number.
 *         socket:- The socket where the instruction was stored.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CommandCancel(int cam, int socket) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  aCommand.send[0]= 0x80 | cam;
  aCommand.send[1]= 0x20 | socket;
  aCommand.send[2]= 0xFF; 
  aCommand.sendLength = 3;
  aCommand.end = 0xFF;
  
  return SendAsynch(fd, &aCommand);
}

/*
 * CAM_Power:- Turns the camera ON and OFF
 *
 * PARAMS: cam:- The camera number.
 *         on_off:- 1 for ON and 0 for OFF.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Power(int cam, int sync, int on_off) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  CHEADER
    aCommand.send[3]= 0x00;
  
  if(on_off == ON) aCommand.send[4]= 0x02;

  else if(on_off == OFF) aCommand.send[4]= 0x03;

  else {

    fputs("14) Illegal on_off parameter.(CAM_Power)\n",logfile);
    fflush(logfile);
    return -14;
  }

  aCommand.send[5]= 0xFF;
  aCommand.sendLength = 6;
  
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Power)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_AutoPowerOff:- Set the timer to turn the camera off
 *                    if no signal is received.
 *
 * PARAMS: cam:- The camera number
 *         timeout:- The time delay.
 *
 * RETURN:- socket number if everything went well
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_AutoPowerOff(int cam, int sync, int timeout) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( (0<= timeout) && (65535 >= timeout) ) {

    CHEADER
      aCommand.send[3]= 0x40;

    DecConvert(timeout); // CHECK FOR FLOAT BEING PASSED.
    

    aCommand.send[4]= 0x00 | array[0];
    aCommand.send[5]= 0x00 | array[1];
    aCommand.send[6]= 0x00 | array[2];
    aCommand.send[7]= 0x00 | array[3];

    ResetArray(); // Reset the array content and counter.

    aCommand.send[8]= 0xFF;
    aCommand.sendLength = 9;
    aCommand.end = 0xFF;
  }

  else {
    fputs("14) Illegal timeout parameter.(CAM_AutoPowerOff)\n",logfile);
    fflush(logfile);
    return -14;
  }
    
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_AutoPowerOff)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Zoom:- Adjust the zoom controls  of the camera.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Zoom(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM;
 
  switch(name) {

  case Stop: // Stop Zoom
    aCommand.send[3]= 0x07;
    aCommand.send[4]= 0x00;  
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case TeleStd: // Zoom maximum
    aCommand.send[3]= 0x07;
    aCommand.send[4]= 0x02;  
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case WideStd: // Zoom minimum
    aCommand.send[3]= 0x07;
    aCommand.send[4]= 0x03;  
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case TeleVar: //Zoom maximum at different speeds
    aCommand.send[3]= 0x07;
    
    if( (0<=temp[0]) && (7>=temp[0]) ) {
      
      aCommand.send[4]= 0x20 | temp[0];
    }
    
    else {
      
      fputs("14) Illegal speed parameter.(CAM_Zoom_TeleVar)\n",logfile);
      fflush(logfile);
      return -14;
    }

    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;
    
  case WideVar: //Zoom minimum at different speeds
    aCommand.send[3]= 0x07;
    
    if( (0<=temp[0]) && (7>=temp[0]) ) {
      
      aCommand.send[4]= 0x30 | temp[0];
    }
    
    else {
    
      fputs("14) Illegal speed parameter.(CAM_Zoom_WideVar)\n",logfile);
      fflush(logfile);
      return -14;
    }
    
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Direct: //Zoom to different positions

    if( (0<=temp[0]) && (28672>=temp[0]) ) {

      aCommand.send[3]= 0x47;
      DecConvert(temp[0]); 
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal zoom parameter.(CAM_Zoom_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }
   
  case DZoomOn: // Digital zoom on
    aCommand.send[3]= 0x06;
    aCommand.send[4]= 0x02;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;
    
  case DZoomOff: // Digital zoom off
    aCommand.send[3]= 0x06;
    aCommand.send[4]= 0x03;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;
    
  default: // Unknown name
    fputs("15) Illegal name parameter.(CAM_Zoom)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
 
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Zoom)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Focus:- Adjust the focusing controls of the camera.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Focus(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Stop: //stop current focusing
    aCommand.send[3] = 0x08;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case FarStd://Far focus.
    aCommand.send[3] = 0x08;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case NearStd: // Near focus.
    aCommand.send[3] = 0x08;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case FarVar: //Far focus at different speed.
    aCommand.send[3]= 0x08;
    
    if( (0<=temp[0]) && (7>=temp[0]) ) {
      
      aCommand.send[4]= 0x20 | temp[0];
    }
    
    else {
      
      fputs("14) Illegal speed parameter.(CAM_Focus_FarVar)\n",logfile);
      fflush(logfile);
      return -14;
    }

    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case NearVar: // Near focus at different speeds.
    aCommand.send[3]= 0x08;
    
    if( (0<=temp[0]) && (7>=temp[0]) ) {
      
      aCommand.send[4]= 0x30 | temp[0];
    }
    
    else {
      
      fputs("14) Illegal speed parameter.(CAM_Focus_NearVar)\n",logfile);
      fflush(logfile);
      return -14;
    }

    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Focus at different positions

    if( (4096<=temp[0]) && (33792>=temp[0]) ) {

      aCommand.send[3]= 0x48;
      DecConvert(temp[0]); 
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal focus parameter.(CAM_Focus_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }
   
  case AutoFocus: // Set camera for Auto focus.
    aCommand.send[3] = 0x38;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case ManualFocus: // Set camera for manual focus.
    aCommand.send[3] = 0x38;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case AutoManual: // Switch from auto to manual and vice versa
    aCommand.send[3] = 0x38;
    aCommand.send[4] = 0x10;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case OnePushTrigger: // Trigger focus
    aCommand.send[3] = 0x18;
    aCommand.send[4] = 0x01;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Infinity: // Focus on infinity
    aCommand.send[3] = 0x18;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case AFSensHigh: // Set AF sensitivity High
    aCommand.send[3] = 0x58;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case AFSensLow: // SEt AF sensitivity Low
    aCommand.send[3] = 0x58;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case NearLimit: // Focus near the limit.
    if( (4096<=temp[0]) && (33792>=temp[0]) ) {

      aCommand.send[3]= 0x28;
      DecConvert(temp[0]); 
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal focus parameter.(CAM_Focus_NearLimit)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_Focus)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
      
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Focus)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_WB:- Adjust the white balance of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_WB(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case Auto: // Auto white balance.
    aCommand.send[3] = 0x35;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Indoor: // Set for indoor white balance.
    aCommand.send[3] = 0x35;
    aCommand.send[4] = 0x01;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Outdoor: // Set for outdoor white balance.
    aCommand.send[3] = 0x35;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case OnePushWB: // One push white balance mode
    aCommand.send[3] = 0x35;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case ATW:// Auto tracing white balance
    aCommand.send[3] = 0x35;
    aCommand.send[4] = 0x04;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Manual:// Set for manual control mode
    aCommand.send[3] = 0x35;
    aCommand.send[4] = 0x05;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case OnePushTrigger: // One push white balnce trigger
    aCommand.send[3] = 0x10;
    aCommand.send[4] = 0x05;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default: // Unknown name
    fputs("15) Illegal name parameter.(CAM_WB)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
    
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_WB)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_RGain:- Adjust the RGain of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_RGain(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the Rgain
    aCommand.send[3] = 0x03;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Up: // Increase red gain
    aCommand.send[3] = 0x03;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Down: // Decrease red gain
    aCommand.send[3] = 0x03;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  
  case Direct: // Adjust Rgain to a certain value.
    if( (0<=temp[0]) && (255>=temp[0]) ) {

      aCommand.send[3]= 0x43;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal R Gain parameter.(CAM_RGain_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_RGain)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_RGain)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_BGain:- Adjust the BGain of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_BGain(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the Bgain
    aCommand.send[3] = 0x04;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase blue gain
    aCommand.send[3] = 0x04;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // Decrease blue gain
    aCommand.send[3] = 0x04;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust Bgain to a certain value.
    if( (0<=temp[0]) && (255>=temp[0]) ) {

      aCommand.send[3]= 0x44;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal B Gain parameter.(CAM_BGain_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_BGain)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
   
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_BGain)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;

}

/*
 * CAM_AE:- Adjust the Aperture Exposure of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_AE(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case FullAuto: // Automatic exposure mode.
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Manual: // Manual control code.
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case ShutterPriority: // Shutter priority automatic exposure mode
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x0A;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case IrisPriority: // Iris priority automatic exposure mode
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x0B;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case GainPriority: // Gain priority automatic exposure mode
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x0C;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case Bright: // Bright mode (Manual control)
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x0D;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case ShutterAuto: // Automatic shutter mode
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x1A;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case IrisAuto: // Automatic Iris mode
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x1B;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case GainAuto: // Automatic Gain
    aCommand.send[3] = 0x39;
    aCommand.send[4] = 0x1C;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default: // Unknown name
    fputs("15) Illegal name parameter.(CAM_AE)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_AE)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_SlowShutter:- Set slow shutter to Auto or Manual.
 *
 * PARAMS: cam:- The camera number.
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_SlowShutter(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case Auto: // Slow shutter Auto
    aCommand.send[3] = 0x5A;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
    
  case Manual: //Slow shutter Manual
    aCommand.send[3] = 0x5A;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
    
  default:
    fputs("15) Illegal name parameter.(CAM_SlowShutter)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_SlowShutter)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Shutter:- Adjust the Shutter of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Shutter(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the shutter settings
    aCommand.send[3] = 0x0A;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase the shutter size
    aCommand.send[3] = 0x0A;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // decrease the shutter size
    aCommand.send[3] = 0x0A;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust shutter position to a certain value.
    if( (0<=temp[0]) && (19>=temp[0]) ) {

      aCommand.send[3]= 0x4A;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_Shutter_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_Shutter)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Shutter)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Iris:- Adjust the Iris of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Iris(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the Iris settings
    aCommand.send[3] = 0x0B;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase the iris size
    aCommand.send[3] = 0x0B;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // Decrease the iris size
    aCommand.send[3] = 0x0B;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust Iris position to a certain value.
    if( (0<=temp[0]) && (17>=temp[0]) ) {

      aCommand.send[3]= 0x4B;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_Iris_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_Iris)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Iris)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Gain:- Adjust the Gain of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Gain(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the gain settings
    aCommand.send[3] = 0x0C;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase the gain
    aCommand.send[3] = 0x0C;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // decrease the gain
    aCommand.send[3] = 0x0C;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust Iris position to a certain value.
    if( (0<=temp[0]) && (7>=temp[0]) ) {

      aCommand.send[3]= 0x4C;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_Gain_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_Gain)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Gain)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Bright:- Adjust the Brightness of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Bright(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the Bright settings
    aCommand.send[3] = 0x0D;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase the bright
    aCommand.send[3] = 0x0D;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // Decrease the bright
    aCommand.send[3] = 0x0D;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust Iris position to a certain value.
    if( (0<=temp[0]) && (23>=temp[0]) ) {

      aCommand.send[3]= 0x4D;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_Bright_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_Bright)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Bright)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;

}

/*
 * CAM_ExpComp:- Adjust the Exposure compensation of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_ExpComp(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case ON: // Set exposure compensation on
    aCommand.send[3] = 0x3E;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case OFF: // Set exposure compensation off
    aCommand.send[3] = 0x3E;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Reset: // Reset the exposure compensation settings
    aCommand.send[3] = 0x0E;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase the exposure compensation
    aCommand.send[3] = 0x0E;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // decrease the exposure compensation
    aCommand.send[3] = 0x0E;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust the exposure compensation to a certain value.
    if( (0<=temp[0]) && (14>=temp[0]) ) {

      aCommand.send[3]= 0x4E;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_ExpComp_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_ExpComp)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_ExpComp)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_BackLight:- Adjust the BackLight compensation of the camera
 *
 * PARAMS: cam:- The camera number.
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_BackLight(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case ON: // Set Back Light on
    aCommand.send[3] = 0x33;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case OFF: // Set Back Light off
    aCommand.send[3] = 0x33;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(CAM_BackLight)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_BackLight)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;

}

/*
 * CAM_Aperture:- Adjust the aperture of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Aperture(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case Reset: // Reset the aperture settings
    aCommand.send[3] = 0x02;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Up: // Increase the aperture
    aCommand.send[3] = 0x02;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Down: // Decrease the aperture
    aCommand.send[3] = 0x02;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Direct: // Adjust the aperture to a certain value.
    if( (0<=temp[0]) && (15>=temp[0]) ) {

      aCommand.send[3]= 0x42;
      DecConvert(temp[0]);
      aCommand.send[4]= 0x00 | array[0];
      aCommand.send[5]= 0x00 | array[1];
      aCommand.send[6]= 0x00 | array[2];
      aCommand.send[7]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_Aperture_Direct)\n",logfile);
      fflush(logfile);
      return -14;
    }

  default:
    fputs("15) Illegal name parameter.(CAM_Aperture)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
   
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Aperture)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;

}

/*
 * CAM_Wide:- Adjust the Wide mode setting of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Wide(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case OFF: // Turn wide view off
    aCommand.send[3] = 0x60;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case Cinema: // Set to cinema view
    aCommand.send[3] = 0x60;
    aCommand.send[4] = 0x01;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;
  case Full: // set tp 16:9 Full view
    aCommand.send[3] = 0x60;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(CAM_Wide)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
  
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Wide)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_LR_Reverse:- Adjust the Mirror imaging setting of the camera
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_LR_Reverse(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case ON: // Turn mirror on
    aCommand.send[3] = 0x61;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case OFF: // Turn mirror off
    aCommand.send[3] = 0x61;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(CAM_LR_Reverse)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
 
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_LR_Reverse)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Freeze:- Freeze the current picture
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Freeze(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {

  case ON: // Turn freeze on
    aCommand.send[3] = 0x62;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case OFF: // Turn freeze off
    aCommand.send[3] = 0x62;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(CAM_Freeze)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
  
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_Freeze)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_PictureEffect:- Adds effects to  the current picture
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_PictureEffect(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  switch(name) {
    
  case OFF: // Turn picture effect off.
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Pastel:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x01;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case NegArt:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Sepia:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case BandW:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x04;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Solarize:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x05;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Mosaic:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x06;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Slim:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x07;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Stretch:
    aCommand.send[3] = 0x63;
    aCommand.send[4] = 0x08;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(CAM_PictureEffect)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;
 
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_PictureEffect)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_DigitalEffect:- Adds Digital effect to the picture
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_DigitalEffect(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  GETPARAM; // Get variable parameters if any

  switch(name) {

  case OFF:
    aCommand.send[3] = 0x64;
    aCommand.send[4] = 0x00;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Still:
    aCommand.send[3] = 0x64;
    aCommand.send[4] = 0x01;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Flash:
    aCommand.send[3] = 0x64;
    aCommand.send[4] = 0x02;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Lumi:
    aCommand.send[3] = 0x64;
    aCommand.send[4] = 0x03;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case Trail:
    aCommand.send[3] = 0x64;
    aCommand.send[4] = 0x04;
    aCommand.send[5] = 0xFF;
    aCommand.sendLength = 6;
    break;

  case EffectLevel: 
    if( (0<=temp[0]) && (32>=temp[0]) ) {

      aCommand.send[3]= 0x65;
      aCommand.send[4]= temp[0];
      aCommand.send[5]= 0xFF;
      aCommand.sendLength = 6;
      break;
    }
    
    else {

      fputs("14) Illegal position parameter.(CAM_DigitalEffect_EffectLevel)\n",logfile);
      fflush(logfile);
      return -14;
    }
    break;

  default:
    fputs("15) Illegal name parameter.(CAM_DigitalEffect)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Adds the header and end bytes. */
  CHEADER;

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(CAM_DigitalEffect)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * CAM_Memory:- Sets the camera memory.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         memory:- The memory to use.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::CAM_Memory(int cam, int sync, int name, int memory) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( (0<=memory) && (5>=memory) ) DecConvert(memory);
    
  else {

    fputs("14) Illegal memory address parameter.(CAM_Memory)\n",logfile);
    fflush(logfile);
    return -14;
  }

  switch(name) {
    
  case Reset:
    aCommand.send[3]= 0x3F;
    aCommand.send[4]= 0x00;
    aCommand.send[5]= 0x00 | array[3];
    aCommand.send[6]= 0xFF;
    aCommand.sendLength = 7;
    break;

  case Set:
    aCommand.send[3]= 0x3F;
    aCommand.send[4]= 0x01;
    aCommand.send[5]= 0x00 | array[3];
    aCommand.send[6]= 0xFF;
    aCommand.sendLength = 7;
    break;

  case Recall:
    aCommand.send[3]= 0x3F;
    aCommand.send[4]= 0x02;
    aCommand.send[5]= 0x00 | array[3];
    aCommand.send[6]= 0xFF;
    aCommand.sendLength = 7;
    break;
    
  default:
    fputs("15) Illegal name parameter.(CAM_Memory)\n",logfile);
    fflush(logfile);
    return -15;
  }

  ResetArray(); // Reset the array content and counter.

  /* Adds the header and end bytes. */
  CHEADER;
 
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }//amish

  else {

    fputs("17) Illegal sync parameter.(CAM_Memory)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * DataScreen:- Show or hides the data screen.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::DataScreen(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  aCommand.send[0]= 0x80 | cam;
  aCommand.send[1]= 0x01;
  aCommand.send[2]= 0x06;
  aCommand.end = 0xFF;
 
  switch(name) {
    
  case ON: // Turn data screen on
    aCommand.send[3]= 0x06;
    aCommand.send[4]= 0x02;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case OFF: // Turn data screen off
    aCommand.send[3]= 0x06;
    aCommand.send[4]= 0x03;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case ONOFF: // Toggle data screen on and off
    aCommand.send[3]= 0x06;
    aCommand.send[4]= 0x10;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(DataScreen)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(DataScreen)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * IR_Receive:- Receive IR or not.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::IR_Receive(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  aCommand.send[0]= 0x80 | cam;
  aCommand.send[1]= 0x01;
  aCommand.send[2]= 0x06;
  aCommand.end = 0xFF;

  switch(name) {
    
  case ON: // Start receiving IR
    aCommand.send[3]= 0x08;
    aCommand.send[4]= 0x02;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case OFF: // Stop receiving IR
    aCommand.send[3]= 0x08;
    aCommand.send[4]= 0x03;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  case ONOFF: // Toggle IR receiving on and off   
    aCommand.send[3]= 0x08;
    aCommand.send[4]= 0x10;
    aCommand.send[5]= 0xFF;
    aCommand.sendLength = 6;
    break;

  default:
    fputs("15) Illegal name parameter.(IR_Receive)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(IR_Receive)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * IR_ReceiveReturn:- Receive IR messages fromor not.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::IR_ReceiveReturn(int cam, int sync, int name) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  aCommand.send[0]= 0x80 | cam;
  aCommand.send[1]= 0x01;
  aCommand.send[2]= 0x7D;
  aCommand.end = 0xFF;

  switch(name) {
    
  case ON: // Start receiving IR messages
    aCommand.send[3]= 0x01;
    aCommand.send[4]= 0x03;
    aCommand.send[5]= 0x00;
    aCommand.send[6]= 0x00;
    aCommand.send[7]= 0xFF;
    aCommand.sendLength = 8;
    break;

  case OFF: // Stop receiving IR messages
    aCommand.send[3]= 0x01;
    aCommand.send[4]= 0x13;
    aCommand.send[5]= 0x00;
    aCommand.send[6]= 0x00;
    aCommand.send[7]= 0xFF;
    aCommand.sendLength = 8;
    break;

  default:
    fputs("15) Illegal name parameter.(IR_ReceiveReturn)\n",logfile);
    fflush(logfile);
    return -15;
  }
 
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(IR_ReceiveReturn)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/*
 * PantiltDrive:- Moves the camera around
 *
 * PARAMS: cam:- The camera number
 *         sync:- Send synchronously or Asynchronously.
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- 0 if everything went well for synchronous message.
 *          socket number if everything went well for asynchronous message.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::PanTiltDrive(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  // Get variable parameters if any

  va_list argp; 
  int tempi[2];
  float tempf[2]; 
  int num = 0; 
  va_start(argp, name); 
  while(num <=1) { 
    tempi[num]= (int) va_arg(argp, int); 
    num++; }
  num = 0;
 while(num <=1) { 
    tempf[num]= (float) va_arg(argp, double); 
    num++; }
  va_end(argp);


  int angle = 0, i = 0, count = 0;

  /* Header and end byte. */
  aCommand.send[0]= 0x80 | cam;
  aCommand.send[1]= 0x01;
  aCommand.send[2]= 0x06;
  aCommand.end = 0xFF;
  
  switch(name) {
    
  case Up: // Set speed of pan and tilt. Move up
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x03;
      aCommand.send[7]= 0x01;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_Up)\n",logfile);
      fflush(logfile);
      return -14;
    }
     
  case Down: // Set speed of pan and tilt. Move down
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x03;
      aCommand.send[7]= 0x02;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_Down)\n",logfile);
      fflush(logfile);
      return -14;
    }
  
  case Left: // Set speed of pan and tilt. Move left
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x01;
      aCommand.send[7]= 0x03;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
	
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_Left)\n",logfile);
      fflush(logfile);
      return -14;
    }
  
  case Right: // Set speed of pan and tilt. Move right
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x02;
      aCommand.send[7]= 0x03;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }
    
    else {
      
      fputs("14) Illegal speed parameter.(PanTiltDrive_Right)\n",logfile);
      fflush(logfile);
      return -14;
    }

  case UpLeft: // Set speed of pan and tilt. Move up and left
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x01;
      aCommand.send[7]= 0x01;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_UpLeft)\n",logfile);
      fflush(logfile);
      return -14;
    }

  case UpRight: // Set speed of pan and tilt. Move up and right
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x02;
      aCommand.send[7]= 0x01;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_UpRight)\n",logfile);
      fflush(logfile);
      return -14;
    }

  case DownLeft: // Set speed of pan and tilt. Move down and left
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x01;
      aCommand.send[7]= 0x02;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_DownLeft)\n",logfile);
      fflush(logfile);
      return -14;
    }

  case DownRight: // Set speed of pan and tilt. Move down and right
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x02;
      aCommand.send[7]= 0x02;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_DownRight)\n",logfile);
      fflush(logfile);
      return -14;
    }

  case Stop: // Stop moving
    if( (1<=tempi[0]) && (24>=tempi[0]) && (1<=tempi[1]) && (20>=tempi[1]) ) {
      aCommand.send[3]= 0x01;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];
      aCommand.send[6]= 0x03;
      aCommand.send[7]= 0x03;
      aCommand.send[8]= 0xFF;
      aCommand.sendLength = 9;
      break;
    }

    else {

      fputs("14) Illegal speed parameter.(PanTiltDrive_Stop)\n",logfile);
      fflush(logfile);
      return -14;
    }

  case AbsolutePosition: // Move to a certain position relative to 0
    
    if( (1<=tempi[0]) && (24>=tempi[0]) &&
	(1<=tempi[1]) && (20>=tempi[1]) &&
	(-100.0<=tempf[0]) && (100.0>=tempf[0]) && 
	(-25.0<=tempf[1]) && (25.0>=tempf[1]) ) {
      
      aCommand.send[3]= 0x02;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];

      angle = (int)abs((int)(tempf[0]/CAM_PANTILT_DEGREES_PER_UNIT)); // Get decimal representation

      // Convert to appropriate number format for camera
      count = 0;
      if(tempf[0] < 0.0) {
	
	for(i=0;i<angle;i++) {
	  
	  count--;
	}
	count = count & 0x0000FFFF;
      }
      
      else {
	
	for(i=0;i<angle;i++) {
	  
	  count++;
	}
      }
      
      DecConvert(count);
      aCommand.send[6]= 0x00 | array[0];
      aCommand.send[7]= 0x00 | array[1];
      aCommand.send[8]= 0x00 | array[2];
      aCommand.send[9]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.

      angle = (int)abs((int)(tempf[1]/CAM_PANTILT_DEGREES_PER_UNIT)); // Get decimal representation

      // Convert to appropriate number format for camera
      count = 0;
      if(tempf[1] < 0.0) {
	
	for(i=0;i<angle;i++) {
	  
	  count--;
	}
	count = count & 0x0000FFFF;
      }
      
      else {
	
	for(i=0;i<angle;i++) {
	  
	  count++;
	}
      }
      
      DecConvert(count);
      aCommand.send[10]= 0x00 | array[0];
      aCommand.send[11]= 0x00 | array[1];
      aCommand.send[12]= 0x00 | array[2];
      aCommand.send[13]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[14]= 0xFF;
      aCommand.sendLength = 15;
      break;
    }
    
    else {
      
      fputs("14) Illegal speed, pan or tilt parameter.(PanTiltDrive_Absolute Position)\n",logfile);
      fflush(logfile);
      return -14;
    }
    
  case RelativePosition: // Move to a certain position realtive to current position
    if( (1<=tempi[0]) && (24>=tempi[0]) &&
	(1<=tempi[1]) && (20>=tempi[1]) &&
	(-200<=tempf[0]) && (200>=tempf[0]) &&
	(-50<=tempf[1]) && (50>=tempf[1]) ) {
      
      aCommand.send[3]= 0x03;
      aCommand.send[4]= tempi[0];
      aCommand.send[5]= tempi[1];

      angle = (int)abs((int)(tempf[0]/0.0694)); // Get decimal representation

      // Convert to appropriate number format for camera
      count = 0;
      if(tempf[0] < 0) {
	
	for(i=0;i<angle;i++) {
	  
	  count--;
	}
	count = count & 0x0000FFFF;
      }
      
      else {
	
	for(i=0;i<angle;i++) {
	  
	  count++;
	}
      }

      DecConvert(count);
      aCommand.send[6]= 0x00 | array[0];
      aCommand.send[7]= 0x00 | array[1];
      aCommand.send[8]= 0x00 | array[2];
      aCommand.send[9]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.

      angle = (int)abs((int)(tempf[1]/0.0694)); // Get decimal representation

      // Convert to appropriate number format for camera
      count = 0;
      if(tempf[1] < 0) {
	
	for(i=0;i<angle;i++) {
	  
	  count--;
	}
	count = count & 0x0000FFFF;
      }
      
      else {
	
	for(i=0;i<angle;i++) {
	  
	  count++;
	}
      }

      DecConvert(count);
      aCommand.send[10]= 0x00 | array[0];
      aCommand.send[11]= 0x00 | array[1];
      aCommand.send[12]= 0x00 | array[2];
      aCommand.send[13]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[14]= 0xFF;
      aCommand.sendLength = 15;
      break;
    }
    
    else {
      
      fputs("14) Illegal speed, pan or tilt parameter.(PanTiltDrive_RelativePosition)\n",logfile);
      fflush(logfile);
      return -14;
    }
    
  case Home: // Move to position 0000
    aCommand.send[3]= 0x04;
    aCommand.send[4]= 0xFF;
    aCommand.sendLength = 5;
    break;
      
  case Reset: // Reset drive
    aCommand.send[3]= 0x05;
    aCommand.send[4]= 0xFF;
    aCommand.sendLength = 5;
    break;
    
  default:
    fputs("15) Illegal name parameter.(PanTiltDrive)\n",logfile);
    fflush(logfile);
    return -15;
  }
  
  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
    
    //Set the socket
    ptSocket = aCommand.reply[1] && 0x0F;
  }

  else {

    fputs("17) Illegal sync parameter.(PanTiltDrive)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

int CommandLevel::getFd()
{
  return fd;
}

void CommandLevel::setFd( int newFd )
{
  fd = newFd;
}

/*
 * PantiltLimitSet:- Sets the limit of where the camera can move.
 *
 * PARAMS: cam:- The camera number
 *         name:- The name of the instruction.
 *         ...:- Optional parameters depending on name.
 *
 * RETURN:- socket number if everything went well.
 *          negative number if something went wrong.
 *
 */
int CommandLevel::PanTiltLimitSet(int cam, int sync, int name, ...) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  // Get variable parameters if any
  va_list argp; 
  int tempi[1];
  int tempf[2]; 
  int num = 0; 
  va_start(argp, name); 
  
    tempi[0]= (int) va_arg(argp, int); 
  
  while(num <=1) { 
    tempf[num]= (int) va_arg(argp, double); 
    num++; }
  va_end(argp);
  
  int angle, i, count;

  /* Header and end byte. */
  aCommand.send[0]= 0x80 | cam;
  aCommand.send[1]= 0x01;
  aCommand.send[2]= 0x06;
  aCommand.end = 0xFF;
  
  switch(name) {
    
  case LimitSet: // Set limit 1 is right and 0 is left
        
    if( ( (tempi[0] == 1) | (tempi[0] == 0) ) &&
	(-100<=tempf[0]) && (100>=tempf[0]) && 
	(-25<=tempf[1]) && (25>=tempf[1]) ) {
      
      aCommand.send[3]= 0x07;
      aCommand.send[4]= 0x00;
      aCommand.send[5]= 0x00 | tempi[0];

      angle = (int)abs((int)(tempf[0]/0.0694)); // Get decimal representation

      // Convert to appropriate number format for camera
      count = 0;
      if(tempf[0] < 0) {
	
	for(i=0;i<angle;i++) {
	  
	  count--;
	}
	count = count & 0x0000FFFF;
      }
      
      else {
	
	for(i=0;i<angle;i++) {
	  
	  count++;
	}
      }
      
      DecConvert(count);
      aCommand.send[6]= 0x00 | array[0];
      aCommand.send[7]= 0x00 | array[1];
      aCommand.send[8]= 0x00 | array[2];
      aCommand.send[9]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.

      angle = (int)abs((int)(tempf[1]/0.0694)); // Get decimal representation

      // Convert to appropriate number format for camera
      count = 0;
      if(tempf[1] < 0) {
	
	for(i=0;i<angle;i++) {
	  
	  count--;
	}
	count = count & 0x0000FFFF;
      }
      
      else {
	
	for(i=0;i<angle;i++) {
	  
	  count++;
	}
      }
      
      DecConvert(count);
      aCommand.send[10]= 0x00 | array[0];
      aCommand.send[11]= 0x00 | array[1];
      aCommand.send[12]= 0x00 | array[2];
      aCommand.send[13]= 0x00 | array[3];
      ResetArray(); // Reset the array content and counter.
      aCommand.send[14]= 0xFF;
      aCommand.sendLength = 15;
      break;
    }
    
    else {
      
      fputs("14) Illegal parameter.(PanTiltLimitSet_LimitSet)\n",logfile);
      fflush(logfile);
      return -14;
    }
    
  case LimitClear: // Set limit to Maximum
 
    if( (tempi[0] == 1) | (tempi[0] == 0) ) {
      
      aCommand.send[3]= 0x07;
      aCommand.send[4]= 0x01;
      aCommand.send[5]= 0x00 | tempi[0];
 
      aCommand.send[6]= 0x07;
      aCommand.send[7]= 0x0F;
      aCommand.send[8]= 0x0F;
      aCommand.send[9]= 0x0F;
      
      aCommand.send[10]= 0x07;
      aCommand.send[11]= 0x0F;
      aCommand.send[12]= 0x0F;
      aCommand.send[13]= 0x0F;
      aCommand.send[14]= 0xFF;
      aCommand.sendLength = 15;
      break;
    }
    
    else {
      
      fputs("14) Illegal  parameter.(PanTiltLimitSet_LimitClear)\n",logfile);
      fflush(logfile);
      return -14;
    }
    
  default:
    fputs("15) Illegal name parameter.(PanTiltLimitSet)\n",logfile);
    fflush(logfile);
    return -15;
  }

  /* Check if instruction is to be sent Synchronously or Asynchronously. */
  if(sync == 1) {

    if((error = SendSynchSony(fd, &aCommand, 5)) < 0) {

      return error;
    }
  }

  else if(sync == 0) {
      
    out = SendAsynch(fd, &aCommand);
  }

  else {

    fputs("17) Illegal sync parameter.(PanTiltLimitSet)\n",logfile);
    fflush(logfile);
    return -17;
  }
 
  return out;
}

/******************************************************************************
 * SONY INQUIRY COMMANDS
 ******************************************************************************/

/*
 * CAM_PowerInq:- Checks camera power status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- ON Camera is on
 *          OFF Camera is off
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_PowerInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x00;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_PowerInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_AutoPowerOffInq:- Checks camera auto power off status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Time in minutes
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_AutoPowerOffInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int timer = 0;
  int output;
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x40;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {
    timer =  HexConvert(&aCommand,2);

    if( (0<= timer) && (65535 >= timer) ) {
      output = timer;
    }

    else {
      fputs("13) Unknown answer.(CAM_AutoPowerOffInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_DZoomModeInq:- Checks camera zoom status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- ON Digital zoom is on
 *          OFF Digital zoom is off
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_DZoomModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x06;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_DZoomModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_ZoomPosInq:- Checks camera zoom position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Focal length
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_ZoomPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x47;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);

    if( (0<= position) && (28672 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_ZoomPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_FocusModeInq:- Checks camera focus status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- ON Auto focus is on
 *          OFF Auto focus is off
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_FocusModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x38;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_FocusModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_FocusPosInq:- Checks camera focus position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Focus position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_FocusPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x48;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {
    
    position =  HexConvert(&aCommand,2);

    if( (4096<= position) && (33792 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_FocusPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_AFModeInq:- Checks Auto Focus sensitivity status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- HIGH AF sensitivity is High
 *          LOW AF sensitivity is low
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_AFModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x58;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return HIGH;

  else if(aCommand.reply[2] == 0x03) return LOW;

  else {

    fputs("13) Unknown answer.(CAM_AFModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_FocusNearLimitInq:- Checks camera focus near limit position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Focus near limit position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_FocusNearLimitInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x28;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);

    if( (4096<= position) && (33792 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_FocusNearLimitInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_WBModeInq:- Checks current white balance setting
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- White balance setting
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_WBModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x35;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    if(aCommand.reply[2] == 0) {
      output = Auto;
    } 
    
    else if(aCommand.reply[2] == 1 ) {
      
      output = Indoor;
    }
    else if(aCommand.reply[2] == 2 ) {
      
      output = Outdoor;
    }
    else if(aCommand.reply[2] == 3 ) {
      
      output = OnePushWB;
    }
    else if(aCommand.reply[2] == 4 ) {
    
      output = ATW;
    }
    else if(aCommand.reply[2] == 5 ) {
      
      output = Manual;
    }
    else {
      fputs("13) Unknown answer.(CAM_WBModeInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
  
  return output;
}

/*
 * CAM_RGainInq:- Checks R Gain
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- R Gain value
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_RGainInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x43;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (256 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_RGainInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_BGainInq:- Checks B Gain
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- B Gain value
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_BGainInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x44;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (256 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_BGainInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_AEModeInq:- Checks Auto Exposure setting
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Auto exposure setting
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_AEModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x39;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    if(aCommand.reply[2] == 0) {
      output = FullAuto;
    } 
    
    else if(aCommand.reply[2] == 3 ) {
      
      output = Manual;
    }
    else if(aCommand.reply[2] == 0x0A ) {
      
      output = ShutterPriority;
    }
    else if(aCommand.reply[2] == 0x0B ) {
      
      output = IrisPriority;
    }
    else if(aCommand.reply[2] == 0x0C ) {
    
      output = GainPriority;
    }
    else if(aCommand.reply[2] == 0x0D ) {
      
      output = Bright;
    }

    else if(aCommand.reply[2] == 0x1A ) {
      
      output = ShutterAuto;
    }
    else if(aCommand.reply[2] == 0x1B ) {
  
      output = IrisAuto;
    }
    else if(aCommand.reply[2] == 0x1C ) {
      
      output = GainAuto;
    }
    else {
      fputs("13) Unknown answer.(CAM_AEModeInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
  
  return output;
}

/*
 * CAM_SlowShutterModeInq:- Checks camera focus status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Auto Slow shutter is on Auto
 *          Manual Slow shutter is on manual
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_SlowShutterModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x5A;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return Auto;

  else if(aCommand.reply[2] == 0x03) return Manual;

  else {

    fputs("13) Unknown answer.(CAM_SlowShutterModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_ShutterPosInq:- Checks the shutter position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Shutter position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_ShutterPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x4A;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (19 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_ShutterPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_IrisPosInq:- Checks the Iris position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Iris position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_IrisPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x4B;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (17 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_IrisPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_GainPosInq:- Checks the Gain position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Gain position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_GainPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x4C;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (7 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_GainPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_BrightPosInq:- Checks the Bright position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Bright position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_BrightPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x4D;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (23 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_BrightPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_ExpCompModeInq:- Checks exposure compensation status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- ON Exp comp is ON
 *          OFF Exp comp is OFF
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_ExpCompModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x3E;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_ExpCompModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_ExpCompPosInq:- Checks the Exposure Compensation position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Exposure Compensation position
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_ExpCompPosInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int position = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x4E;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    position =  HexConvert(&aCommand,2);
  
    if( (0<= position) && (14 >= position) ) {
      output = position;
    }

    else {
      fputs("13) Unknown answer.(CAM_ExpCompPosInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_BackLightModeInq:- Checks backlight status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN: ON:- backlight is ON
 *          OFF:- backlight is OFF
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_BackLightModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x33;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_BacklightModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_ApertureInq:- Checks the aperture gain
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Aperture gain
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_ApertureInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int gain = 0;
  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x42;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    gain =  HexConvert(&aCommand,2);
  
    if( (0<= gain) && (15 >= gain) ) {
      output = gain;
    }

    else {
      fputs("13) Unknown answer.(CAM_ApertureInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
 
  return output;
}

/*
 * CAM_WideModeInq:- Checks Wide mode setting
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Wide mode setting
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_WideModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x60;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    if(aCommand.reply[2] == 0) {
      output = OFF;
    } 
    
    else if(aCommand.reply[2] == 1 ) {
      
      output = Cinema;
    }
    else if(aCommand.reply[2] == 2 ) {
      
      output = Full;
    }
    else {
      fputs("13) Unknown answer.(CAM_WideModeInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
  
  return output;
}

/*
 * CAM_LR_ReverseModeInq:- Checks Left Rigt inverse status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN: ON:- LR reverse is ON
 *          OFF:- LR reverse is OFF
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_LR_ReverseModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x61;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_LR_ReverseModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_FreezeModeInq:- Checks if freeze mode is on
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN: ON:- Freeze mode is ON
 *          OFF:- Freeze mode is OFF
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_FreezeModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x62;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(CAM_FreezeModeInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_PictureEffectModeInq:- Checks current picture effect
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Picture effect currently being used
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_PictureEffectModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x63;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    if(aCommand.reply[2] == 0) {

      output = OFF;
    } 
    
    else if(aCommand.reply[2] == 1 ) {
      
      output = Pastel;
    }
    else if(aCommand.reply[2] == 2 ) {
      
      output = NegArt;
    }
    else if(aCommand.reply[2] == 3 ) {
      
      output = Sepia;
    }
    else if(aCommand.reply[2] == 4 ) {
    
      output = BandW;
    }
    else if(aCommand.reply[2] == 5 ) {
      
      output = Solarize;
    }

    else if(aCommand.reply[2] == 6 ) {
      
      output = Mosaic;
    }
    else if(aCommand.reply[2] == 7 ) {
  
      output = Slim;
    }
    else if(aCommand.reply[2] == 8 ) {
      
      output = Stretch;
    }
    else {
      fputs("13) Unknown answer.(CAM_PictureEffectModeInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
  
  return output;
}

/*
 * CAM_DigitalEffectModeInq:- Checks current Digital effect
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Digital effect currently being used
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_DigitalEffectModeInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x64;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else {

    if(aCommand.reply[2] == 0) {

      output = OFF;
    } 
    
    else if(aCommand.reply[2] == 1 ) {
      
      output = Still;
    }
    else if(aCommand.reply[2] == 2 ) {
      
      output = Flash;
    }
    else if(aCommand.reply[2] == 3 ) {
      
      output = Lumi;
    }
    else if(aCommand.reply[2] == 4 ) {
    
      output = Trail;
    }
    else {
      fputs("13) Unknown answer.(CAM_DigitalEffectModeInq)\n",logfile);
      fflush(logfile);
      output = -13;
    }
  }
  
  return output;
}

/*
 * CAM_DigitalEffectLevelInq:- Checks current Digital effect level
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Digital effect level currently being used
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_DigitalEffectLevelInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x65;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  output = aCommand.reply[2];

  return output;
}

/*
 * CAM_MemoryInq:- Checks last memory used
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- las memory used
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_MemoryInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int output;
  
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x04;
  aCommand.send[3]=0x3F;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  output = aCommand.reply[2];

  return output;
}

/*
 * DatascreenInq:- Data screen status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN: ON:- Data screen is ON
 *          OFF:- Data screen is OFF
 *          negative number if there is an error
 *
 */
int CommandLevel::DatascreenInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x06;
  aCommand.send[3]=0x06;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x02) return ON;

  else if(aCommand.reply[2] == 0x03) return OFF;

  else {

    fputs("13) Unknown answer.(DatascreenInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * PanTiltModeInq:- Checks Pan/Tilt status
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- Pan/Tilt status
 *          negative number if there is an error
 *          output[0]: Status 0
 *          output[1]; Status 1
 *
 */
int CommandLevel::PanTiltModeInq(int cam, int *output) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x06;
  aCommand.send[3]=0x10;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  output[0] = aCommand.reply[2];
  output[1] = aCommand.reply[3];

  return 0;
}

/*
 * PanTiltMaxSpeedInq:- Checks Pan/Tilt maximum speed
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- [0] Pan speed
 *          [1] Tilt speed
 *          negative number if there is an error
 *
 */
int CommandLevel::PanTiltMaxSpeedInq(int cam, int *output) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
 
  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x06;
  aCommand.send[3]=0x11;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  output[0] = aCommand.reply[2];
  output[1] = aCommand.reply[3];
  return 0;
}

/*
 * CAM_PTZInq:- Checks Pan/Tilt and zoom positions
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- pan  - Pan position
 *          tilt - Tilt position
 *          zoom - Zoom value
 *          rt   - negative number if there is an error
 *
 */

int CommandLevel::CAM_PTZInq( int cam, float *myPan, float *myTilt, int *myZoom ){

  int retval = 0;
  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
  
  command *ptReply;
  command *zReply;

  /*Add header and end byte*/
  //Format for P/T request
  IHEADER;
  aCommand.send[2]= 0x06;
  aCommand.send[3]=0x12;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  //Format for Zoom request
  eTestCommand.send[0] = aCommand.send[0];
  eTestCommand.send[1] = aCommand.send[1];
  eTestCommand.send[2] = 0x04;
  eTestCommand.send[3] = 0x47;
  eTestCommand.send[4] = 0xFF;
  eTestCommand.sendLength = 5;
  eTestCommand.end = 0xFF;
  
  //Send both inquiries
  SendToCam(fd, &aCommand);
  SendToCam(fd, &eTestCommand);
  
  /*printf( "Sent (aCommand): %x %x %x %x | %x (%s @ %d)\n", 
    aCommand.send[0], aCommand.send[1], aCommand.send[2], aCommand.send[3], 
    aCommand.send[4], __FILE__, __LINE__);
    
  printf( "Sent (eTestCommand): %x %x %x %x | %x (%s @ %d)\n", 
    eTestCommand.send[0], eTestCommand.send[1], eTestCommand.send[2], eTestCommand.send[3],
    eTestCommand.send[4], __FILE__, __LINE__);
  */
  //Get the responses
  ReadFromCam(fd, &aCommand, 1);
  ReadFromCam(fd, &eTestCommand, 1);

 /* printf( "Received (aCommand): len: %d  vals: %x %x %x %x | %x %x %x %x | %x %x %x %x"
          " | %x %x %x %x(%s @ %d)\n", 
    aCommand.replyLength, 
    aCommand.reply[0], aCommand.reply[1], aCommand.reply[2], aCommand.reply[3], 
    aCommand.reply[4], aCommand.reply[5], aCommand.reply[6], aCommand.reply[7], 
    aCommand.reply[8], aCommand.reply[9], aCommand.reply[10],aCommand.reply[11],
    aCommand.reply[12], aCommand.reply[13], aCommand.reply[14],aCommand.reply[15],
    __FILE__, __LINE__);
    
  printf( "Received (eTestCommand): len: %d  vals: %x %x %x %x | %x %x %x %x | %x %x %x %x"
          " | %x %x %x %x(%s @ %d)\n", 
    eTestCommand.replyLength, 
    eTestCommand.reply[0], eTestCommand.reply[1], eTestCommand.reply[2], eTestCommand.reply[3], 
    eTestCommand.reply[4], eTestCommand.reply[5], eTestCommand.reply[6], eTestCommand.reply[7], 
    eTestCommand.reply[8], eTestCommand.reply[9], eTestCommand.reply[10],eTestCommand.reply[11],
    eTestCommand.reply[12], eTestCommand.reply[13], eTestCommand.reply[14],eTestCommand.reply[15],
    __FILE__, __LINE__);*/
    
    /* If there is a syntax error. */
  if(IsErrorSony(&aCommand) == 1) {
    
    fputs("12) Camera returned an error (aCommand syntax Inquiry):\n",logfile);
    WhichErrorSony(&aCommand);
    PrintError(&aCommand);
    fflush(logfile);
    retval = -12;
  }
  
  /* Check that previous instruction was not done before inquiy. */
  else if(IsCplSony(&aCommand) == 1) {
    
    /* Read reply. */
    ReadFromCam(fd,&aCommand, 1);
    
    /* If there is a syntax error. */
    if(IsErrorSony(&aCommand) == 1) {
      
      fputs("12) Camera returned an error (aCommand not done Inquiry):\n",logfile);
      WhichErrorSony(&aCommand);
      PrintError(&aCommand);
      fflush(logfile);
      retval = -12;
    }
  }

    /* If there is a syntax error. */
  if(IsErrorSony(&eTestCommand) == 1) {
    
    fputs("12) Camera returned an error (eTestCommand syntax Inquiry):\n",logfile);
    WhichErrorSony(&eTestCommand);
    PrintError(&eTestCommand);
    fflush(logfile);
    retval = -12;
  }
  
  /* Check that previous instruction was not done before inquiy. */
  else if(IsCplSony(&eTestCommand) == 1) {
    
    /* Read reply. */
    ReadFromCam(fd,&eTestCommand, 1);
    
    /* If there is a syntax error. */
    if(IsErrorSony(&eTestCommand) == 1) {
      
      fputs("12) Camera returned an error (eTestCommand not done Inquiry):\n",logfile);
      WhichErrorSony(&eTestCommand);
      PrintError(&eTestCommand);
      fflush(logfile);
      retval = -12;
    }
  }  
    
  //See if it is the 
  if( aCommand.replyLength == 7 && eTestCommand.replyLength == 11 )
  {
    zReply = &aCommand;
    ptReply = &eTestCommand;
  }
  else if( aCommand.replyLength == 11 && eTestCommand.replyLength == 7 )
  {
    ptReply = &aCommand;
    zReply = &eTestCommand;
  }
  else
  {
    //printf( "Bad reply sizes (%s @ %d)\n", __FILE__, __LINE__ );  
    retval = -1;
    ptReply = &aCommand;  
    zReply = &eTestCommand;
  }
  
  *myPan = (float)((int)HexConvert(ptReply,2)*CAM_PANTILT_DEGREES_PER_UNIT);
  *myTilt = (float)((int)HexConvert(ptReply,6)*CAM_PANTILT_DEGREES_PER_UNIT);
  
  *myZoom =  HexConvert(zReply,2);

  if( (0<= *myZoom) && (28672 >= *myZoom) ) {
    retval = 0;
  }

  else {
    fputs("13) Unknown answer.(CAM_ZoomPosInq)\n",logfile);
    fflush(logfile);
    retval = -13;
  }

  
    
  return retval;
}

/*
 * PanTiltPosInq:- Checks Pan/Tilt position
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- [0] Pan position
 *          [1] Tilt position
 *          negative number if there is an error
 *
 */
int CommandLevel::PanTiltPositionInq(int cam, float *output) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int pan;
  int tilt;

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x06;
  aCommand.send[3]=0x12;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;
  
  pan = HexConvert(&aCommand,2);
  tilt = HexConvert(&aCommand,6);
    
  output[0] = pan*CAM_PANTILT_DEGREES_PER_UNIT;
  output[1] = tilt*CAM_PANTILT_DEGREES_PER_UNIT;
  
  return 0;
}

/*
 * VideoSystemInq:- Checks if the video system
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN: NTSC:- System is NTSC
 *         PAL:- System is PAL
 *         negative number if there is an error
 *
 */
int CommandLevel::VideoSystemInq(int cam) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x06;
  aCommand.send[3]=0x23;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  else if(aCommand.reply[2] == 0x00) return NTSC;

  else if(aCommand.reply[2] == 0x01) return PAL;

  else {

    fputs("13) Unknown answer.(VideoSystemInq)\n",logfile);
    fflush(logfile);
    return -13;
  }
}

/*
 * CAM_DeviceTypeVersionInq:- Checks vender ID, model ID ROM
 *                            version and number of sockets
 *
 * PARAMS: cam:- Camera number
 *
 * RETURN:- [0] Vender ID
 *          [1] Model ID
 *          [2] ROM version
 *          [3] Number of sockets
 *
 *          negative number if there is an error
 *
 */
int CommandLevel::CAM_DeviceTypeVersionInq(int cam, int *output) {

  if( aFlag.sony_canon != 1) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A CANON CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int vender = 0;
  int model = 0;
  int rom = 0;

  /*Add header and end byte*/
  IHEADER;
  aCommand.send[2]= 0x00;
  aCommand.send[3]=0x02;
  aCommand.send[4]=0xFF;
  aCommand.sendLength = 5;
  
  if((error = Inquiry(fd, &aCommand)) < 0) return error;

  
  vender |= aCommand.reply[2];
  
  vender = vender << 4;
  
  vender |= aCommand.reply[3];

  output[0] = vender;
  
  model |= aCommand.reply[4];

  model = model << 4;

  model |= aCommand.reply[5];
  
  output[1] = model;

  rom |= aCommand.reply[6];

  rom = rom << 4;

  rom |= aCommand.reply[7];
  
  output[2] = rom;

  output[3] = aCommand.reply[8];

  return 0;
}
/******************************************************************************
 * CANON COMMAND LIST
 ******************************************************************************/
/*
 * PanSpeedAssignment:- Assigns a speed for PAN. This can be done while
 *                      PAN is running
 *
 * PARAMS: speed: The speed in deg/s
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanSpeedAssignment(float speed) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int theSpeed;
  
  if( (0.9<=speed) && (speed<=90) ) {
    
    theSpeed = (int)((float) speed / (float) 0.1125);

    /*Add header and end byte*/
    CCHEADER;

    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x50;  
    
    /* Add parameters */
    DecConvert(theSpeed);
    AsciiConvert();

    aCommand.send[5]= array[1];
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal speed parameter.(PanSpeedAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * TiltSpeedAssignment:- Assigns a speed for TILT. This can be done while
 *                      TILT is running
 *
 * PARAMS: speed: The speed in deg/s
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::TiltSpeedAssignment(float speed) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int theSpeed;

  if( (0.9<=speed) && (speed<=70) ) {
    
    theSpeed = (int)((float) speed / (float) 0.1125);

    /*Add header and end byte*/
    CCHEADER;

    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x51;  
    
    /* Add parameters */
    DecConvert(theSpeed);
    AsciiConvert();
    aCommand.send[5]= array[1];
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal speed parameter.(TiltSpeedAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * PanTiltStop:- Stops PAN and TILT running
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanTiltRunningStop() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;

  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x53;  
    
  /* Add parameters */
  
  aCommand.send[5]= 0x30;
    
  
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;

  return SendSynchCanon(fd, &aCommand);
}

/*
 * PanRightStart:- Start PANNING right
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanRightStart(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }
 
  /*Add header and end byte*/
  CCHEADER;

  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x53;  
    
  /* Add parameters */
  
  aCommand.send[5]= 0x31;
    
  
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;

  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * PanLeftStart:- Start PANNING left
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanLeftStart(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;

  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x53;  
    
  /* Add parameters */
  
  aCommand.send[5]= 0x32;
    
  
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;

  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * TiltUpStart:- Start TILTING up
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::TiltUpStart(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;

  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x53;  
    
  /* Add parameters */
  
  aCommand.send[5]= 0x33;
    
  
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;

  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * TiltDownStart:- Start TILTING down
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::TiltDownStart(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;

  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x53;  
    
  /* Add parameters */
  
  aCommand.send[5]= 0x34;
    
  
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;

  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * HomePosition:- Go to HOME position
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::HomePosition(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x57;
  
  /* Add End mark */
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * PedestalInitialize1:- Initialize pedestal, run to Home position.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PedestalInitialize1(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x58;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * PedestalInitialize2:- Initialize pedestal, go to former position.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PedestalInitialize2(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x58;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * PanTiltStop:- stop pan and tilt.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanTiltMotionStop() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x60;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0x30;

  /* Add End mark */
  aCommand.send[7]= 0xEF;
  aCommand.sendLength = 8;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * PanTiltStartStop:- Start or stop pan and tilt.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanTiltStartStop(int panDirectionStartStop, int tiltDirectionStartStop, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  if( (panDirectionStartStop == 0) || 
      (panDirectionStartStop == 1) || 
      (panDirectionStartStop == 2) || 
      (tiltDirectionStartStop == 0) || 
      (tiltDirectionStartStop == 1) || 
      (tiltDirectionStartStop == 2) ) { 
  
    /*Add header and end byte*/
    CCHEADER;
  
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x60;
  
    /* Add parameter */


    aCommand.send[5]= 0x30 | panDirectionStartStop;

    aCommand.send[6]= 0x30 | tiltDirectionStartStop;

    /* Add End mark */
    aCommand.send[7]= 0xEF;
    aCommand.sendLength = 8;
  }
  
  else {
    fputs("14) Illegal parameter.(PanTiltStartStop)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * PanTiltAngleAssignment:- Move camera to a certain position.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanTiltAngleAssignment(int panAngle, int tiltAngle, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int pan = 0;
  int tilt = 0;

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  if( ((-100<= panAngle ) && (panAngle <= 100)) && 
      ((-30<= tiltAngle) && (tiltAngle <= 90)) ) { 
    
    pan = (int)(panAngle / 0.1125) + 32768;
    tilt = (int)(tiltAngle / 0.1125) + 32768;
   
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x62;
    
    /* Add parameter */
    DecConvert(pan);
    AsciiConvert();
    aCommand.send[5]= array[0];
    aCommand.send[6]= array[1];
    aCommand.send[7]= array[2];
    aCommand.send[8]= array[3];
    ResetArray();
    
    DecConvert(tilt);
    AsciiConvert();
    aCommand.send[9]= array[0];
    aCommand.send[10]= array[1];
    aCommand.send[11]= array[2];
    aCommand.send[12]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[13]= 0xEF;
    aCommand.sendLength = 14;
  }
      
  else {
    fputs("14) Illegal pan or tilt parameter.(PanTiltAngleAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * PanMovableRangeAssignment:- Assign range of pan movement of the camera.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PanMovableRangeAssignment(int minimumAngle, int maximumAngle) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int min = 0;
  int max = 0;

  if( ((-100<= minimumAngle ) && (minimumAngle <= 100)) && 
      ((-100<= maximumAngle) && (maximumAngle <= 100)) &&
      (minimumAngle < maximumAngle) ) { 
    
    min = (int)(minimumAngle / 0.1125) + 32768;
    max = (int)(maximumAngle / 0.1125) + 32768;
   
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x64;
    aCommand.send[5]= 0x30;
    /* Add parameter */
    DecConvert(min);
    AsciiConvert();
    aCommand.send[6]= array[0];
    aCommand.send[7]= array[1];
    aCommand.send[8]= array[2];
    aCommand.send[9]= array[3];
    ResetArray();
    
    DecConvert(max);
    AsciiConvert();
    aCommand.send[10]= array[0];
    aCommand.send[11]= array[1];
    aCommand.send[12]= array[2];
    aCommand.send[13]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[14]= 0xEF;
    aCommand.sendLength = 15;
  }
      
  else {
    fputs("14) Illegal pan parameter.(PanMovableRangeAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * TiltMovableRangeAssignment:- Assign range of tilt movement of the camera.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::TiltMovableRangeAssignment(int minimumAngle, int maximumAngle) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int min = 0;
  int max = 0;

  if( ((-30<= minimumAngle ) && (minimumAngle <= 90)) && 
      ((-30<= maximumAngle) && (maximumAngle <= 90)) ) { 
    
    min = (int)(minimumAngle / 0.1125) + 32768;
    max = (int)(maximumAngle / 0.1125) + 32768;
   
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x64;
    aCommand.send[5]= 0x31;
    /* Add parameter */
    DecConvert(min);
    AsciiConvert();
    aCommand.send[6]= array[0];
    aCommand.send[7]= array[1];
    aCommand.send[8]= array[2];
    aCommand.send[9]= array[3];
    ResetArray();
    
    DecConvert(max);
    AsciiConvert();
    aCommand.send[10]= array[0];
    aCommand.send[11]= array[1];
    aCommand.send[12]= array[2];
    aCommand.send[13]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[14]= 0xEF;
    aCommand.sendLength = 15;
  }
      
  else {
    fputs("14) Illegal tilt parameter.(tiltMovableRangeAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * CameraOFF:- Turns the camera off.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::CameraOFF(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA0;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * CameraON:- Turns the camera on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::CameraON(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA0;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * FocusAuto:- Sets auto focus on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FocusAuto() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA1;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * FocusManual:- Sets manual focus on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FocusManual() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA1;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * FocusNear:- Sets focus point near.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FocusNear(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA1;
  
  /* Add parameter */
  aCommand.send[5]= 0x32;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * FocusFar:- Sets focus point far.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FocusFar(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA1;
  
  /* Add parameter */
  aCommand.send[5]= 0x33;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * FocusPositionAssignment:- Sets focus to a certain point.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FocusPositionAssignment(int focusPoint, int sync) { 
       
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
    
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB0;
    
  /* Add parameter */
  DecConvert(focusPoint);
  AsciiConvert();
  aCommand.send[5]= array[0];
  aCommand.send[6]= array[1];
  aCommand.send[7]= array[2];
  aCommand.send[8]= array[3];
  ResetArray();
    
  /* Add End mark */
  aCommand.send[9]= 0xEF;
  aCommand.sendLength = 10;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * OnePushAF:- One push auto focus.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::OnePushAF(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB1;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomStop:- Stop zooming.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomStop(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA2;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomWide:- zoom wide.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomWide(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA2;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomTele:- zoom tele.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomTele(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA2;
  
  /* Add parameter */
  aCommand.send[5]= 0x32;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomHiWide:- zoom wide at hi speed.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomHiWide(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA2;
  
  /* Add parameter */
  aCommand.send[5]= 0x33;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
 
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomHiTele:- zoom tele at hi speed.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomHiTele(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA2;
  
  /* Add parameter */
  aCommand.send[5]= 0x34;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomPosition1Assignment:- Assign a zoom position.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomPosition1Assignment(int zoom, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  if( ((0<= zoom ) && (zoom <= 128)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xA3;

    /* Add parameter */
    DecConvert(zoom);
    AsciiConvert();
    aCommand.send[5]= array[2];
    aCommand.send[6]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[7]= 0xEF;
    aCommand.sendLength = 8;
  }
      
  else {
    fputs("14) Illegal zoom parameter.(ZoomPosition1Assignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomPosition1Assignment:- Assign a zoom position.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomPosition2Assignment(int zoom, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
    
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB3;

  /* Add parameter */
  DecConvert(zoom);
  AsciiConvert();
  aCommand.send[5]= array[0];
  aCommand.send[6]= array[1];
  aCommand.send[7]= array[2];
  aCommand.send[8]= array[3];
  ResetArray();
    
  /* Add End mark */
  aCommand.send[9]= 0xEF;
  aCommand.sendLength = 10;
   
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ZoomSpeedAssignment:- Assign a zoom speed.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ZoomSpeedAssignment(int speed) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= speed ) && (speed <= 7)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xB4;

    /* Add parameter */
    DecConvert(speed);
    AsciiConvert();
    aCommand.send[5]= 0x31;
    aCommand.send[6]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[7]= 0xEF;
    aCommand.sendLength = 8;
  }
  
  else {
    fputs("14) Illegal speed parameter.(ZoomSpeedAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * BackLightCompensationOFF:- Turn backlight OFF.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::BackLightCompensationOFF(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * BackLightCompensationON:- Turn backlight ON.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::BackLightCompensationON(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ExposureAuto:- Set auto exposure on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ExposureAuto() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  
  /* Add parameter */
  aCommand.send[5]= 0x32;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * ExposureManual:- Set exposure manual on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ExposureManual() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  
  /* Add parameter */
  aCommand.send[5]= 0x33;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * AELockOFF:- Auto exposure lock off.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::AELockOFF() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  
  /* Add parameter */
  aCommand.send[5]= 0x34;
  aCommand.send[6]= 0x30;
  /* Add End mark */
  aCommand.send[7]= 0xEF;
  aCommand.sendLength = 8;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * AELockON:- Auto exposure lock on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::AELockON() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  
  /* Add parameter */
  aCommand.send[5]= 0x34;
  aCommand.send[6]= 0x31;
  /* Add End mark */
  aCommand.send[7]= 0xEF;
  aCommand.sendLength = 8;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * ShutterSpeedProgram:- Chagne shutter speed to program mode.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ShutterSpeedProgram() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA8;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * ShutterSpeed1_60:- Chagne shutter speed to 1/60 (PAL 1/50).
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ShutterSpeed1_60(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA8;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ShutterSpeed1_100:- Chagne shutter speed to 1/100 (PAL 1/120).
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ShutterSpeed1_100(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA8;
  
  /* Add parameter */
  aCommand.send[5]= 0x32;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * ShutterSpeedAssignment:- Assign a shutter speed.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ShutterSpeedAssignment(int speed, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= speed ) && (speed <= 25)) ) { 
    
    if(sync == 1){
      CommandTerminationNotificationSetting(1);
    }

    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xA5;
    aCommand.send[5]= 0x35;
    /* Add parameter */
    DecConvert(speed);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal speed parameter.(ShutterSpeedAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * AGCGainAssignment:- Assign automatic gain control gain.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::AGCGainAssignment(int gain) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= gain ) && (gain <= 255)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xA5;
    aCommand.send[5]= 0x37;
    /* Add parameter */
    DecConvert(gain);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal gain parameter.(AGCGainAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * IrisAssignment:- Assign Iris size.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::IrisAssignment(int size, int sync) {
 
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if( ((2<= size) && (size <= 16)) ) { 
    
    if(sync == 1){
      CommandTerminationNotificationSetting(1);
    }

    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xA5;
    aCommand.send[5]= 0x39;
    /* Add parameter */
    DecConvert(size);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal iris parameter.(IrisAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * AETargetValueAssignment:- Assign target value for Aperture exposure brightness.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::AETargetValueAssignment(int bright, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((16<= bright) && (bright <= 255)) ) { 

   
    if(sync == 1){
      CommandTerminationNotificationSetting(1);
    }
 
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xA5;
    aCommand.send[5]= 0x3B;
    /* Add parameter */
    DecConvert(bright);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal brightness parameter.(AETargetValueAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * AutoWhiteBalanceNormal:- Set white balance to auto.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::AutoWhiteBalanceNormal() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA7;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * AutoWhiteBalanceLock:- Lock auto white balance.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::AutoWhiteBalanceLock() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA7;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * WhiteBalanceModeManual:- Set white balance to manual.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::WhiteBalanceManualMode(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA7;
  
  /* Add parameter */
  aCommand.send[5]= 0x32;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * WhiteBalanceValueAssignment:- Assign a white balance value.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::WhiteBalanceValueAssignment(int value) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= value) && (value <= 255)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0xA7;
    aCommand.send[5]= 0x34;
    /* Add parameter */
    DecConvert(value);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    /* Add End mark */
    aCommand.send[8]= 0xEF;
    aCommand.sendLength = 9;
  }
  
  else {
    fputs("14) Illegal value parameter.(WhiteBalanceValueAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * FadeNormal:- Set normal output of image.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FadeNormal(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA9;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * FadeWhite:- Fade in white slowly.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FadeWhite(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA9;
  
  /* Add parameter */
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * FadeHiSpeedWhite:- Fade in white fast.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FadeHiSpeedWhite(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA9;
  
  /* Add parameter */
  aCommand.send[5]= 0x32;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * FadeHiSpeedBlack:- Fade in black fast.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::FadeHiSpeedBlack(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA9;
  
  /* Add parameter */
  aCommand.send[5]= 0x33;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * CameraReset:- Reset camera.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::CameraReset(int sync) {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }


  if(sync == 1){
    CommandTerminationNotificationSetting(1);
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xAA;
  
  /* Add End mark */
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * RemoteControlON:- Turn the remote control ON.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::RemoteControlON() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x80;
  aCommand.send[5]= 0x30;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * RemoteControlOFF:- Turn the remote control OFF.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::RemoteControlOFF() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x80;
  aCommand.send[5]= 0x31;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * PresetSet:- Set memory.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PresetSet(int value) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((1<= value) && (value <= 9)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x89;
    aCommand.send[5]= (value+48);
    
    /* Add End mark */
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }
  
  else {
    fputs("14) Illegal value parameter.(PresetSet)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * PresetMove:- Move to set memory.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PresetMove(int value, int sync) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((1<= value) && (value <= 9)) ) { 
    

    if(sync == 1){
      CommandTerminationNotificationSetting(1);
    }

    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x8A;
    aCommand.send[5]= (value+48);
   
    /* Add End mark */
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }
  
  else {
    fputs("14) Illegal value parameter.(PresetMove)\n",logfile);
    fflush(logfile);
    return -14;
  }
  if( (out = SendSynchCanon(fd, &aCommand)) < 0 ) return out;

  if(sync == 1){
    WaitForNotification(fd);
    CommandTerminationNotificationSetting(0);
  }
  return 0;
}

/*
 * RemoteControllerThroughSetting:- Admit remote controller notification.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::RemoteControllerThroughSetting(int value) {
  
  if( aFlag.sony_canon != 0) {
    
    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
  
  if( ((0== value) || (value == 1)) ) { 
   
    /*Add header and end byte*/
    CCHEADER;
   
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x8D;
    aCommand.send[5]= (value+48);
    /* Add End mark */
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }
 
  else {
    fputs("14) Illegal value parameter.(RemoteControllerThroughSetting)\n",logfile);
    fflush(logfile);
    return -14;
  }
 
  return SendSynchCanon(fd, &aCommand);
}

/*
 * LedNormalDisplay:- LED lighting mode to normal.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::LedNormalDisplay() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
 
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x8E;
  aCommand.send[5]= 0x30;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * LedForcedControl:- Set LED control.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::LedForcedControl(int value) {
  
  if( aFlag.sony_canon != 0) {
    
    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
  
  if( ((1<= value) && (value <= 4)) ) { 
   
    /*Add header and end byte*/
    CCHEADER;
   
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x8E;
    aCommand.send[5]= (value+48);
    /* Add End mark */
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }
 
  else {
    fputs("14) Illegal value parameter.(LedForcedControl)\n",logfile);
    fflush(logfile);
    return -14;
  }
 
  return SendSynchCanon(fd, &aCommand);
}

/*
 * cascadeOFF:- Set cascase off.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::cascadeOFF() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
 
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x8F;
  aCommand.send[5]= 0x30;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * cascadeON:- Set cascase on.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::cascadeON() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
 
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x8F;
  aCommand.send[5]= 0x31;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * HostControlMode:- Give control to the host.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::HostControlMode() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
 
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x90;
  aCommand.send[5]= 0x30;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * LocalControlMode:- Give control to the remote.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::LocalControlMode() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
 
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x90;
  aCommand.send[5]= 0x31;
  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * ScreenControl:- Control the screen display.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::ScreenControl(int value) {
  
  if( aFlag.sony_canon != 0) {
    
    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
  
  if( ((0<= value) && (value <= 9)) ) { 
   
    /*Add header and end byte*/
    CCHEADER;
   
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x91;
    aCommand.send[5]= 0x30;
    aCommand.send[6]= (value+48);
    /* Add End mark */
    aCommand.send[7]= 0xEF;
    aCommand.sendLength = 8;
  }
 
  else {
    fputs("14) Illegal value parameter.(ScreenControl)\n",logfile);
    fflush(logfile);
    return -14;
  }
 
  return SendSynchCanon(fd, &aCommand);
}

/*
 * DisplayCharacterDataAssignment:- display a character on screen.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::DisplayCharacterDataAssignment(int horizontal, int vertical, int character) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= horizontal) && (horizontal <= 23)) &&
      ((0<= vertical) && (vertical <= 10)) &&
      ((32<= character) && (horizontal <= 95)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x91;
    aCommand.send[5]= 0x31;

    /* Add parameter */
    DecConvert(horizontal);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    DecConvert(vertical);
    AsciiConvert();
    aCommand.send[8]= array[3];
    ResetArray();

    DecConvert(character);
    AsciiConvert();
    aCommand.send[9]= array[2];
    aCommand.send[10]= array[3];
    ResetArray();
    /* Add End mark */
    aCommand.send[11]= 0xEF;
    aCommand.sendLength = 12;
  }
  
  else {
    fputs("14) Illegal parameter.(DisplayCharacterDataAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * DisplayDateAssignment:- display the date on screen.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::DisplayDateAssignment(int year, int month, int day) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= year) && (year <= 0x99)) &&
      ((0<= month) && (month <= 0x12)) &&
      ((0<= day) && (day <= 0x31)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x91;
    aCommand.send[5]= 0x33;

    /* Add parameter */
    DecConvert(year);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();

    DecConvert(month);
    AsciiConvert();
    aCommand.send[8]= array[2];
    aCommand.send[9]= array[3];
    ResetArray();

    DecConvert(day);
    AsciiConvert();
    aCommand.send[10]= array[2];
    aCommand.send[11]= array[3];
    ResetArray();
    /* Add End mark */
    aCommand.send[12]= 0xEF;
    aCommand.sendLength = 13;
  }
  
  else {
    fputs("14) Illegal parameter.(DisplayDateAssignment)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * DisplayTimeSetting:- display the time on screen.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::DisplayTimeSetting(int hour, int minute, int second) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= hour) && (hour <= 0x23)) &&
      ((0<= minute) && (minute <= 0x59)) &&
      ((0<= second) && (second <= 0x59)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x91;
    aCommand.send[5]= 0x35;

    array[2] = (hour & 0xF0);
    array[2] = array[2] >> 4; 
    array[2] = array[2] | 0x30;
    array[3] = (hour & 0x0F); 
    array[3] = array[3] | 0x30;
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    array[2] = (minute & 0xF0);
    array[2] = array[2] >> 4; 
    array[2] = array[2] | 0x30;
    array[3] = (minute & 0x0F); 
    array[3] = array[3] | 0x30;
    aCommand.send[8]= array[2];
    aCommand.send[9]= array[3];
    ResetArray();

    array[2] = (second & 0xF0);
    array[2] = array[2] >> 4; 
    array[2] = array[2] | 0x30;
    array[3] = (second & 0x0F); 
    array[3] = array[3] | 0x30;
    aCommand.send[10]= array[2];
    aCommand.send[11]= array[3];
    ResetArray();
    /* Add End mark */
    aCommand.send[12]= 0xEF;
    aCommand.sendLength = 13;
  }
  
  else {
    fputs("14) Illegal parameter.(DisplayTimeSetting)\n",logfile);
    fflush(logfile);
    return -14;
  }
  return SendSynchCanon(fd, &aCommand);
}

/*
 * DefaultSetting:- Set factory default.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::DefaultSetting() {
  
  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x93;
  
  /* Add parameter */
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  return SendSynchCanon(fd, &aCommand);
}

/*
 * CommandTerminationNotificationSetting:- Send termination notice 
 *                                         for type 2 instructions.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::CommandTerminationNotificationSetting(int value) {
  
  if( aFlag.sony_canon != 0) {
    
    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
  
  if( ((1== value) || (value == 0)) ) { 
   
    /*Add header and end byte*/
    CCHEADER;
   
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x94;
    aCommand.send[5]= 0x30 | value;
    /* Add End mark */
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }
 
  else {
    fputs("14) Illegal value parameter.(CommandTerminationNotificationSetting)\n",logfile);
    fflush(logfile);
    return -14;
  }
 
  return SendSynchCanon(fd, &aCommand);
}

/*
 * GlobalNotificationSetting:- Set global notification in cascading mode.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::GlobalNotificationSetting(int value) {
  
  if( aFlag.sony_canon != 0) {
    
    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }
  
  if( ((1== value) || (value == 0)) ) { 
   
    /*Add header and end byte*/
    CCHEADER;
   
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x95;
    aCommand.send[5]= 0x30 | value;
    /* Add End mark */
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }
 
  else {
    fputs("14) Illegal value parameter.(GlobalNotificationSetting)\n",logfile);
    fflush(logfile);
    return -14;
  }
 
  return SendSynchCanon(fd, &aCommand);
}


/******************************************************************************
 * CANON INQUIRY COMMANDS
 ******************************************************************************/
/*
 * PanSpeedRequest:- Gets the PAN speed
 *
 *
 * RETURN:- PAN speed.
 *
 */
int CommandLevel::PanSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x52;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
  
    return (int) ceil((getValue(5,3) * 0.1125));
  }
}

/*
 * TiltSpeedRequest:- Gets the TILT speed
 *
 *
 * RETURN:- TILT speed.
 *
 */
int CommandLevel::TiltSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x52;
  aCommand.send[5]= 0x31;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (int) ceil((getValue(5,3) * 0.1125));
  }
}

/*
 * PanSlowestSpeedRequest:- Gets the slowest PAN speed
 *
 *
 * RETURN:- PAN speed.
 *
 */
int CommandLevel::PanSlowestSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x59;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (int) ceil((getValue(5,3) * 0.1125));
  }
}

/*
 * PanFastestSpeedRequest:- Gets the fastest PAN speed
 *
 *
 * RETURN:- PAN speed.
 *
 */
int CommandLevel::PanFastestSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x59;
  aCommand.send[5]= 0x31;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (int) ceil((getValue(5,3) * 0.1125));
  }
}

/*
 * TiltSlowestSpeedRequest:- Gets the slowest TILT speed
 *
 *
 * RETURN:- TILT speed.
 *
 */
int CommandLevel::TiltSlowestSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x59;
  aCommand.send[5]= 0x32;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (int) ceil((getValue(5,3) * 0.1125));
  }
}

/*
 * TiltFastestSpeedRequest:- Gets the fastest TILT speed
 *
 *
 * RETURN:- TILT speed.
 *
 */
int CommandLevel::TiltFastestSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x59;
  aCommand.send[5]= 0x33;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (int) ceil((getValue(5,3) * 0.1125));
  }
}

/*
 * PanAnglePulseRatioRequest:- Gets the PAN angle coefficient.
 *
 *
 * RETURN:- Pan angle coefficient.
 *
 */
float CommandLevel::PanAnglePulseRatioRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x5B;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (float) getValue(5,4) / (float) 100000;
  }
}

/*
 * TiltAnglePulseRatioRequest:- Gets the TILT angle coefficient
 *
 *
 * RETURN:- Tilt angle coefficient.
 *
 */
float CommandLevel::TiltAnglePulseRatioRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x5B;
  aCommand.send[5]= 0x31;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  else {
    return (float) getValue(5,4) / (float) 100000;
  }
}

/*
 * PanMinimumAngleRequest:- Gets the minimum range of panning possible
 *
 *
 * RETURN:- pan angle
 *
 */
int CommandLevel::PanMinimumAngleRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x5C;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return (int)((getValue(5,4) - 32768) * 0.1125); 
}

/*
 * PanMaximumAngleRequest:- Gets the maximum range of panning possible
 *
 *
 * RETURN:- pan angle
 *
 */
int CommandLevel::PanMaximumAngleRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x5C;
  aCommand.send[5]= 0x31;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return (int)((getValue(5,4) - 32768) * 0.1125); 
}

/*
 * TiltMinimumAngleRequest:- Gets the minimum range of tiltinh possible
 *
 *
 * RETURN:- tilt angle
 *
 */
int CommandLevel::TiltMinimumAngleRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x5C;
  aCommand.send[5]= 0x32;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return (int)((getValue(5,4) - 32768) * 0.1125);
}

/*
 * TiltMaximumAngleRequest:- Gets the maximum range of tiltinh possible
 *
 *
 * RETURN:- tilt angle
 *
 */
int CommandLevel::TiltMaximumAngleRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x5C;
  aCommand.send[5]= 0x33;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return (int)ceil((getValue(5,4) - 32768) * 0.1125);
}

/*
 * PanTiltAngleRequest:- Gets the pan and tilt angle
 *
 *
 * RETURN:- pan and tilt angle
 *          theArray[0]: Pan
 *          theArray[1]: Tilt
 *
 */
int CommandLevel::PanTiltAngleRequest(float *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x63;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = ceil((getValue(5,4) - 32768) * 0.1125);
  theArray[1] = ceil((getValue(9,4) - 32768) * 0.1125);

  return 0;
}

/*
 * PanMovableRangeRequest:- Gets the pan movable range.
 *
 *
 * RETURN:- minimum and maximum range for pan.
 *          theArray[0]: Minimum
 *          theArray[1]: Maximum
 *
 */
int CommandLevel::PanMovableRangeRequest(int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x65;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = (int)ceil((getValue(5,4) - 32768) * 0.1125);
  theArray[1] = (int)floor((getValue(9,4) - 32768) * 0.1125);

  return 0;
}

/*
 * TiltMovableRangeRequest:- Gets the tilt movable range.
 *
 *
 * RETURN:- minimum and maximum range for tilt.
 *          theArray[0]: Minimum
 *          theArray[1]: Maximum
 *
 */
int CommandLevel::TiltMovableRangeRequest(int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x65;
  aCommand.send[5]= 0x31;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = (int)ceil((getValue(5,4) - 32768) * 0.1125);
  theArray[1] = (int)floor((getValue(9,4) - 32768) * 0.1125);

  return 0;
}

/*
 * FocusPositionRequest:- Gets the current focus position
 *
 *
 * RETURN:- focus position
 *
 */
int CommandLevel::FocusPositionRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB1;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,4);
}

/*
 * FocusRangeRequest:- Gets the focus range.
 *
 *
 * RETURN:- minimum and maximum range.
 *          theArray[0]: Minimum
 *          theArray[1]: Maximum
 *
 */
int CommandLevel::FocusRangeRequest(int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB1;
  aCommand.send[5]= 0x32;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = getValue(5,4);
  theArray[1] = getValue(9,4);

  return 0;
}

/*
 * ZoomPosition1Request:- Gets the current zoom position
 *
 *
 * RETURN:- zoom position
 *
 */
int CommandLevel::ZoomPosition1Request() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA4;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * ZoomPosition2Request:- Gets the current zoom position
 *
 *
 * RETURN:- zoom position
 *
 */
int CommandLevel::ZoomPosition2Request() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB4;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,4);
}

/*
 * ZoomSpeedRequest:- Gets the current zoom speed
 *
 *
 * RETURN:- zoom speed
 *
 */
int CommandLevel::ZoomSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB4;
  aCommand.send[5]= 0x32;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,1);
}

/*
 * ZoomMaximumRequest:- Gets the maximum zoom possible
 *
 *
 * RETURN:- zoom position
 *
 */
int CommandLevel::ZoomMaximumRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xB4;
  aCommand.send[5]= 0x33;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,4);
}

/*
 * ShutterSpeedRequest:- Gets the current shutter speed
 *
 *
 * RETURN:- Shutter speed
 *
 */
int CommandLevel::ShutterSpeedRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  aCommand.send[5]= 0x36;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * AGCGainRequest:- Gets the current AGC gain
 *
 *
 * RETURN:- gain
 *
 */
int CommandLevel::AGCGainRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  aCommand.send[5]= 0x38;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * IrisRequest:- Gets the current iris size
 *
 *
 * RETURN:- iris size
 *
 */
int CommandLevel::IrisRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  aCommand.send[5]= 0x3A;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * AETargetValueRequest:- Gets the current brightness setup
 *
 *
 * RETURN:- brightness
 *
 */
int CommandLevel::AETargetValueRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA5;
  aCommand.send[5]= 0x3C;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * WhiteBalanceValueRequest:- Gets the current brightness setup
 *
 * RETURN:- value
 *
 */
int CommandLevel::WhiteBalanceValueRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xA7;
  aCommand.send[5]= 0x35;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * ZoomRatioRequest:- Gets the zoom ratio
 *
 * RETURN:- ratio
 *
 */
int CommandLevel::ZoomRatioRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xAB;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * PixelSizeRequest:- Gets the pixel size
 *
 * RETURN:- array[0] = numerator
 *          array[1] = denominator
 *
 */
int CommandLevel::PixelSizeRequest(int *array) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xAC;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  array[0] = aCommand.reply[5] - 48;
  array[1] = aCommand.reply[6] - 48;

  return 0;
}

/*
 * ProductVersionRequest:- Gets the version of the camera
 *
 * RETURN:- version
 *
 */
int CommandLevel::ProductVersionRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xBE;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * EEPROMVersionRequest:- Gets the EEPROM version of the camera
 *
 * RETURN:- version
 *
 */
int CommandLevel::EEPROMVersionRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0xBE;
  aCommand.send[5]= 0x31;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * OperationStatusRequest:- Return operation status
 *
 * RETURN:- status
 *
 */
int CommandLevel::OperationStatusRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x86;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
    
  return getValue(5,3);
}

/*
 * extendedOperationStatusRequest:- Return operation status
 *
 * RETURN:- Extended status
 *
 */
int CommandLevel::ExtendedOperationStatusRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x86;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,5);
}

/*
 * ProductNameRequest:- Return product name
 *
 * RETURN:- Product name
 *          theArray[0]: 1st character
 *          theArray[1]: 2nd character
 *          theArray[2]: 3rd character
 *          theArray[3]: 4th character
 *          theArray[4]: 5th character
 *
 */
int CommandLevel::ProductNameRequest(int * theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x87;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = aCommand.reply[5];
  theArray[1] = aCommand.reply[6];
  theArray[2] = aCommand.reply[7];
  theArray[3] = aCommand.reply[8];
  theArray[4] = aCommand.reply[9];
  return 0;
}

/*
 * RomVersionRequest:- Return ROM version
 *
 * RETURN:- ROM version
 *          theArray[0]: 1st character
 *          theArray[1]: 2nd character
 *          theArray[2]: 3rd character
 *          theArray[3]: 4th character
 *          theArray[4]: 5th character
 *
 */
int CommandLevel::RomVersionRequest(int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x88;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = aCommand.reply[5];
  theArray[1] = aCommand.reply[6];
  theArray[2] = aCommand.reply[7];
  theArray[3] = aCommand.reply[8];
  theArray[4] = aCommand.reply[9];
  return 0;
 
}

/*
 * PresetStatusRequest:- Return the status of memory
 *
 * RETURN:- Memory status
 *
 */
int CommandLevel::PresetStatusRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x8B;
  aCommand.send[5]= 0xEF;
  aCommand.sendLength = 6;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,2);
}

/*
 * ExtendedPresetStatusRequest:- Return the extended status of memory
 *
 * RETURN:- Extended memory status
 *
 */
int CommandLevel::ExtendedPresetStatusRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  /*Add header and end byte*/
  CCHEADER;
  
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x8B;
  aCommand.send[5]= 0x30;
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;

  return getValue(5,3);
}

/*
 * DisplayCharacterDataRequest:- Get a character on screen at a certain 
 *                               position.
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *          theArray[0]: Upper nibble
 *          theArray[1]: Lower nibble
 *
 */
int CommandLevel::DisplayCharacterDataRequest(int horizontal, int vertical, int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  if( ((0<= horizontal) && (horizontal <= 23)) &&
      ((0<= vertical) && (vertical <= 10)) ) { 
    
    /*Add header and end byte*/
    CCHEADER;
    
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x91;
    aCommand.send[5]= 0x32;

    /* Add parameter */
    DecConvert(horizontal);
    AsciiConvert();
    aCommand.send[6]= array[2];
    aCommand.send[7]= array[3];
    ResetArray();
    
    DecConvert(vertical);
    AsciiConvert();
    aCommand.send[8]= array[3];
    ResetArray();

    /* Add End mark */
    aCommand.send[9]= 0xEF;
    aCommand.sendLength = 10;
  }
  
  else {
    fputs("14) Illegal parameter.(DisplayCharacterDataRequest)\n",logfile);
    fflush(logfile);
    return -14;
  }
  
  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  theArray[0] = aCommand.reply[5];
  theArray[1] = aCommand.reply[6];

  return 0;
}

/*
 * DisplayDateRequest:- Get the date set
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *          theArray[0]: Year in hex
 *          theArray[1]: Month in hex
 *          theArray[2]: Day in hex
 *
 */
int CommandLevel::DisplayDateRequest(int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
    
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x91;
  aCommand.send[5]= 0x34;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;


  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;

  theArray[0] = aCommand.reply[5] - 48;
  theArray[0] = theArray[0] << 4;
  theArray[0] = theArray[0] | (aCommand.reply[6] - 48);

  theArray[1] = aCommand.reply[5] - 48;
  theArray[1] = theArray[1] << 4;
  theArray[1] = theArray[1] | (aCommand.reply[6] - 48);

  theArray[2] = aCommand.reply[5] - 48;
  theArray[2] = theArray[2] << 4;
  theArray[2] = theArray[2] | (aCommand.reply[6] - 48);

  return 0;
}

/*
 * DisplayTimeRequest:- Get the time set
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *          theArray[0]: Hour in hex
 *          theArray[1]: Minute in hex
 *          theArray[2]: second in hex
 *
 */
int CommandLevel::DisplayTimeRequest(int *theArray) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
    
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x91;
  aCommand.send[5]= 0x36;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;

  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;

  theArray[0] = aCommand.reply[5] - 48;
  theArray[0] = theArray[0] << 4;
  theArray[0] = theArray[0] | (aCommand.reply[6] - 48);

  theArray[1] = aCommand.reply[5] - 48;
  theArray[1] = theArray[1] << 4;
  theArray[1] = theArray[1] | (aCommand.reply[6] - 48);

  theArray[2] = aCommand.reply[5] - 48;
  theArray[2] = theArray[2] << 4;
  theArray[2] = theArray[2] | (aCommand.reply[6] - 48);
  return 0;
}

/*
 * TurningONTimeRequest:- Gets the amount of time the camera has been on
 *
 * RETURN:- time
 *
 */
int CommandLevel::TurningONTimeRequest(int part) {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  int error = 0;

  if( (part == 0) || (part == 1) ) {

    /*Add header and end byte*/
    CCHEADER;
  
    /* Add Command */
    aCommand.send[3]= 0x00;
    aCommand.send[4]= 0x92;
    aCommand.send[5]= 0x30 | part;
    aCommand.send[6]= 0xEF;
    aCommand.sendLength = 7;
  }

  else {
    fputs("14) Illegal parameter.(TurningONTimeRequest)\n",logfile);
    fflush(logfile);
    return -14;
  }

  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;
  
  return getValue(5,4);
}

/*
 * PedestalModelRequest:- Get model of the pedestal
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::PedestalModelRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
    
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x9A;
  aCommand.send[5]= 0x30;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;


  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;

  return getValue(5,1);
}

/*
 * CameraModelRequest:- Get model of the camera
 *
 * RETURN:- 0 if everything is fine
 *          negative number if there is an error
 *
 */
int CommandLevel::cameraModelRequest() {

  if( aFlag.sony_canon != 0) {

    fputs("18) THIS INSTRUCTION IS NOT ALLOWED FOR A SONY CAMERA\n",logfile);
    fflush(logfile);
    return -18;
  }

  /*Add header and end byte*/
  CCHEADER;
    
  /* Add Command */
  aCommand.send[3]= 0x00;
  aCommand.send[4]= 0x9A;
  aCommand.send[5]= 0x31;

  /* Add End mark */
  aCommand.send[6]= 0xEF;
  aCommand.sendLength = 7;


  if( (error = InquiryCanon(fd,&aCommand)) < 0) return error;

  return getValue(5,1);
}

/***********************************************************************
 * Internal functions
 ************************************************************************/

//Converts decimal to hex stored in an array.
void CommandLevel::DecConvert(int dec) {
  
  int quotient;
  int remainder;

  
  quotient = dec/16; /* Get result */
  remainder = dec%16; /* Get remainder */

  if(quotient != 0) {
      
    array[count] = remainder;
    count--;
    DecConvert(quotient);
  }  
  else array[count] = remainder;
}

//Reset global array.
void CommandLevel::ResetArray() {

  count = 3;
  int i;
  for(i=0; i<4;i++) {
    
    array[i] = 0x00;
  }
}

//Convert to hex numbers stored in an array.
int CommandLevel::HexConvert(command *hex, int firstpos) {

  int position = 0;

  if(hex->reply[firstpos] == 0x0F) {

    position += -4096;
  }
  else {

    position += (int)((hex->reply[firstpos])*pow(16,3));
  }

  position += (int)((hex->reply[firstpos+1])*pow(16,2));
  position += (int)((hex->reply[firstpos+2])*pow(16,1));
  position += (int)((hex->reply[firstpos+3])*pow(16,0));
  
  return position;
}

//Convert ascii into decimal.
void CommandLevel::AsciiConvert() {

  int i;
  for(i=0; i<4;i++) {
 
    if(array[i] >9) {

      array[i] = array[i] - 9 + 64;
    }
    else {
      
      array[i] = array[i] +48;
    }
  }
}

// Gets a decimal value from ascii.
int CommandLevel::getValue(int start, int param) {

  int i;
  int count = param;
  int position = 0;

  for(i=start; i<param+start;i++) {
 
    if(aCommand.reply[i] > 57) {
     
      aCommand.reply[i] = aCommand.reply[i] - 55;
     
      position += (int)((aCommand.reply[i])*pow(16,count-1));
     
      count--;
    }

    else {
     
      aCommand.reply[i] = aCommand.reply[i] - 48;
     
      position += (int)((aCommand.reply[i])*pow(16,count-1));
      
      count--;
    }
  }
 
  return position;
}
