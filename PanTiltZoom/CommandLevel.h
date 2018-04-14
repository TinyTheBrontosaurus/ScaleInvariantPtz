#ifndef _COMMANDLEVEL_H_
#define _COMMANDLEVEL_H_

/**********************************************************************
 * CONSTANTS
 *********************************************************************/
#include "structFlag.h"
//CONSTANTS
#define ON 1
#define OFF 0

/*ZOOM*/
#define Stop 2
#define TeleStd 3
#define WideStd 4
#define TeleVar 5
#define WideVar 6
#define Direct 7
#define DZoomOn 8
#define DZoomOff 9
#define Ox0 0 
#define Ox1 3693 
#define Ox2 6286
#define Ox3 9479
#define Ox4 11138
#define Ox5 12592
#define Ox6 13614
#define Ox7 14429
#define Ox8 15176
#define Ox9 15873
#define Dx1 16384
#define Dx2 20480
#define Dx3 24576
#define Dx4 28672

/*FOCUS*/
#define FarStd 10
#define NearStd 11
#define FarVar 12
#define NearVar 13
#define AutoFocus 14
#define ManualFocus 15
#define AutoManual 16
#define OnePushTrigger 17
#define Infinity 18
#define AFSensHigh 19
#define AFSensLow 20
#define NearLimit 21

/*White Balance*/
#define Auto 22
#define Indoor 23
#define Outdoor 24
#define OnePushWB 25
#define ATW 26
#define Manual 27

/*RGain and BGain*/
#define Reset 28
#define Up 29
#define Down 30

/*Aperture Exposure*/
#define FullAuto 31
#define ShutterPriority 32
#define IrisPriority 33
#define GainPriority 34
#define Bright 35
#define ShutterAuto 36
#define IrisAuto 37
#define GainAuto 38

/*Wide view*/
#define Cinema 39
#define Full 40

/*Picture Effect*/
#define Pastel 41
#define NegArt 42
#define Sepia 43
#define BandW 44
#define Solarize 45
#define Mosaic 46
#define Slim 47
#define Stretch 48

/*Digital Effect*/
#define Still 49
#define Flash 50
#define Lumi 51
#define Trail 52
#define EffectLevel 53

/*Memory*/
#define Set 54
#define Recall 55

/*Data screen*/
#define ONOFF 56

/*Pan and tilt*/
#define Left 57
#define Right 58
#define UpRight 59
#define UpLeft 60
#define DownRight 61
#define DownLeft 62
#define AbsolutePosition 63
#define RelativePosition 64
#define Home 65
#define CAM_PANTILT_DEGREES_PER_UNIT 0.07

/*Limit set*/
#define LimitSet 66
#define LimitClear 67

/*Auto focus sensitivity*/
#define HIGH 68
#define LOW 69

/*Video system*/
#define NTSC 70
#define PAL 71

class CommandLevel
{
  public:
  
  CommandLevel();
  /**********************************************************************
  * ABSTRACTION LEVEL INSTRUCTIONS
  *********************************************************************/
  // Initialize
  int Init(char *serial, int cameranum);
  int Init38400(char *serial, int cameranum);
  int getFd();
  void setFd( int newFd );
  
  //Resets the port to the original settings
  int ResetThePort();
  
  //Set everything to manual for the camera
  int SetManual(int cameranum);
  
  //Set everything to Auto for the camera
  int SetAuto(int cameranum);
  
  //Set camera to an abosulte position.
  int PanTiltAbsolutePosition(int cam, int sync, float pan, 
                          float tilt, float speedP, float speedT);
  
  //Set camera to a position relative to the current one.
  int PanTiltRelativePosition(int cam, int sync, float pan, 
                          float tilt, float speedP, float speedT);
  
  //Stop moving
  int StopMoving(int cam, int sync, int socket);
  
  //Turn camea ON
  int PowerON(int cam, int sync);
  
  //Turn camera OFF
  int PowerOFF(int cam, int sync);
  
  //Turn back light on
  int BackLightON(int cam, int sync);
  
  //Turn back light off
  int BackLightOFF(int cam, int sync);
  
  //Set memory
  int SetMemory(int cam, int sync, int value);
  
  //Call memory
  int RecallMemory(int cam, int sync, int value);
  
  //Request the current position.
  int PositionRequest(int cam, float *theArray);
  
  /**********************************************************************
  * SONY COMMAND INSTRUCTIONS
  *********************************************************************/
  
  // Cancel a command
  int CommandCancel(int cam, int socket);
  int ptCancel( int cam );
  
  // Turn power ON and OFF
  int CAM_Power(int cam, int sync, int on_off);
  
  // Turn auto power off ON and to a certain timeout
  int CAM_AutoPowerOff(int cam, int sync, int timeout);
  
  // Camera zoom setting
  int CAM_Zoom(int cam, int sync, int name, ...);
  
  //Camera Focus setting
  int CAM_Focus(int cam, int sync, int name, ...);
  
  //Camera white balance setting
  int CAM_WB(int cam, int sync, int name);
  
  // Camera RGain
  int CAM_RGain(int cam, int sync, int name, ...);
  
  // Camera BGain
  int CAM_BGain(int cam, int sync, int name, ...);
  
  // Camera Aperture exposure
  int CAM_AE(int cam, int sync, int name);
  
  // Camera slow sutter setting
  int CAM_SlowShutter(int cam, int sync, int name);
  
  // Camera shutter setting
  int CAM_Shutter(int cam, int sync, int name, ...);
  
  // camera Iris setting
  int CAM_Iris(int cam, int sync, int name, ...);
  
  // Camera Gain setting
  int CAM_Gain(int cam, int sync, int name, ...);
  
  // Camera Brightness setting
  int CAM_Bright(int cam, int sync, int name, ...);
  
  // Camera exposure compensation setting
  int CAM_ExpComp(int cam, int sync, int name, ...);
  
  // Camera Back light setting
  int CAM_BackLight(int cam, int sync, int name);
  
  // Camera Aperture setting
  int CAM_Aperture(int cam, int sync, int name, ...);
  
  // Camera Wide view setting
  int CAM_Wide(int cam, int sync, int name);
  
  // Camera left right mirroring setting
  int CAM_LR_Reverse(int cam, int sync, int name);
  
  // Camera free picture setting
  int CAM_Freeze(int cam, int sync, int name);
  
  // camera picture effect setting
  int CAM_PictureEffect(int cam, int sync, int name);
  
  // Camera digital effect setting
  int CAM_DigitalEffect(int cam, int sync, int name, ...);
  
  // Set, recall or reset camera memory
  int CAM_Memory(int cam, int sync, int name, int memory);
  
  // Turn data screen on or off
  int DataScreen(int cam, int sync, int name);
  
  // Set IR to receive or not.
  int IR_Receive(int cam, int sync, int name);
  
  //Set camera to send IR messages or not
  int IR_ReceiveReturn(int cam, int sync, int name);
  
  // Set pan tilt drive options and moves the camera.
  int PanTiltDrive(int cam, int sync, int name, ...);
  
  // Sets the pan and tilt limit
  int PanTiltLimitSet(int cam, int sync, int name, ...);
  
  /**********************************************************************
  * SONY INQUIRY INSTRUCTION
  *********************************************************************/
  // Camera power inquiry
  int CAM_PowerInq(int cam);
  
  // Camera auto power off timer inquiry
  int CAM_AutoPowerOffInq(int cam);
  
  // Camera Digital zoom status inquiry
  int CAM_DZoomModeInq(int cam);
  
  // Camera zoom position inquiry
  int CAM_ZoomPosInq(int cam);
  
  // Camera Focus mode status inquiry
  int CAM_FocusModeInq(int cam);
  
  // Camera focus position inquiry
  int CAM_FocusPosInq(int cam);
  
  // Camera auto focus sensitivity inquiry
  int CAM_AFModeInq(int cam);
  
  // Camera focus near limit inquiry
  int CAM_FocusNearLimitInq(int cam);
  
  // Camera white balance inquiry
  int CAM_WBModeInq(int cam);
  
  // Camera R Gain inquiry
  int CAM_RGainInq(int cam);
  
  // Camera B Gain inquiry
  int CAM_BGainInq(int cam);
  
  // Camera Auto exposure inquiry
  int CAM_AEModeInq(int cam);
  
  // Camera Slow shutter inquiry
  int CAM_SlowShutterModeInq(int cam);
  
  // Camera shutter position inquiry
  int CAM_ShutterPosInq(int cam);
  
  // Camera Iris position inquiry
  int CAM_IrisPosInq(int cam);
  
  // Camera Gain position inquiry
  int CAM_GainPosInq(int cam);
  
  // Camera Bright position inquiry
  int CAM_BrightPosInq(int cam);
  
  // camera Exposure compensation inquiry
  int CAM_ExpCompModeInq(int cam);
  
  // Camera exposure compensation position
  int CAM_ExpCompPosInq(int cam);
  
  // Camera back light status
  int CAM_BackLightModeInq(int cam);
  
  // Camera aperture gain inquiry
  int CAM_ApertureInq(int cam);
  
  // Camera wide mode inquiry
  int CAM_WideModeInq(int cam);
  
  //Camera left right reverse inquiry
  int CAM_LR_ReverseModeInq(int cam);
  
  // Camera freeze image inquiry
  int CAM_FreezeModeInq(int cam);
  
  // Camera picture effect inquiry
  int CAM_PictureEffectModeInq(int cam);
  
  // Camera Digital effect inquiry
  int CAM_DigitalEffectModeInq(int cam);
  
  // Camera effect level inquiry
  int CAM_DigitalEffectLevelInq(int cam);
  
  // Camera memory inquiry
  int CAM_MemoryInq(int cam);
  
  // Camera data screen inquiry
  int DatascreenInq(int cam);
  
  // Pan/Tilt status
  int PanTiltModeInq(int cam, int *output);
  
  // Checks Pan/Tilt max speed
  int PanTiltMaxSpeedInq(int cam, int *output);
  
  // Checks current position of the camera
  int PanTiltPositionInq(int cam, float *output);
  
  // Checks the video system of the camera
  int VideoSystemInq(int cam);
  
  // Checks information about the acamera.
  int CAM_DeviceTypeVersionInq(int cam, int *output);
  
  //Retrieves the pan, tilt, and zoom
  int CAM_PTZInq( int cam, float *myPan, float *myTilt, int *myZoom );
  
  /**********************************************************************
  * CANON COMMAND INSTRUCTIONS
  *********************************************************************/
  
  //Sets the panning speed.
  int PanSpeedAssignment(float speed);
  
  //sets the tilt speed.
  int TiltSpeedAssignment(float speed);
  
  //Stops PAN and TILT running
  int PanTiltRunningStop();
  
  //Start panning right
  int PanRightStart(int sync);
  
  //Start panning left
  int PanLeftStart(int sync);
  
  //Start tilting up
  int TiltUpStart(int sync);
  
  //Start tilting down
  int TiltDownStart(int sync);
  
  //Go to home position
  int HomePosition(int sync);
  
  //Initialize and go to home position
  int PedestalInitialize1(int sync);
  
  //Initialize and stay at same position
  int PedestalInitialize2(int sync);
  
  //Stop pan and tilt motion
  int PanTiltMotionStop();
  
  //Start or stop pan and tilt
  int PanTiltStartStop(int panDirectionStartStop, 
                      int tiltDirectionStartStop, int sync);
  
  //Move camera to a certain position
  int PanTiltAngleAssignment(int panAngle, int tiltAngle, int sync);
  
  //Set the range for panning
  int PanMovableRangeAssignment(int minimumAngle, int maximumAngle);
  
  //Set the range for tilting
  int TiltMovableRangeAssignment(int minimumAngle, int maximumAngle);
  
  //Turn the camera off
  int CameraOFF(int sync);
  
  //Turn the camera on
  int CameraON(int sync);
  
  //Sets auto focus
  int FocusAuto();
  
  //Sets manual focus
  int FocusManual();
  
  //Sets focus near
  int FocusNear(int sync);
  
  //Sets focus far
  int FocusFar(int sync);
  
  //Sets the focus to a certain position
  int FocusPositionAssignment(int focusPoint, int sync);
  
  //Sets the focus with 1 push.
  int OnePushAF(int sync);
  
  //Stop zooming
  int ZoomStop(int sync);
  
  //zoom wide
  int ZoomWide(int sync);
  
  //zoom tele.
  int ZoomTele(int sync);
  
  //zoom wide at hi speed.
  int ZoomHiWide(int sync);
  
  //zoom tele at hi speed.
  int ZoomHiTele(int sync);
  
  //set a position for zoom
  int ZoomPosition1Assignment(int zoom, int sync);
  
  //set a position for zoom
  int ZoomPosition2Assignment(int zoom, int sync);
  
  //Sets the zoom speed
  int ZoomSpeedAssignment(int speed);
  
  //Turn backlight off
  int BackLightCompensationOFF(int sync);
  
  //Turn backlight on
  int BackLightCompensationON(int sync);
  
  //Set auto exposure on
  int ExposureAuto();
  
  //Set manual exposure on
  int ExposureManual();
  
  //Set auto exposure lock off
  int AELockOFF();
  
  //Set auto exposure lock on
  int AELockON();
  
  //set the shutter speed to program mode
  int ShutterSpeedProgram();
  
  //Set shutter speed to 1/60 (PAL 1/50)
  int ShutterSpeed1_60(int sync);
  
  //Set the shutter speed to 1/100 (PAL 1/120)
  int ShutterSpeed1_100(int sync);
  
  //Set the shutter speed to a certain value.
  int ShutterSpeedAssignment(int speed, int sync);
  
  //Set the AGC gain
  int AGCGainAssignment(int gain);
  
  //Set the iris size
  int IrisAssignment(int size, int sync);
  
  //Set the brightness
  int AETargetValueAssignment(int bright, int sync);
  
  //Set white balance to auto
  int AutoWhiteBalanceNormal();
  
  //Lock auto white balance
  int AutoWhiteBalanceLock();
  
  //Set white balance to manual
  int WhiteBalanceManualMode(int sync);
  
  //Set a white balance value
  int WhiteBalanceValueAssignment(int value);
  
  // Fade slowly
  int FadeNormal(int sync);
  
  //Fade white slowly
  int FadeWhite(int sync);
  
  //Fade white fast
  int FadeHiSpeedWhite(int sync);
  
  //Fade black fast
  int FadeHiSpeedBlack(int sync);
  
  //Reset camera status to certain values
  int CameraReset(int sync);
  
  //Turn remote controller on
  int RemoteControlON();
  
  //Turn remote controller off
  int RemoteControlOFF();
  
  //Set memory
  int PresetSet(int value);
  
  //Use preset memory
  int PresetMove(int value, int sync);
  
  //Allow messages from the camera when using remote.
  int RemoteControllerThroughSetting(int value);
  
  //Set led to normal display
  int LedNormalDisplay();
  
  //Set led to forced display
  int LedForcedControl(int value);
  
  //Set cascade connection of cameras off
  int cascadeOFF();
  
  //Set cascade connection of cameras on
  int cascadeON();
  
  //Give control to host
  int HostControlMode();
  
  //Give control to remote.
  int LocalControlMode();
  
  //Display control
  int ScreenControl(int value);
  
  //Display characters on screen
  int DisplayCharacterDataAssignment(int horizontal, 
                                    int vertical, int character);
  
  //Display the date
  int DisplayDateAssignment(int year, int month, int day);
  
  //Display the time
  int DisplayTimeSetting(int hour, int minute, int second);
  
  //set factory defaults
  int DefaultSetting();
  
  //Set the termination notification on or off
  int CommandTerminationNotificationSetting(int value);
  
  //Set global notification on or off
  int GlobalNotificationSetting(int value);
  
  /**********************************************************************
  * CANON INQUIRY INSTRUCTION
  *********************************************************************/
  // Get the PAN speed
  int PanSpeedRequest();
  
  //Get the TILT speed
  int TiltSpeedRequest();
  
  //Get the slowest pan speed
  int PanSlowestSpeedRequest();
  
  //Get the fastest pan speed
  int PanFastestSpeedRequest();
  
  //Get the slowest tilt speed
  int TiltSlowestSpeedRequest();
  
  //Get the fastest tilt speed
  int TiltFastestSpeedRequest();
  
  //Get convertion coefficient for pan angle
  float PanAnglePulseRatioRequest();
  
  //Get convertion coefficient for tilt angle
  float TiltAnglePulseRatioRequest();
  
  //Get the minimum range of panning possible
  int PanMinimumAngleRequest();
  
  //Get the maximum range of panning possible
  int PanMaximumAngleRequest();
  
  //Get the minimum range of tilting possible
  int TiltMinimumAngleRequest();
  
  //Get the maximum range of tilting possible
  int TiltMaximumAngleRequest();
  
  //Get the pan and tilt angle
  int PanTiltAngleRequest(float *theArray);
  
  //Get the pan range
  int PanMovableRangeRequest(int *theArray);
  
  //Get the tilt range
  int TiltMovableRangeRequest(int *theArray);
  
  //Gets the current focus position
  int FocusPositionRequest();
  
  //Get the focus range
  int FocusRangeRequest(int *theArray);
  
  // Get the zoom position
  int ZoomPosition1Request();
  
  // Get the zoom position
  int ZoomPosition2Request();
  
  //Get the zoom speed
  int ZoomSpeedRequest();
  
  //get the maximmum zoom position
  int ZoomMaximumRequest();
  
  //Get the shutter speed.
  int ShutterSpeedRequest();
  
  //Get the AGC gain
  int AGCGainRequest();
  
  //Get the iris size
  int IrisRequest();
  
  //Get the brightness
  int AETargetValueRequest();
  
  //Get the white balance value
  int WhiteBalanceValueRequest();
  
  //Return the zoom ratio
  int ZoomRatioRequest();
  
  //Get the pixel size
  int PixelSizeRequest(int *array);
  
  //Get the version of camera section
  int ProductVersionRequest();
  
  //Get the EEPROM version
  int EEPROMVersionRequest();
  
  //Get the opration status
  int OperationStatusRequest();
  
  //Get extended operation status
  int ExtendedOperationStatusRequest();
  
  //Get the product name
  int ProductNameRequest(int *theArray);
  
  //Get the rom version
  int RomVersionRequest(int *theArray);
  
  //Memory status request
  int PresetStatusRequest();
  
  //Get extended memory information
  int ExtendedPresetStatusRequest();
  
  //get chraracter at a certain location
  int DisplayCharacterDataRequest(int horizontal, int vertical, 
                                  int *theArray);
  
  //Get the data displayed
  int DisplayDateRequest(int *theArray);
  
  //Get the time displayed
  int DisplayTimeRequest(int *theArray);
  
  //Get amount of time camera has been turned on
  int TurningONTimeRequest(int part);
  
  //Get pedestal model
  int PedestalModelRequest();
  
  //Get camera model
  int cameraModelRequest();
  
  private:
    /* Protototypes */ 
    void DecConvert(int dec);
    
    int HexConvert(command *hex, int firstpos);
    
    void ResetArray();
    
    void AsciiConvert();
    
    int getValue(int start, int param);
    
    
    
    
    
    
    
    /* Global variables. */
    int fd; // The file descriptor for the serial port
    
    flag aFlag; // Flag stating if it's SONY or CANON
    
    command aCommand; // Command to send to camera.
    
    //2nd command for sending multiple inquiries
    command eTestCommand;
    
    int error; // Error returned by functions.
    
    int count; // Count into the array.
    
    unsigned char array[4]; // Array for decimal to hex conversion.
    
    int out; // Return socket number
    
    int speedPan;
    int speedTilt; //Speed for pan and tilt
    
    int ptSocket; // The socket on which the current PT is operating

};
#endif

