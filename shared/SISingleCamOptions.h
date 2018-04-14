
#ifndef SISINGLECAMOPTIONS_H
#define SISINGLECAMOPTIONS_H

#include "types.h"

/**************************************************************
 *
 * SISingleCamOptions class
 * Description: A collection of all the various options used by
 *              the SIKernel class. All options are public.
 **************************************************************
 */
class SISingleCamOptions {

  public:
    /******************************************
    * ctor
    * Description: Sets defaults for all options
    ******************************************
    */
    SISingleCamOptions();

    /******************************************
    * loadOptions
    * Description: Loads options from a file
    * Parameters:  filename - the filename
    * Returns:     Code returned by file I/O
    ******************************************
    */
    void loadOptions( CHAR *fileName );

    /******************************************
    * saveOptions
    * Description: Saves options to a file
    * Parameters:  filename - the filename
    * Returns:     Code returned by file I/O
    ******************************************
    */
    void saveOptions( CHAR *fileName );

    /******************************************
    * copy
    * Description: Copy from one object to "this"
    * Parameters:  that - the object to be copied
    ******************************************
    */
    void copy(SISingleCamOptions const *that );

  private:
   /******************************************
    * setVariable
    * Description: Sets a variable, depicted by a string,
    *              to a value, also a string.
    * Parameters:  var   - the variable
    *              value - the value of that variable
    *              newOptions - the object that should have its
    *                           variable set.
    * Returns:     True if var and value both valid
    ******************************************
    */
    BOOL setVariable( CHAR *var, CHAR *value,
      SISingleCamOptions *newOptions );

  public:
    ////////////////////////////////////////////////////////
    //I/O options
    // Description: All of the options that deal with I/O.
    ////////////////////////////////////////////////////////

    // Video Output /////////////
    // The type of video to output
    VideoDstType vidDst;
    // When vidDst is a file, then the video is saved as a number of
    // images. Each of the images are located in a common directory and
    // have a common prefix and suffix, varying only by a number that
    // indicates the order of the images.
    // For example, images going to '/home/edn2065/temp/vid00000.bmp'
    // will have the following parameters:
    //  directory: '/home/edn2065/temp/'
    //  prefix:    'vid'
    //  numDigits: 5
    //  suffix:    'bmp'
    CHAR vidDstDirectory[CO_PATH_LENGTH];
    CHAR vidDstPrefix[CO_PATH_LENGTH];
    UINT8 vidDstNumDigits;
    CHAR vidDstSuffix[CO_PATH_LENGTH];
    //Whether to remove the horizontal line on the screen
    BOOL vidDstPreventLine;

    //Video Input //////////////
    //The type of video to input
    VideoSrcType vidSrc;
    // When vidSrc is a file, then the video was saved as a number of
    // images. Each of the images are located in a common directory and
    // have a common prefix and suffix, varying only by a number that
    // indicates the order of the images.
    // For example, images starting from '/home/edn2065/temp/vid00012.bmp'
    // will have the following parameters:
    //  prefix: '/home/edn2065/temp/vid'
    //  numDigits: 5
    //  suffix:    'bmp'
    //  start:12
    //  loop: true to start over after last image read. False to generate EOF.
    //
    // On the other hand, when the source is a frame grabber, then
    // Directory and file should be set accordingly.
    // For example, '/dev/v4l/video0' should be
    //  directory: '/dev/v4l/'
    //  file:      '/video0'
    CHAR vidSrcDirectory[CO_PATH_LENGTH];
    CHAR vidSrcFile[CO_PATH_LENGTH];
    CHAR vidSrcPrefix[CO_PATH_LENGTH];
    UINT8  vidSrcNumDigits;
    CHAR vidSrcSuffix[CO_PATH_LENGTH];
    UINT32  vidSrcStart;
    BOOL vidSrcLoop;

    /////
    // Misc Display
    // True if the FPS calculation should be displayed in the video
    BOOL showFPS;
    // True if the tracking boxes should be displayed in the video
    BOOL showTrackingBoxes;
    // The maximum number of tracking boxes to display
    UINT8 numTrackingBoxes;
    // Which video is being output.
    UINT8 videoStreamOutput;

    ///////////////////////////
    //Tracking options
    ///////////////////////////
    TrackerType trackingTrkr;
    //The color to be segmented when tracking
    TrackerColorType trackingTrkrColor;
    // True if tracking should be done
    BOOL track;
    // True if the camera should fixate on the tracked object
    BOOL fixate;
    //True if the tracked object should be made scale
    //invariant
    BOOL scaleInv;
    //True if optical zoom should be used for scale invariance
    BOOL opticalZoom;
    //True if digital zoom should be used for scale invariance
    BOOL digitalZoom;
    //True if the video from the panoramic camera should be spliced
    //with the zoomed camera when a fast zoom out is requested.
    BOOL spliceZoomOut;
    //True if the object should be placed at the center of the frame when zooming.
    //False if digital zoom should just speed up the zoom lens by preserving the 
    //principal point.
    BOOL fixateDigital;
    //The fixation algorithm to be used when fixating
    FixationType fixationAlg;
    //The process noise for the Kalman filter
    FLOAT kalmanProcessNoise;
    //The measurement noise for the Kalman filter
    FLOAT kalmanMeasurementNoise;
    //The proportional gain for fixation
    FLOAT fixationGain;
    //Psi for Tordoff's focal length selection algorithm
    FLOAT tordoffPsi;
    //Gamma1 for Tordoff's focal length selection algorithm
    FLOAT tordoffGamma1;
    //Gamma2 for Tordoff's focal length selection algorithm
    FLOAT tordoffGamma2;
    //The memory factor for zooming
    FLOAT zoomCoeff;
    FLOAT zoomDelayCoeff;
    
    // Agressiveness of zooming parameters //////////////
    
    //The size of the object in the image at which optical zoom should 
    //quit zooming in further
    UINT32 targetAreaOpticalZoom;
    //The size of the object in the image at which digital zoom 
    //zoom for
    UINT32 targetAreaDigitalZoom;
    //True when the focal length selection algorithm should choose the higher
    //magnification between the two gammas. False if it should choose to lower
    //magnification.
    BOOL tordoffErrToHigherZoom;


    /////////////////////////////////
    //Measure final size options
    /////////////////////////////////
    //True if the final size of the tracked object in the video should
    //be monitored
    BOOL mfs;
    //The tracker being used for MFS
    TrackerType mfsTrkr;
    //The color of the MFS tracker
    TrackerColorType mfsTrkrColor;
    //The name of the directory file to save the MFS data
    CHAR mfsDirectory[CO_PATH_LENGTH];
    CHAR mfsFile[CO_PATH_LENGTH];

    /////////////////////////////////
    //Camera communication options
    /////////////////////////////////
    CHAR serialPort[CO_PATH_LENGTH];
    
    /////////////////////////////////
    //Pan options
    /////////////////////////////////
    DOUBLE panPos1;
    DOUBLE panPos2;
    UINT32 panIterations;
    UINT32 panPulseFrames;
    BOOL   panInquiry;

    /////////////////////////////////
    //Tilt options
    /////////////////////////////////
    DOUBLE tiltPos1;
    DOUBLE tiltPos2;
    UINT32 tiltIterations;
    UINT32 tiltPulseFrames; 
    BOOL   tiltInquiry;

    /////////////////////////////////
    //Zoom options
    /////////////////////////////////
    DOUBLE zoomPos1;
    DOUBLE zoomPos2;
    UINT32 zoomIterations;
    UINT32 zoomPulseFrames;
    BOOL   zoomInquiry;

    /////////////////////////////////
    //Calibration options
    /////////////////////////////////
    CHAR calDirectory[CO_PATH_LENGTH];
    CHAR calFile[CO_PATH_LENGTH];
    UINT16 calTableLength;

    /////////////////////////////////
    //Load/Save configuration options
    /////////////////////////////////
    CHAR conDirectory[CO_PATH_LENGTH];
    CHAR conFile[CO_PATH_LENGTH];

};

#endif

// File: $Id: SISingleCamOptions.h,v 1.27 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: SISingleCamOptions.h,v $
// Revision 1.27  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.26  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.25  2005/09/06 01:57:12  edn2065
// Added zoom delay handling
//
// Revision 1.24  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.23  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
// Revision 1.22  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.21  2005/08/09 21:59:27  edn2065
// Added easy serialPort switching via SISingleCamOptions files
//
// Revision 1.20  2005/08/03 02:34:54  edn2065
// Added tordoff psi and gammas to menu
//
// Revision 1.19  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.18  2005/06/28 15:09:40  edn2065
// Added multiple tracker color option to kernel
//
// Revision 1.17  2005/06/09 20:51:22  edn2065
// Added pulsing to zoom and pt
//
// Revision 1.16  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.15  2005/06/07 16:55:57  edn2065
// Created SICameraComms thread, now tryin to test.
//
// Revision 1.14  2005/06/03 12:49:46  edn2065
// Changed Controller to SIKernel
//
// Revision 1.13  2005/05/27 13:20:53  edn2065
// Added ability to change number of ROIs in output
//
// Revision 1.12  2005/05/26 19:15:02  edn2065
// Added VideoStream. tested. Still need to get tracker working
//
// Revision 1.11  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
//
// Revision 1.10  2005/04/22 20:52:48  edn2065
// Implemented load and save of controller options
//
// Revision 1.6  2005/04/21 13:47:39  edn2065
// Added Menus and fully commented. commented ControllerOptions.
//
// Revision 1.5  2005/04/07 14:28:36  edn2065
// Implemented tester. untested tester.
//
// Revision 1.4  2005/04/06 21:21:09  edn2065
// Have number of bugs with threads. Revamping GUI to VideoSourceFile call
//
// Revision 1.3  2005/04/06 19:02:06  edn2065
// Added functionality for creation and destruction of input and output video streams.
//
// Revision 1.2  2005/04/02 13:45:27  edn2065
// Moved types to types.h
//
// Revision 1.1  2005/04/02 12:52:13  edn2065
// Adding file
// ----------------------------------------------------------------------
//
