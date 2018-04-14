
#ifndef MENU_H
#define MENU_H

//For printf
#include <stdio.h>
//Gateway to the kernel
#include "SISingleCamOptions.h"
#include "SISingleCam.h"
//General types
#include "types.h"

//Length of temporary text string
#define MNU_TEMP_TEXT_LENGTH (1000)
#define MNU_BUFFER_LENGTH (1000)

/**************************************************************
 *
 * Menu class (abstract)
 * Description: A menu system.
 *
 *
 **************************************************************
 */
class Menu
{
  public:
    /******************************************
     * ctor
     * Description: Sets textMenu for new menu
     * Parameters:  text - string of menu text
     ******************************************
     */
    Menu( const CHAR * text );

    /******************************************
     * display
     * Description: Outputs the menu's text
     * Parameters:  parent (optional) - Sets the parent of this menu
     ******************************************
     */
    void display( Menu *parent );
    void display();

    /******************************************
     * displayBanner
     * Description: Outputs the main banner for Menu.
     * Returns:     Ptr to main menu
     ******************************************
     */
    static Menu *displayBanner();

    /******************************************
     * performAction
     * Description: Outputs the main banner for Menu.
     * Parameters:  command - action request
     * Returns:     Ptr to new current menu
     ******************************************
     */
    virtual Menu *performAction( CHAR command ) = 0;

    /******************************************
     * print
     * Description: Prints text
     * Parameters:  text - text to print
     ******************************************
     */
    static void print( const CHAR *text );

    /******************************************
     * readLine
     * Description: Reads a line of text
     * Returns:     Pointer to the text. Do not delete.
     ******************************************
     */
    static char *readLine();

    /******************************************
     * readUINT32
     * Description: Reads a line of text and converts it to
     *              a number. Returns 0 if bad number.
     * Returns:     The number. 0 if not a good number.
     ******************************************
     */
    static UINT32 readUINT32();

    /******************************************
     * readUINT16
     * Description: Reads a line of text and converts it to
     *              a number. Returns 0 if bad number.
     * Returns:     The number. 0 if not a good number.
     ******************************************
     */
    static UINT16 readUINT16();

    /******************************************
     * readUINT8
     * Description: Reads a line of text and converts it to
     *              a number. Returns 0 if bad number.
     * Returns:     The number. 0 if not a good number.
     ******************************************
     */
    static UINT8 readUINT8();

    /******************************************
     * readFLOAT
     * Description: Reads a line of text and converts it to
     *              a number. Returns 0.0 if bad number.
     * Returns:     The number. 0.0 if not a good number.
     ******************************************
     */
    static FLOAT readFLOAT();

    /******************************************
     * readDOUBLE
     * Description: Reads a line of text and converts it to
     *              a number. Returns 0.0 if bad number.
     * Returns:     The number. 0.0 if not a good number.
     ******************************************
     */
    static DOUBLE readDOUBLE();

    /******************************************
     * setOptions
     * Description: Sets the current options structure
     * Parameters:  myOptions - pointer to the options
     ******************************************
     */
    static void setOptions( SISingleCamOptions *myOptions );

    /******************************************
     * displayOptions
     * Description: Display the current value of the optoins
     *              that can be changed with this menu
     ******************************************
     */
    virtual void displayOptions();

  protected:
    /******************************************
     * notImplemented
     * Description: Notifies user that feature is not yet implemented
     *              by printing.
     ******************************************
     */
    void notImplemented();

    /******************************************
     * handleCommon
     * Description: Similar to performAction, but covers actions that
     *              are common to all menus
     * Parameters:  command - action request
     * Returns:     Ptr to new current menu
     ******************************************
     */
    Menu *handleCommon( CHAR command );

    
  public:
    //For testing program
    static BOOL execute;
    
  protected:  
    //The parent of this menu
    Menu *parentMenu;
    //The options for all Menus; the gateway to the kernel.
    static SISingleCamOptions *options;
    static SISingleCam *siKernel;
    static SISingleCam *siKernelPan;
    static SISingleCam *siKernelZoom;
    //Used as a scratch pad for all inputs and outputs
    static CHAR mnuTempText[MNU_TEMP_TEXT_LENGTH];

  private:
    //The text of this menu
    const CHAR * textMenu;

};

/**************************************************************
 *
 * MenuMain class
 * Description: The main menu of the system.
 **************************************************************
 */
class MenuMain : public Menu
{
  public:
    MenuMain();
    Menu *performAction( CHAR command );
    void displayOptions();
  private:
};

/**************************************************************
 *
 * MenuDisplay class
 * Description: The display menu
 **************************************************************
 */
class MenuDisplay : public Menu
{
  public:
    MenuDisplay();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};

/**************************************************************
 *
 * MenuIO class
 * Description: The I/O menu
 **************************************************************
 */
class MenuIO : public Menu
{
  public:
    MenuIO();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};


/**************************************************************
 *
 * MenuVideoDestination class
 * Description: The video destination selection menu
 **************************************************************
 */
class MenuVideoDestination : public Menu
{
  public:
    MenuVideoDestination();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};

/**************************************************************
 *
 * MenuVideoSource class
 * Description: The video source selection menu
 **************************************************************
 */
class MenuVideoSource : public Menu
{
  public:
    MenuVideoSource();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};

/**************************************************************
 *
 * MenuSelectOutput class
 * Description: The video output selection menu
 **************************************************************
 */
class MenuSelectOutput : public Menu
{
  public:
    MenuSelectOutput();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};

/**************************************************************
 *
 * MenuVideoCapture class
 * Description: The video output selection menu
 **************************************************************
 */
class MenuVideoCapture : public Menu
{
  public:
    MenuVideoCapture();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};


/**************************************************************
 *
 * MenuVideoDestinationType class
 * Description: The video destination type selection menu
 **************************************************************
 */
class MenuVideoDestinationType : public Menu
{
  public:
    MenuVideoDestinationType();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};

/**************************************************************
 *
 * MenuVideoSourceType class
 * Description: The video source type selection menu
 **************************************************************
 */
class MenuVideoSourceType : public Menu
{
  public:
    MenuVideoSourceType();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:
};

/**************************************************************
 *
 * MenuTracking class
 * Description: The Tracking menu
 **************************************************************
 */
class MenuTracking : public Menu
{
  public:
    MenuTracking();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuFixationAlg class
 * Description: The Fixation Algorithm selection menu
 **************************************************************
 */
class MenuFixationAlg : public Menu
{
  public:
    MenuFixationAlg();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuZoomAlg class
 * Description: The Zoom Control Algorithm selection menu
 **************************************************************
 */
class MenuZoomAlg : public Menu
{
  public:
    MenuZoomAlg();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};


/**************************************************************
 *
 * MenuTrackerType class
 * Description: The tracker type selection menu
 **************************************************************
 */
class MenuTrackerType : public Menu
{
  public:
    MenuTrackerType();
    Menu *performAction( CHAR command );

    /******************************************
     * setTrackerType
     * Description: Since there are more than one activies that
     *              require Trackers, this menu will be used from many
     *              different parent menus. Therefore, the desired
     *              tracker to change can be selected using this
     *              function
     * Parameters:  tt - The TrackerType that will be changed
     ******************************************
     */
    void setTrackerType(TrackerType *tt);
  protected:
  private:
    //What TrackerType to alter.
    TrackerType *ttToChange;
};

/**************************************************************
 *
 * MenuTrackerColorType class
 * Description: The tracker type selection menu
 **************************************************************
 */
class MenuTrackerColorType : public Menu
{
  public:
    MenuTrackerColorType();
    Menu *performAction( CHAR command );

    /******************************************
     * setTrackerType
     * Description: Since there are more than one activies that
     *              require Trackers, this menu will be used from many
     *              different parent menus. Therefore, the desired
     *              tracker to change can be selected using this
     *              function
     * Parameters:  tt - The TrackerType that will be changed
     ******************************************
     */
    void setTrackerColorType(TrackerColorType *tt);
  protected:
  private:
    //What TrackerType to alter.
    TrackerColorType *ttToChange;
};

/**************************************************************
 *
 * MenuMFS class
 * Description: The Measure Final Size (MFS) menu
 **************************************************************
 */
class MenuMFS : public Menu
{
  public:
    MenuMFS();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuPTZ class
 * Description: The Pan/Tilt/Zoom (PTZ) menu
 **************************************************************
 */
class MenuPTZ : public Menu
{
  public:
    MenuPTZ();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuPan class
 * Description: The Pan menu
 **************************************************************
 */
class MenuPan : public Menu
{
  public:
    MenuPan();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuTilt class
 * Description: The Tilt menu
 **************************************************************
 */
class MenuTilt : public Menu
{
  public:
    MenuTilt();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuZoom class
 * Description: The Zoom menu
 **************************************************************
 */
class MenuZoom : public Menu
{
  public:
    MenuZoom();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuCalibrateZoom class
 * Description: The Calibrate Zoom menu
 **************************************************************
 */
class MenuCalibrateZoom : public Menu
{
  public:
    MenuCalibrateZoom();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuLoadSaveConfiguration class
 * Description: The Load/Save Configuration menu
 **************************************************************
 */
class MenuLoadSaveConfiguration : public Menu
{
  public:
    MenuLoadSaveConfiguration();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 *
 * MenuHzo class
 * Description: The Hybrid zoom out menu
 **************************************************************
 */
class MenuHzo : public Menu
{
  public:
    MenuHzo();
    Menu *performAction( CHAR command );
    void displayOptions();
  protected:
  private:

};

/**************************************************************
 * Convert class
 * Description: Used for enum conversion.
 */
class Convert {
  public:
    //The string converted
    CHAR conString [50];

};

/**************************************************************
 * VideoSrcTypeToString class
 * Description: Used for enum conversion of VideoSrcType.
 */
class VideoSrcTypeToString : public Convert
{
  public:
    void convert(VideoSrcType vst);
};


/**************************************************************
 * VideoDstTypeToString class
 * Description: Used for enum conversion of VideoDstType.
 */
class VideoDstTypeToString : public Convert
{
  public:
    void convert(VideoDstType vdt);
};

/**************************************************************
 * TrackerTypeToString class
 * Description: Used for enum conversion of TrackerType.
 */
class TrackerTypeToString : public Convert
{
  public:
    void convert(TrackerType tt);
};

/**************************************************************
 * TrackerColorTypeToString class
 * Description: Used for enum conversion of TrackerColorType.
 */
class TrackerColorTypeToString : public Convert
{
  public:
    void convert(TrackerColorType tt);
};

/**************************************************************
 * FixationTypeToString class
 * Description: Used for enum conversion of FixationType.
 */
class FixationTypeToString : public Convert
{
  public:
    void convert(FixationType tt);
};

#endif

// File: $Id: Menu.h,v 1.15 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: Menu.h,v $
// Revision 1.15  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.14  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.13  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.12  2005/08/26 00:57:21  edn2065
// Added menu options for HZO. Setup control arbitration
//
// Revision 1.11  2005/08/25 23:55:59  edn2065
// Removed deadlock in SIDualCam and separated SISingleCams from GUI
//
// Revision 1.10  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.9  2005/06/28 18:33:26  edn2065
// Completed addition of blue and red trackers
//
// Revision 1.8  2005/06/03 12:49:46  edn2065
// Changed Controller to SISingleCam
//
// Revision 1.7  2005/05/26 19:15:02  edn2065
// Added VideoStream. tested. Still need to get tracker working
//
// Revision 1.6  2005/05/26 14:39:47  edn2065
// Made execute from Menu a static class variable.
//
// Revision 1.5  2005/05/24 15:36:51  edn2065
// Implemented I/O display in menu.
// Bugs appeared:
//   Horizontal line on video (maybe a timing issue?)
//   OpenCV experiences NULL pointer because join may not be working
//
// Revision 1.4  2005/04/22 17:09:40  edn2065
// Add MFS,PTZ,and load/save to menus
//
// Revision 1.1  2005/04/21 13:47:39  edn2065
// Added Menus and fully commented. commented ControllerOptions.
//
