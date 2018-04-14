
#ifndef SIDUALCAMOPTIONS_H
#define SIDUALCAMOPTIONS_H

#include "types.h"

/**************************************************************
 *
 * SIDualCamOptions class
 * Description: A collection of all the various options used by
 *              the SIDualCam class. All options are public.
 **************************************************************
 */
class SIDualCamOptions {

  public:
    /******************************************
    * ctor
    * Description: Sets defaults for all options
    ******************************************
    */
    SIDualCamOptions();

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
    void copy(SIDualCamOptions const *that );

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
      SIDualCamOptions *newOptions );

  public:
    
    //True when control should be coordinated between the panoramic and 
    //zoomed cameras.
    BOOL arbitrateControl;
    //True when display should be coordinated between the panoramic and 
    //zoomed cameras.
    BOOL arbitrateDisplay;
    //The minimum zoom level of zooming camera
    FLOAT minZoomMagZoomCam;
    //The options to be loaded for the panoramic camera when hybridly zooming
    CHAR panoramicCamOptions[CO_PATH_LENGTH];
    //The options to be loaded for the zooming camera when hybridly zooming
    CHAR zoomCamOptions[CO_PATH_LENGTH];
    
    // Video destination options //////////////////////
    //The prefix to be tacked onto the beginning of the video file names
    CHAR vidDstDirectory[CO_PATH_LENGTH];
    CHAR vidDstPrefix[CO_PATH_LENGTH];
    //The suffix to be tacked onto the end of the video file names
    CHAR vidDstSuffix[CO_PATH_LENGTH];
    //True when the original streams from the camera should be saved
    BOOL vidDstSaveOrigStream;
    //True when the digitally altered streams should be saved
    BOOL vidDstSaveDigStream;
    //True when the HZO stream should be saved
    BOOL vidDstSaveSplicedStream;
    
    // Logging options////////////////////////////////
    BOOL saveStats;
    
    
};

#endif

// File: $Id: SIDualCamOptions.h,v 1.5 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Description: The options for an object in the Controller class
// Revisions:
// $Log: SIDualCamOptions.h,v $
// Revision 1.5  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.4  2005/09/10 02:42:48  edn2065
// Added minimum zoom option. Added timer to apply in menu.
//
// Revision 1.3  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.2  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.1  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SIKernel
//
// Revision 1.22  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.21  2005/08/09 21:59:27  edn2065
// Added easy serialPort switching via SIKernelOptions files
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
