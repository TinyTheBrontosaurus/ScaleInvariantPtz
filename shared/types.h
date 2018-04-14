
#ifndef TYPES_H
#define TYPES_H

#define VERSION "0.74"

//#define DEBUG_FIXATION
//#define DEBUG_TORDOFF_MEAS
//#define DEBUG_TORDOFF_CMD
//#define DEBUG_SCALE_INV
//#define DEBUG_SCALE_INV_KALMAN
//#define DEBUG_CAM_RX
//#define DEBUG_CAM_TX
//#define DEBUG_CAM_TX_TIMER
//#define DEBUG_PANTILTZOOM_READING_INTERP
//#define DEBUG_HZO_ARBITRATION_AREA
//#define DEBUG_CVC

#define DO_PAN
#define DO_TILT
#define DO_ZOOM_INQ
#define DO_ZOOM_CMD

#ifdef DO_PAN
  #define DO_PANTILT
#else
  #ifdef DO_TILT
    #define DO_PANTILT
  #endif
#endif

//Override the primitives
typedef unsigned char UINT8;
typedef char SINT8;
typedef char CHAR;
typedef unsigned short UINT16;
typedef short SINT16;
typedef unsigned int UINT32;
typedef int SINT32;
typedef float FLOAT;
typedef double DOUBLE;
typedef bool BOOL;

#define MAX_UINT8   ((UINT8) -1)
#define MAX_UINT16  ((UINT16)-1)
#define MAX_UINT32  ((UINT32)-1)
#define MIN_UINT8   ((UINT8)  0)
#define MIN_UINT16  ((UINT16) 0)
#define MIN_UINT32  ((UINT32) 0)

#define MAX_SINT8   ((SINT8) 0x7F      )
#define MAX_SINT16  ((SINT16)0x7FFF    )
#define MAX_SINT32  ((SINT32)0x7FFFFFFF)
#define MIN_SINT8   ((SINT8) 0x80      )
#define MIN_SINT16  ((SINT16)0x8000    )
#define MIN_SINT32  ((SINT32)0x80000000)

// The possible sources of video
typedef enum {
  SRC_NONE,
  SRC_FIRST = SRC_NONE,
  SRC_FILE,
  SRC_FRAME_GRABBER,
  SRC_LAST = SRC_FRAME_GRABBER
}VideoSrcType;

// The possible destinations of video
typedef enum {
  DST_NONE,
  DST_FIRST = DST_NONE,
  DST_FILE,
  DST_SCREEN,
  DST_LAST = DST_SCREEN
} VideoDstType;

// The different types of trackers
typedef enum {
  TRKR_NONE,
  TRKR_FIRST = TRKR_NONE,
  TRKR_CSEG,
  TRKR_CAMSHIFT,
  TRKR_LAST = TRKR_CAMSHIFT
} TrackerType;

// The different types of colors that can be segmented 
typedef enum {
  TRKR_COLOR_RED,
  TRKR_COLOR_FIRST = TRKR_COLOR_RED,
  TRKR_COLOR_BLUE,
  TRKR_COLOR_LAST = TRKR_COLOR_BLUE
} TrackerColorType;

//Step and Pulse control for P/T/Z
typedef enum {
  CMD_NONE,
  CMD_FIRST = CMD_NONE,
  CMD_STEP,
  CMD_PULSE,
  CMD_LAST = CMD_PULSE
} CommandType;

// The different types of fixation algorithms
typedef enum {
  FIX_P,
  FIX_FIRST = FIX_P,
  FIX_KALMAN,
  FIX_LAST = FIX_KALMAN
} FixationType;  

#define CRLF "\n"
#define CR "\r"
#define LF "\f"

//Minimum/Maximum calculation
#define SI_MIN(a,b) ((a) < (b) ? (a) : (b) )
#define SI_MAX(a,b) ((a) > (b) ? (a) : (b) )

//For images
//#define IM_LARGE
//Set interlacing to BOTTOM if not 640/480. If it is 640x480, do interlacing
#ifdef IM_LARGE
  #define IM_INTERLACING V4L2_FIELD_INTERLACED
  #define IM_WIDTH 640
  #define IM_HEIGHT 480
#else
  #define IM_INTERLACING V4L2_FIELD_BOTTOM
  #define IM_WIDTH 320
  #define IM_HEIGHT 240
#endif 
#define IM_BITDEPTH IPL_DEPTH_8U
#define IM_CHANNELS 3
#define SATURATION_RANGE 255 
#define HUE_RANGE 360
#define MAX_ROIS 10

//For ColorSegmentationTracker
#define CSEG_MIN_ROI_W 6
#define CSEG_MIN_ROI_H 6
#define CSEG_DENSITYMAP 1
#define CSEG_DENSITYDIV 1
#define CSEG_MAX_DIST 5
#define CSEG_INTENSITY_MIN 25
#define CSEG_INTENSITY_MAX 245

//For VideoStreams
#define VIDSTR_MAX_STREAMS 100
#define VIDSTR_MAX_DESC_LENGTH 50
#define VID_DST_MAX_DISPLAYS 3
#define VID_DST_SLEEP_TIME 10

//RAM drive for video storage. 
//Storage space = seconds * 30 fps * 250kB/frame * 5 streams
// 30 seconds -> 1.2GB, 45 seconds ->1.7GB
#define VID_DST_RAM_DRIVE_SECONDS_OF_VIDEO 45
#define VID_DST_RAM_DRIVE_NUM_IMAGES (30*VID_DST_RAM_DRIVE_SECONDS_OF_VIDEO)

#define NEW_IMAGE (cvCreateImage(cvSize(IM_WIDTH,IM_HEIGHT), IM_BITDEPTH, IM_CHANNELS))
#define NEW_IMAGE_BW (cvCreateImage(cvSize(IM_WIDTH,IM_HEIGHT), IM_BITDEPTH, 1))
#define NEW_IMAGE_DM (cvCreateImage(cvSize(IM_WIDTH/CSEG_DENSITYMAP,IM_HEIGHT/CSEG_DENSITYMAP), IM_BITDEPTH, 1))
#define ENDISABLED_STRING(arg) ((arg)?"Enabled":"Disabled")
#define E_MIN(a, b) (a < b ? a : b)
#define E_MAX(a, b) (a > b ? a : b)
#define IMAGE_NUM_ORIGINAL 0
#define IMAGE_NUM_DIGITAL 13

//For zoom
#define ZOOM_OPTICAL_MAX_READING 16384
#define ZOOM_DIG_INTERPOLATION CV_INTER_LINEAR
#define LKP_DEFAULT_LENGTH ZOOM_OPTICAL_MAX_READING
#define ZOOM_DIG_KALMAN_PROC_NOISE 0.0005
#define ZOOM_DIG_KALMAN_MEAS_NOISE 1.0

//For Camera comms
#define SYNC 1
#define ASYNC 0

//For Pulsing
#define PAN_PULSE_FRAMES_DEFAULT  15
#define TILT_PULSE_FRAMES_DEFAULT 15
#define ZOOM_PULSE_FRAMES_DEFAULT 50

//For handling delays in P/T/Z readings and image measurement
#define PTZ_HISTORY_SIZE 10

//The delay in milliseconds between pan/tilt/zoom readings and image capture.
//See 6/17 page 4 notes
#define PTZ_PANTILT_DELAY 0
#define PTZ_ZOOM_DELAY    0
#define PTZ_MAX_SOCKETS 2

//The gain of the P controller in the fixation loop for P/T
#define SI_FIXATION_GAIN 1.0

//The frequency of pan and zoom readings/commands
#define CAM_PAN_FREQUENCY 2
#define CAM_ZOOM_FREQUENCY 2
//Periods of pan and zoom readings
#define CAM_PAN_Ts (0.033 * CAM_PAN_FREQUENCY)
#define CAM_ZOOM_Ts (0.033 * CAM_ZOOM_FREQUENCY)

//Constants for SIKernel
#define CO_PATH_LENGTH (255)
#define CO_DELIM (':')
#define CO_COMMENT (';')
#define CO_BUFFER_LENGTH (CO_PATH_LENGTH*4)

//Hyrbrid zoom out borders (because of black border around captured image
#define HZO_ADD_BORDERS
#ifdef HZO_ADD_BORDERS
  #define HZO_LT_BORDER 10
  #define HZO_RT_BORDER 5
  #define HZO_TOP_BORDER 5
  #define HZO_BOT_BORDER 5
#else
  #define HZO_LT_BORDER 1
  #define HZO_RT_BORDER 1
  #define HZO_TOP_BORDER 1
  #define HZO_BOT_BORDER 1
#endif

//Maximum number of frames to allow for zooming camera to recover
#define HZO_MAX_BAD_FRAMES 45

//Distance between center of cameras in meters
#define HZO_CVC_D 0.13

#define CAM_ZOOM_COEFF 0.5

#define CAM_PIXEL_ERROR 5

//Digital zooming coefficients
#define ZOOM_IN_ADD_DEAD_ZONE
#ifdef ZOOM_IN_ADD_DEAD_ZONE
  #define ZOOM_IN_DIGITAL_MIN          (1.03)
  #define ZOOM_OUT_DIGITAL_MAX         (0.97)
#else
  #define ZOOM_IN_DIGITAL_MIN          (1.0)
  #define ZOOM_OUT_DIGITAL_MAX         (1.0)
#endif
#define ZOOM_OUT_DIGITAL_MIN         (0.01)
#define ZOOM_DIGITAL_MAG_DEFAULT     (1.0)

#define UI_TIMED_HZO_APPLY_SECONDS   10

#define CAM_TX_WATCHDOG_LATENCY 20
#define CAM_TX_WATCHDOG_RESET 30
#define CAM_TX_WATCHDOG_ENABLE

#define CAM_IRIS                13
#define CAM_GAIN                 1
#define CAM_SHUTTER              4     


#endif

// File: $Id: types.h,v 1.53 2005/09/11 01:13:59 edn2065 Exp $
// Author: Eric D Nelson
// Description: Different types to use in the project
// Revisions:
// $Log: types.h,v $
// Revision 1.53  2005/09/11 01:13:59  edn2065
// Added TxThread Watchdog
//
// Revision 1.52  2005/09/10 02:42:48  edn2065
// Added minimum zoom option. Added timer to apply in menu.
//
// Revision 1.51  2005/09/10 00:35:36  edn2065
// synchronized video streams to file (kind of)
//
// Revision 1.50  2005/09/09 20:16:19  edn2065
// Lowered CSEG intensity. Made HZO state determination more stable by checking W and H along with A
//
// Revision 1.49  2005/09/08 23:07:42  edn2065
// Fixed saving wrong stream bug
//
// Revision 1.48  2005/09/08 19:07:49  edn2065
// Created RAM drive for file storage
//
// Revision 1.47  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.46  2005/09/07 01:05:16  edn2065
// Finished CVC implementation
//
// Revision 1.45  2005/09/06 20:41:09  edn2065
// Added dead zone to digital zoom
//
// Revision 1.44  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.43  2005/08/28 00:04:49  edn2065
// Added initial CVC calculations
//
// Revision 1.42  2005/08/26 21:03:51  edn2065
// Put border around spliced image
//
// Revision 1.41  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
// Revision 1.40  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.39  2005/08/18 03:07:22  edn2065
// Fixed digital zoom and digital fixate bug
//
// Revision 1.38  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.37  2005/08/10 02:48:03  edn2065
// Changed to allow warning and error free ICC compile
//
// Revision 1.36  2005/08/09 21:59:27  edn2065
// Added easy serialPort switching via SIKernelOptions files
//
// Revision 1.35  2005/08/09 04:23:42  edn2065
// Scaled measurement noise to go with zoom
//
// Revision 1.34  2005/08/05 23:08:09  edn2065
// Made kalman filter for zooming
//
// Revision 1.33  2005/08/03 01:43:26  edn2065
// Implemented focal length selection
//
// Revision 1.32  2005/08/01 23:30:07  edn2065
// Added zoom invariance to KF
//
// Revision 1.31  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.30  2005/07/28 20:57:49  edn2065
// Kalman filter working
//
// Revision 1.29  2005/07/28 01:42:18  edn2065
// Made it so fixation was only calculated when p/t/z values were available
//
// Revision 1.28  2005/07/26 20:31:51  edn2065
// Gave up trying to improve timing
//
// Revision 1.27  2005/07/26 18:46:43  edn2065
// Added scale invariance
//
// Revision 1.26  2005/07/15 02:50:58  edn2065
// Working on adding synch to fix instability
//
// Revision 1.25  2005/07/15 01:08:58  edn2065
// Integrating RxTxThread. Still has jerkiness bug
//
// Revision 1.24  2005/07/13 23:54:49  edn2065
// Power off save
//
// Revision 1.23  2005/07/01 16:43:05  edn2065
// Created sine wave generator for camera
//
// Revision 1.22  2005/06/30 14:46:28  edn2065
// overnight checkin
//
// Revision 1.21  2005/06/28 18:33:26  edn2065
// Completed addition of blue and red trackers
//
// Revision 1.20  2005/06/28 15:09:40  edn2065
// Added multiple tracker color option to kernel
//
// Revision 1.19  2005/06/22 16:57:40  edn2065
// Tied pantilt and zoom readings together to make them faster
//
// Revision 1.18  2005/06/21 11:13:04  edn2065
// Added delay handling for P/T/Z.
//
// Revision 1.17  2005/06/17 18:35:51  edn2065
// Added verbosity to pulsing
//
// Revision 1.16  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.15  2005/06/10 13:40:47  edn2065
// Added splicing of zoom out
//
// Revision 1.14  2005/06/09 20:51:22  edn2065
// Added pulsing to zoom and pt
//
// Revision 1.13  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.12  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
// Revision 1.11  2005/06/02 18:11:42  edn2065
// Tested ZoomOptical
//
// Revision 1.10  2005/06/01 19:49:29  edn2065
// Checking in for night
//
// Revision 1.9  2005/05/27 13:20:53  edn2065
// Added ability to change number of ROIs in output
//
// Revision 1.8  2005/05/26 17:47:08  edn2065
// Added VideoStream to compileAll
//
// Revision 1.7  2005/05/26 14:08:11  edn2065
// Tracker compiles, but doesnt build because of execute in Menu.h
//
// Revision 1.6  2005/05/25 19:36:16  edn2065
//
// Checking in whatever i forgot to check in
// ----------------------------------------------------------------------
//
// Revision 1.5  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.4  2005/04/22 17:09:40  edn2065
// Add MFS,PTZ,and load/save to menus
//
// Revision 1.2  2005/04/21 13:47:39  edn2065
// Added Menus and fully commented. commented ControllerOptions.
//
// Revision 1.1  2005/04/02 12:58:09  edn2065
// Added to repository
//
//
