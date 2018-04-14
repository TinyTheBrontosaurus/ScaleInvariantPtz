#include "Menu.h"
#include "Barrier.h"
#include "SIDualCam.h"
//for strcpy
#include <string.h>
#include <stdlib.h>

///////////////////////////////////////////
// Definition of static variables
SISingleCamOptions *Menu::options;
CHAR Menu::mnuTempText[MNU_TEMP_TEXT_LENGTH];

///////////////////////////////////////////
// Definition of all the static variables
static VideoDstTypeToString videoDstTypeToString;
static VideoSrcTypeToString videoSrcTypeToString;
static TrackerTypeToString trackerTypeToString;
static TrackerColorTypeToString trackerColorTypeToString;
static FixationTypeToString fixationTypeToString;
//SISingleCam before menus because of VideoStream init.
//static SISingleCam siKernel("last"); 
static SIDualCam siHzo;
SISingleCam *Menu::siKernel;
SISingleCam *Menu::siKernelPan;
SISingleCam *Menu::siKernelZoom;
static MenuMain menuMain;
static MenuDisplay menuDisplay;
static MenuIO menuIO;
static MenuVideoDestination menuVideoDestination;
static MenuVideoSource menuVideoSource;
static MenuVideoDestinationType menuVideoDestinationType;
static MenuVideoSourceType menuVideoSourceType;
static MenuSelectOutput menuSelectOutput;
static MenuVideoCapture menuVideoCapture;
static MenuTracking menuTracking;
static MenuFixationAlg menuFixationAlg;
static MenuZoomAlg menuZoomAlg;
static MenuTrackerType menuTrackerType;
static MenuTrackerColorType menuTrackerColorType;
static MenuMFS menuMFS;
static MenuPTZ menuPTZ;
static MenuPan menuPan;
static MenuTilt menuTilt;
static MenuZoom menuZoom;
static MenuCalibrateZoom menuCalibrateZoom;
static MenuLoadSaveConfiguration menuLoadSaveConfiguration;
static MenuHzo menuHzo;


BOOL Menu::execute;


///////////////////////////////////////////
// Definition of class functions

/*****************************************************************
 * Menu ctor
 *****************************************************************
 */
Menu::Menu( const CHAR * text )
  : textMenu( text )
{
  //All Menus start out with no parent Menu.
  parentMenu = (Menu *) NULL;

  siKernelZoom = siHzo.getZoomCam();
  siKernelPan = siHzo.getPanCam();
  
  siKernel = siKernelZoom;
  
  //Set the local options
  options = &siKernelZoom->options;
}

/*****************************************************************
 * Menu::display
 *****************************************************************
 */
void Menu::display( Menu *parent )
{
  parentMenu = parent;
  display();
}

const CHAR textCommonHeader[] = CRLF CRLF
"====== Scale Invariant Tracker ======" CRLF;
const CHAR textCommonFooter[] =
"c:     Switch camera options"  CRLF
"d:     Toggle display images"  CRLF
"A/a:   Apply changes to all/this"  CRLF
"V/v:   Apply just hybrid timed/immediate"     CRLF
"R/r:   Reset all/this"            CRLF
"?:     Display option values" CRLF
"x:     Go to previous menu"  CRLF
"q:     Quit"                 CRLF
"Enter: Redisplay menu"       CRLF;

/*****************************************************************
 * Menu::display
 *****************************************************************
 */
void Menu::display()
{
  //Display general header and footer with menu-specific text in
  //between
  Menu::print( textCommonHeader );
  Menu::print( textMenu );
  Menu::print( textCommonFooter );
}

/*****************************************************************
 * Menu::handleCommon
 *****************************************************************
 */
Menu *Menu::handleCommon( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
  
    case 'C':
    case 'c':
      if( siKernel == siKernelZoom)
      {
        siKernel = siKernelPan;
        options = &siKernel->options;
        sprintf(  mnuTempText, "Options are panoramic camera" );
      }
      else
      {
        siKernel = siKernelZoom;
        options = &siKernel->options;
        sprintf(  mnuTempText, "Options are zooming camera" );
      }
            
      print( mnuTempText );
      break;
  
    //Toggle digital/original images
    case 'D':
    case 'd':
      if( options->videoStreamOutput == IMAGE_NUM_ORIGINAL )
      {
        options->videoStreamOutput = IMAGE_NUM_DIGITAL;
      }
      else
      {
        options->videoStreamOutput = IMAGE_NUM_ORIGINAL;
      }
      sprintf(  mnuTempText, "VideoStream output is %s",
        siKernel->getVideoStream()->getDescription(options->videoStreamOutput) );
      print( mnuTempText );
      break;
      
    //Apply changes
    case 'A':
      siKernelPan->applyOptions();
      siKernelZoom->applyOptions();
      siHzo.applyOptions();
      break;
    case 'a':
      siKernel->applyOptions();
      siHzo.applyOptions();
      break;
    case 'V':
      //Run 10-second timer
      for( UINT8 ticksLeft = UI_TIMED_HZO_APPLY_SECONDS; 
           ticksLeft > 0; ticksLeft-- )
      {
      
        printf( "%2d...", ticksLeft );
        //Beep on 5, 4, 3 but not 2, 1
        if( ticksLeft <= 5 && ticksLeft >= 3 )
        {
          printf( "****************\a****************"CRLF );
        }
        else
        {
          printf( CRLF );
        }
        sleep(1);
      }
      printf( "  GO!****************\a****************" CRLF );
      siHzo.applyOptions();
      break;
    case 'v':
      siHzo.applyOptions();
      break;
    
      
    //Reset
    case 'R':
      siHzo.reset();
      siKernelZoom->reset();
      siKernelPan->reset();
      break;
      
    case 'r':
      siHzo.reset();
      siKernel->reset();
      break;      
      
    //Display the value of the options
    case '?':
      displayOptions();
      break;

    //Go to previous menu
    case 'X':
    case 'x':
      if( parentMenu != NULL )
      {
        currentMenu = parentMenu;
      }
      break;

    //Quit the program
    case 'Q':
    case 'q':
      execute = false;
      break;

    //Refresh the display
    case '\n':
      display();
    default:
      break;
  }
  return( currentMenu );
}

const CHAR mainBannerText[] =
CRLF CRLF CRLF
"Scale Invariant Tracking Tool Version " VERSION   CRLF
"By Eric D Nelson"                                 CRLF
"Compiled on " __DATE__ " @ " __TIME__             CRLF
                                                   CRLF
"   Being Developed in Partial Fulfillment of the" CRLF
"   Requirements for the Degree of"                CRLF
"   Master of Science in Computer Engineering"     CRLF
"   Supervised by"                                 CRLF
"   Associate Professor Dr. Juan C. Cockburn"      CRLF
"   Department of Computer Engineering"            CRLF
"   Kate Gleason College of Engineering"           CRLF
"   Rochester Institute of Technology"             CRLF
"   Rochester, New York"                           CRLF;

/*****************************************************************
 * Menu::displayBanner
 *****************************************************************
 */
Menu *Menu::displayBanner()
{
  Menu::print( mainBannerText );
  return &menuMain;
}

/*****************************************************************
 * Menu::displayOptions
 *****************************************************************
 */
void Menu::displayOptions()
{
  print( "No options available to display for this menu\n");
}
const CHAR notImplementedText[] =
"This option not yet implemented." CRLF;
/*****************************************************************
 * Menu::notImplemented
 *****************************************************************
 */
void Menu::notImplemented()
{
  Menu::print( notImplementedText );
}

/*****************************************************************
 * Menu::print
 *****************************************************************
 */
void Menu::print( const CHAR *text )
{
  //Print to standard output
  printf( text );
}

/*****************************************************************
 * Menu::readLine
 *****************************************************************
 */
CHAR *Menu::readLine()
{
   scanf( "%s", mnuTempText );
   return mnuTempText;
}

/*****************************************************************
 * Menu::readUINT32
 *****************************************************************
 */
UINT32 Menu::readUINT32()
{
  return (UINT32) atoi(readLine());
}

/*****************************************************************
 * Menu::readUINT16
 *****************************************************************
 */
UINT16 Menu::readUINT16()
{
  return (UINT16)readUINT32();
}

/*****************************************************************
 * Menu::readUINT8
 *****************************************************************
 */
UINT8 Menu::readUINT8()
{
  return (UINT8)readUINT32();
}

/*****************************************************************
 * Menu::readFLOAT
 *****************************************************************
 */
FLOAT Menu::readFLOAT()
{
  return (FLOAT)readDOUBLE();
}

/*****************************************************************
 * Menu::readDOUBLE
 *****************************************************************
 */
DOUBLE Menu::readDOUBLE()
{
  return (DOUBLE)atof(readLine());
}

const CHAR textInMenuMain[] =
"       Main Menu"       CRLF
"       ---------"       CRLF
"Press"                  CRLF
"1:     Display Options" CRLF
"2:     I/O Options"     CRLF
"3:     Tracking Options"CRLF
"4:     Measure Final Size Options"CRLF
"5:     Pan/Tilt/Zoom Options"CRLF
"6:     Load/Save Configuration Options" CRLF
"7:     Hybrid Zoom Out" CRLF
"8:     About"CRLF;

/*****************************************************************
 * Menu::setOptions
 *****************************************************************
 */
void Menu::setOptions( SISingleCamOptions *myOptions )
{
  options = myOptions;
}

/*****************************************************************
 * MenuMain ctor
 *****************************************************************
 */
MenuMain::MenuMain() :
  Menu(textInMenuMain)
{
}

/*****************************************************************
 * MenuMain::performAction
 *****************************************************************
 */
Menu *MenuMain::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Go to display options
    case '1':
      currentMenu = &menuDisplay;
      currentMenu->display( this );
      break;

    //Go to I/O options
    case '2':
      currentMenu = &menuIO;
      currentMenu->display( this );
      break;

    //Go to tracking options
    case '3':
      currentMenu = &menuTracking;
      currentMenu->display( this );
      break;

    //Go to measure final size options
    case '4':
      currentMenu = &menuMFS;
      currentMenu->display( this );
      break;

    //Go to pan/tile/zoom options
    case '5':
      currentMenu = &menuPTZ;
      currentMenu->display( this );
      break;

    //Go to load/save options
    case '6':
      currentMenu = &menuLoadSaveConfiguration;
      currentMenu->display( this );
      break;

    //Go to Hybrid zoom out
    case '7':
      currentMenu = &menuHzo;
      currentMenu->display( this );
      break;
    
    //Show about
    case '8':
      displayBanner();

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuMain::displayOptions
 *****************************************************************
 */
 void MenuMain::displayOptions()
 {
   //Create output notification
   menuDisplay.displayOptions();
   menuIO.displayOptions();
   menuTracking.displayOptions();
   menuMFS.displayOptions();
   menuPTZ.displayOptions();
   menuLoadSaveConfiguration.displayOptions();
 }


const CHAR textInMenuDisplay[] =
"       Display Menu" CRLF
"       ------------" CRLF
"Press" CRLF
"1:     Toggle Show FPS" CRLF
"2:     Toggle Tracking Box(es)" CRLF
"3:     Change Number of Tracking Boxes"CRLF;

/*****************************************************************
 * MenuDisplay ctor
 *****************************************************************
 */
MenuDisplay::MenuDisplay() :
  Menu(textInMenuDisplay)
{
}

/*****************************************************************
 * MenuDisplay::performAction
 *****************************************************************
 */
Menu *MenuDisplay::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Toggle FPS
    case '1':
      //Toggle option
      options->showFPS = !options->showFPS;
      //Create output notification
      sprintf( mnuTempText, "Show FPS %s" CRLF,
        ENDISABLED_STRING(options->showFPS));
      print( mnuTempText );
      break;

    //Toggle Tracking boxes
    case '2':
      //Toggle option
      options->showTrackingBoxes = !options->showTrackingBoxes;
      //Create output notification
      sprintf( mnuTempText, "Show Tracking Box(es) %s" CRLF,
        ENDISABLED_STRING(options->showTrackingBoxes));
      print( mnuTempText );
      break;
      
    //Change number of tracking boxes
    case '3':
      //
      Menu::print("Enter new number of digits: ");
      options->numTrackingBoxes = readUINT8();
      sprintf(  mnuTempText, "   New number of tracking boxes: %d",
            options->numTrackingBoxes );
      print( mnuTempText );
      break;
      
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuDisplay::displayOptions
 *****************************************************************
 */
 void MenuDisplay::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Display: "             CRLF
     "   Show FPS %s"        CRLF
     "   Show Tracking Box(es) %s" CRLF
     "   Number of tracking boxes %d" CRLF,
     ENDISABLED_STRING(options->showFPS),
     ENDISABLED_STRING(options->showTrackingBoxes),
     (UINT16)options->numTrackingBoxes);
   print( mnuTempText );
 }


const CHAR textInMenuIO[] =
"       I/O Menu" CRLF
"       --------" CRLF
"Press" CRLF
"1:     Select Zooming Video Source" CRLF
"2:     Select Panoramic Video Source" CRLF
"3:     Select Video Destination" CRLF
"4:     Select Image to Output" CRLF
"5:     Configure Video Capture" CRLF;

/*****************************************************************
 * MenuIO ctor
 *****************************************************************
 */
MenuIO::MenuIO() :
  Menu(textInMenuIO)
{
}

/*****************************************************************
 * MenuIO::performAction
 *****************************************************************
 */
Menu *MenuIO::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Go to zooming video source options
    case '1':
      currentMenu = &menuVideoSource;
      currentMenu->display( this );
      break;

    // Go to panoramic video source options
    case '2':
      notImplemented();
      break;

    //Go to video destinations options
    case '3':
      currentMenu = &menuVideoDestination;
      currentMenu->display( this );
      break;

    //Go to Image to Output options
    case '4':
      currentMenu = &menuSelectOutput;
      currentMenu->display( this );
      break;

    //Go to video capture options
    case '5':
      currentMenu = &menuVideoCapture;
      currentMenu->display( this );
      break;            
      
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuIO::displayOptions
 *****************************************************************
 */
 void MenuIO::displayOptions()
 {
   //Create output notification
   menuVideoSource.displayOptions();
   menuVideoDestination.displayOptions();
   menuSelectOutput.displayOptions();
   menuVideoCapture.displayOptions();
 }


const CHAR textInMenuVideoDestination[] =
"       Video Destination Menu" CRLF
"       ----------------------" CRLF
"Press"               CRLF
"1:     Change type" CRLF
"2:     Change directory" CRLF
"3:     Change prefix" CRLF
"4:     Change number of digits" CRLF
"5:     Change suffix" CRLF
"6:     Toggle horizontal line prevention" CRLF;
/*****************************************************************
 * MenuDisplay ctor
 *****************************************************************
 */
MenuVideoDestination::MenuVideoDestination() :
  Menu(textInMenuVideoDestination)
{
}

/*****************************************************************
 * MenuVideoDestination::performAction
 *****************************************************************
 */
Menu *MenuVideoDestination::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Change type
    case '1':
      currentMenu = &menuVideoDestinationType;
        currentMenu->display( this );
      break;

    //Change directory
    case '2':
      Menu::print("Enter new directory: ");
      strcpy( options->vidDstDirectory, readLine() );
      sprintf(  mnuTempText, "   New directory: %s",
            options->vidDstDirectory );
      print( mnuTempText );
      break;

    //Change prefix
    case '3':
      Menu::print("Enter new prefix: ");
      strcpy( options->vidDstPrefix, readLine() );
      sprintf(  mnuTempText, "   New prefix: %s",
            options->vidDstPrefix );
      print( mnuTempText );
      break;

    //Change number of digits
    case '4':
      Menu::print("Enter new number of digits: ");
      options->vidDstNumDigits = readUINT8();
      sprintf(  mnuTempText, "   New number of digits: %d",
            options->vidDstNumDigits );
      print( mnuTempText );
      break;

    //Change suffix
    case '5':
      Menu::print("Enter new suffix: ");
      strcpy( options->vidDstSuffix, readLine() );
      sprintf(  mnuTempText, "   New suffix: %s",
            options->vidDstSuffix );
      print( mnuTempText );
      break;
    case '6':
      options->vidDstPreventLine = !options->vidDstPreventLine;
      //Create output notification
      sprintf( mnuTempText, "Prevent Line %s" CRLF,
        ENDISABLED_STRING(options->vidDstPreventLine));
      print( mnuTempText );
      break;

    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuVideoDestination::displayOptions
 *****************************************************************
 */
 void MenuVideoDestination::displayOptions()
 {
   //Create output notification

   print( "Video destination:" CRLF "   ");
   menuVideoDestinationType.displayOptions();
   sprintf( mnuTempText, CRLF
     "   directory: %s"   CRLF
     "   prefix:    %s"   CRLF
     "   numDigits: %d"   CRLF
     "   suffix:    %s"   CRLF
     "   preventLine: %d" CRLF,
     options->vidDstDirectory, options->vidDstPrefix,
     options->vidDstNumDigits, options->vidDstSuffix,
     options->vidDstPreventLine);
   print( mnuTempText );
 }


const CHAR textInMenuVideoCapture[] =
"       Video Capture Menu" CRLF
"       ----------------------" CRLF
"Press"               CRLF
"1:     Change directory" CRLF
"2:     Change prefix" CRLF
"3:     Change suffix" CRLF
"4:     Toggle original stream capture" CRLF
"5:     Toggle digital stream capture" CRLF
"6:     Toggle hybrid stream capture" CRLF;

/*****************************************************************
 * MenuVideoCapture ctor
 *****************************************************************
 */
MenuVideoCapture::MenuVideoCapture() :
  Menu(textInMenuVideoCapture)
{
}

/*****************************************************************
 * MenuVideoCapture::performAction
 *****************************************************************
 */
Menu *MenuVideoCapture::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {

    //Change directory
    case '1':
      Menu::print("Enter new directory: ");
      strcpy( siHzo.options.vidDstDirectory, readLine() );
      sprintf(  mnuTempText, "   New directory: %s",
            siHzo.options.vidDstDirectory );
      print( mnuTempText );
      break;

    //Change prefix
    case '2':
      Menu::print("Enter new prefix: ");
      strcpy( siHzo.options.vidDstPrefix, readLine() );
      sprintf(  mnuTempText, "   New prefix: %s",
            siHzo.options.vidDstPrefix );
      print( mnuTempText );
      break;

    //Change suffix
    case '3':
      Menu::print("Enter new suffix: ");
      strcpy( siHzo.options.vidDstSuffix, readLine() );
      sprintf(  mnuTempText, "   New suffix: %s",
            siHzo.options.vidDstSuffix );
      print( mnuTempText );
      break;
      
    //Toggle save original stream
    case '4':
      siHzo.options.vidDstSaveOrigStream = !siHzo.options.vidDstSaveOrigStream;
      //Create output notification
      sprintf( mnuTempText, "Save original stream %s" CRLF,
        ENDISABLED_STRING(siHzo.options.vidDstSaveOrigStream));
      print( mnuTempText );
      break;

    //Toggle save digital stream
    case '5':
      siHzo.options.vidDstSaveDigStream = !siHzo.options.vidDstSaveDigStream;
      //Create output notification
      sprintf( mnuTempText, "Save digital stream %s" CRLF,
        ENDISABLED_STRING(siHzo.options.vidDstSaveDigStream));
      print( mnuTempText );
      break;
      
    //Toggle save hybrid stream
    case '6':
      siHzo.options.vidDstSaveSplicedStream = !siHzo.options.vidDstSaveSplicedStream;
      //Create output notification
      sprintf( mnuTempText, "Save hybrid stream %s" CRLF,
        ENDISABLED_STRING(siHzo.options.vidDstSaveSplicedStream));
      print( mnuTempText );
      break;            

    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuVideoCapture::displayOptions
 *****************************************************************
 */
 void MenuVideoCapture::displayOptions()
 {
   //Create output notification

   print( "Video capture:" CRLF "   ");
   sprintf( mnuTempText, 
     "   directory: %s"   CRLF
     "   prefix:    %s"   CRLF
     "   suffix:    %s"   CRLF
     "   save orig: %s" CRLF
     "   save dig:  %s" CRLF
     "   save hyb:  %s" CRLF,
     siHzo.options.vidDstDirectory, siHzo.options.vidDstPrefix,
     siHzo.options.vidDstSuffix,
     ENDISABLED_STRING(siHzo.options.vidDstSaveOrigStream),
     ENDISABLED_STRING(siHzo.options.vidDstSaveDigStream),
     ENDISABLED_STRING(siHzo.options.vidDstSaveSplicedStream));
   print( mnuTempText );
 }
 
 
CHAR textInMenuVideoDestinationType[MNU_BUFFER_LENGTH];
/*****************************************************************
 * MenuVideoDestinationType ctor
 *****************************************************************
 */
MenuVideoDestinationType::MenuVideoDestinationType() :
  Menu(textInMenuVideoDestinationType)
{
  //Generate the menu based on the names of the VideoDstType
  strcpy( textInMenuVideoDestinationType,
    "       Video Destination Type Menu" CRLF
    "       ---------------------------" CRLF
    "Press" CRLF);
  for( VideoDstType vdtCounter = DST_FIRST;
       vdtCounter <= DST_LAST;
       vdtCounter = (VideoDstType)((UINT8)vdtCounter+1 ))
  {
    videoDstTypeToString.convert( vdtCounter );
    sprintf( textInMenuVideoDestinationType,
      "%s%d:     %s" CRLF,
      textInMenuVideoDestinationType, (UINT8)vdtCounter,
      videoDstTypeToString.conString);
  }
}

/*****************************************************************
 * MenuVideoDestinationType::performAction
 *****************************************************************
 */
Menu *MenuVideoDestinationType::performAction( CHAR command )
{
  Menu *currentMenu = this;
  //Convert from ASCII to VideoDestType
  UINT8 commandInt = command - (UINT8)'0' - (UINT8)DST_FIRST;

  //Make sure it is a legal input
  if( commandInt <= (UINT8) DST_LAST )
  {
    //Set the new vidDst
    options->vidDst = (VideoDstType)commandInt;
    //Output the new value
    videoDstTypeToString.convert( options->vidDst );
    sprintf(  mnuTempText, "   %s",
      videoDstTypeToString.conString );
    print( mnuTempText );

    //Go to previous menu since option selected
    if( parentMenu != NULL )
    {
      currentMenu = parentMenu;
    }
  }
  else
  {
    currentMenu = handleCommon( command );
  }
  return currentMenu;
}

/*****************************************************************
 * MenuVideoDestinationType::displayOptions
 *****************************************************************
 */
void MenuVideoDestinationType::displayOptions()
{
  //Output the new value
  videoDstTypeToString.convert( options->vidDst );
  sprintf(  mnuTempText, "type:      %s",
    videoDstTypeToString.conString );
  print( mnuTempText );
}

const CHAR textInMenuVideoSource[] =
"       Video Source Menu" CRLF
"       -----------------" CRLF
"Press"               CRLF
"1:     Change type" CRLF
"2:     Change prefix" CRLF
"3:     Change number of digits" CRLF
"4:     Change suffix" CRLF
"5:     Change start" CRLF
"6:     Toggle loop" CRLF
"7:     Change directory" CRLF
"8:     Change file" CRLF;

/*****************************************************************
 * MenuDisplay ctor
 *****************************************************************
 */
MenuVideoSource::MenuVideoSource() :
  Menu(textInMenuVideoSource)
{
}

/*****************************************************************
 * MenuVideoSource::performAction
 *****************************************************************
 */
Menu *MenuVideoSource::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Change type
    case '1':
      currentMenu = &menuVideoSourceType;
      currentMenu->display( this );
      break;

    //Enter new prefix
    case '2':
      Menu::print("Enter new prefix: ");
      strcpy( options->vidSrcPrefix, readLine() );
      sprintf(  mnuTempText, "   New prefix: %s",
            options->vidSrcPrefix );
      print( mnuTempText );
      break;

    //Change number of digits
    case '3':
      Menu::print("Enter new number of digits: ");
      options->vidSrcNumDigits = readUINT8();
      sprintf(  mnuTempText, "   New number of digits: %d",
            options->vidSrcNumDigits );
      print( mnuTempText );
      break;

    //Change suffix
    case '4':
      Menu::print("Enter new suffix: ");
      strcpy( options->vidSrcSuffix, readLine() );
      sprintf(  mnuTempText, "   New suffix: %s",
            options->vidSrcSuffix );
      print( mnuTempText );
      break;

    //Change start
    case '5':
      Menu::print("Enter new start number: ");
      options->vidSrcStart = readUINT32();
      sprintf(  mnuTempText, "   New number of digits: %u",
            options->vidSrcStart );
      print( mnuTempText );
      break;

    //Toggle loop
    case '6':
      options->vidSrcLoop = !options->vidSrcLoop;
      //Create output notification
      sprintf( mnuTempText, "Loop %s" CRLF,
        ENDISABLED_STRING(options->vidSrcLoop));
      print( mnuTempText );
      break;

    //Change directory
    case '7':
      Menu::print("Enter new directory: ");
      strcpy( options->vidSrcDirectory, readLine() );
      sprintf(  mnuTempText, "   New directory: %s",
            options->vidSrcDirectory );
      print( mnuTempText );
      break;

    //Change file
    case '8':
      Menu::print("Enter new file: ");
      strcpy( options->vidSrcFile, readLine() );
      sprintf(  mnuTempText, "   New file: %s",
            options->vidSrcFile );
      print( mnuTempText );
      break;

    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuVideoSource::displayOptions
 *****************************************************************
 */
 void MenuVideoSource::displayOptions()
 {
   //Create output notification

   print( "Video source:" CRLF "   ");
   menuVideoSourceType.displayOptions();
   sprintf( mnuTempText, CRLF
     "   prefix:    %s"   CRLF
     "   numDigits: %d"   CRLF
     "   suffix:    %s"   CRLF
     "   start:     %d"   CRLF
     "   loop:      %s"   CRLF
     "   directory: %s"   CRLF
     "   file:      %s"   CRLF,
     options->vidSrcPrefix,
     options->vidSrcNumDigits, options->vidSrcSuffix,
     options->vidSrcStart,
     ENDISABLED_STRING(options->vidSrcLoop),
     options->vidSrcDirectory, options->vidSrcFile );
   print( mnuTempText );
 }

CHAR textInMenuVideoSourceType[MNU_BUFFER_LENGTH];
/*****************************************************************
 * MenuVideoSourceType ctor
 *****************************************************************
 */
MenuVideoSourceType::MenuVideoSourceType() :
  Menu(textInMenuVideoSourceType)
{
  //Generate the menu based on the names of the VideoDstType
  strcpy( textInMenuVideoSourceType,
    "       Video Source Type Menu" CRLF
    "       ----------------------" CRLF
    "Press" CRLF);
  for( VideoSrcType vstCounter = SRC_FIRST;
       vstCounter <= SRC_LAST;
       vstCounter = (VideoSrcType)((UINT8)vstCounter+1 ))
  {
    videoSrcTypeToString.convert( vstCounter );
    sprintf( textInMenuVideoSourceType,
      "%s%d:     %s" CRLF,
      textInMenuVideoSourceType, (UINT8)vstCounter,
      videoSrcTypeToString.conString);
  }
}

/*****************************************************************
 * MenuVideoSourceType::performAction
 *****************************************************************
 */
Menu *MenuVideoSourceType::performAction( CHAR command )
{
  Menu *currentMenu = this;
  //Convert from ASCII to VideoDestType
  UINT8 commandInt = command - (UINT8)'0' - (UINT8)SRC_FIRST;

  //Make sure it is a legal input
  if( commandInt <= (UINT8) SRC_LAST )
  {
    //Set the new vidSrc
    options->vidSrc = (VideoSrcType)commandInt;
    //Output the new value
    videoSrcTypeToString.convert( options->vidSrc );
    sprintf(  mnuTempText, "   %s",
      videoSrcTypeToString.conString );
    print( mnuTempText );

    //Go to previous menu since option selected
    if( parentMenu != NULL )
    {
      currentMenu = parentMenu;
    }
  }
  else
  {
    currentMenu = handleCommon( command );
  }
  return currentMenu;
}

/*****************************************************************
 * MenuVideoSourceType::displayOptions
 *****************************************************************
 */
void MenuVideoSourceType::displayOptions()
{
  //Output the new value
  videoSrcTypeToString.convert( options->vidSrc );
  sprintf(  mnuTempText, "type:      %s",
    videoSrcTypeToString.conString );
  print( mnuTempText );
}


CHAR textInMenuSelectOutput[MNU_BUFFER_LENGTH];
/*****************************************************************
 * MenuSelectOutput ctor
 *****************************************************************
 */
MenuSelectOutput::MenuSelectOutput() :
  Menu(textInMenuSelectOutput)
{
  //Generate the menu based on the names of the VideoDstType
  strcpy( textInMenuSelectOutput,
    "       Select Output Menu" CRLF
    "       ---------------------------" CRLF
    "Press" CRLF
    "s:     Select output. Options are"CRLF );
  for( UINT8 curVidStream = 0;  
       curVidStream < siKernel->getVideoStream()->getNumStreams();
       curVidStream++)
  {    
    sprintf( textInMenuSelectOutput,
      "%s   %3d:     %s" CRLF,
      textInMenuSelectOutput, (UINT8)curVidStream,
      siKernel->getVideoStream()->getDescription(curVidStream));
  }
}

/*****************************************************************
 * MenuSelectOutput::performAction
 *****************************************************************
 */
Menu *MenuSelectOutput::performAction( CHAR command )
{
  Menu *currentMenu = this;
  UINT8 newOutput;
  
  if( command == 's' || command == 'S' )
  {
    newOutput = readUINT8();
    //Make sure it is a legal input
    if( newOutput < siKernel->getVideoStream()->getNumStreams() )
    {
      options->videoStreamOutput = newOutput;
      sprintf(  mnuTempText, "VideoStream output is %s",
        siKernel->getVideoStream()->getDescription(options->videoStreamOutput) );
      print( mnuTempText );
      //Go to previous menu since option selected
      if( parentMenu != NULL )
      {
        currentMenu = parentMenu;
      }   
    }
  }
  else
  {
    currentMenu = handleCommon( command );
  }
  return currentMenu;
}

/*****************************************************************
 * MenuSelectOutput::displayOptions
 *****************************************************************
 */
void MenuSelectOutput::displayOptions()
{
  //Output the new value
  sprintf(  mnuTempText, "VideoStream output:        %s"CRLF,
    siKernel->getVideoStream()->getDescription(options->videoStreamOutput) );
  print( mnuTempText );
  
}


const CHAR textInMenuTracking[] =
"       Tracking Menu"           CRLF
"       -------------"           CRLF
"Press"                          CRLF
"1:     Select Tracker"          CRLF
"2:     Select Tracker color"    CRLF
"3:     Toggle Tracking"           CRLF
"4:     +Toggle Fixation"          CRLF
"5:     +Toggle Scale Invariance"  CRLF
"6:     -+Toggle Optical Zoom"     CRLF
"7:     -+Toggle Digital Zoom"     CRLF
"8:     --+Toggle Digital Fixation"CRLF
"9:     Select Fixation Algorithm" CRLF
"0:     Set Zoom Control Parameters" CRLF;
/*****************************************************************
 * MenuTracking ctor
 *****************************************************************
 */
MenuTracking::MenuTracking() :
  Menu(textInMenuTracking)
{
}

/*****************************************************************
 * MenuTracking::performAction
 *****************************************************************
 */
Menu *MenuTracking::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Go to select tracker options
    case '1':
      currentMenu = &menuTrackerType;
      menuTrackerType.setTrackerType(&options->trackingTrkr);
      currentMenu->display( this );
      break;

    //Go to select tracker color options
    case '2':
      currentMenu = &menuTrackerColorType;
      menuTrackerColorType.setTrackerColorType(
        &options->trackingTrkrColor);
      currentMenu->display( this );
      break;      
      
    //Toggle tracking
    case '3':
      //Toggle option
      options->track = !options->track;
      //Create output notification
      sprintf( mnuTempText, "Tracking %s" CRLF,
        ENDISABLED_STRING(options->track));
      print( mnuTempText );
      break;

    //Toggle fixation
    case '4':
      //Toggle option
      options->fixate = !options->fixate;
      //Create output notification
      sprintf( mnuTempText, "Fixation %s" CRLF,
        ENDISABLED_STRING(options->fixate));
      print( mnuTempText );
      break;

    //Toggle scale invariance
    case '5':
      //Toggle option
      options->scaleInv = !options->scaleInv;
      //Create output notification
      sprintf( mnuTempText, "Scale Invariance %s" CRLF,
        ENDISABLED_STRING(options->scaleInv));
      print( mnuTempText );
      break;

    //Toggle optical zoom
    case '6':
      //Toggle option
      options->opticalZoom = !options->opticalZoom;
      //Create output notification
      sprintf( mnuTempText, "Optical Zooming %s" CRLF,
        ENDISABLED_STRING(options->opticalZoom));
      print( mnuTempText );
      break;

    //Toggle digital zoom
    case '7':
      //Toggle option
      options->digitalZoom = !options->digitalZoom;
      //Create output notification
      sprintf( mnuTempText, "Digital Zooming %s" CRLF,
        ENDISABLED_STRING(options->digitalZoom));
      print( mnuTempText );
      break;

    //Toggle digital fixation
    case '8':
      //Toggle option
      options->fixateDigital = !options->fixateDigital;
      //Create output notification
      sprintf( mnuTempText, "Digital Fixation %s" CRLF,
        ENDISABLED_STRING(options->fixateDigital));
      print( mnuTempText );
      break;

    //Select Fixation Algorithm
    case '9':
      //Go to select algorithm menu
      currentMenu = &menuFixationAlg;
      currentMenu->display( this );
      break;
      
    //Select Zoom params
    case '0':
      //Go to select algorithm menu
      currentMenu = &menuZoomAlg;
      currentMenu->display( this );
      break;
      
    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuTracking::displayOptions
 *****************************************************************
 */
 void MenuTracking::displayOptions()
 {
   trackerTypeToString.convert(options->trackingTrkr);
   trackerColorTypeToString.convert(options->trackingTrkrColor);
   //Create output notification
   sprintf( mnuTempText,
     "Tracking:"          CRLF
     "   type:                 %s" CRLF
     "   color:                %s" CRLF
     "   Tracking:             %s" CRLF
     "   Fixation:             %s" CRLF
     "   Scale Invariance:     %s" CRLF
     "   Optical Zooming:      %s" CRLF
     "   Digital Zooming:      %s" CRLF
     "   Digital Fixation:     %s" CRLF,
     trackerTypeToString.conString,
     trackerColorTypeToString.conString,
     ENDISABLED_STRING(options->track),
     ENDISABLED_STRING(options->fixate),
     ENDISABLED_STRING(options->scaleInv),
     ENDISABLED_STRING(options->opticalZoom),
     ENDISABLED_STRING(options->digitalZoom),
     ENDISABLED_STRING(options->fixateDigital));
   print( mnuTempText );
   menuFixationAlg.displayOptions();
   menuZoomAlg.displayOptions();
 }

const CHAR textInMenuFixationAlg[] =
"       Fixation Algorithm Selection Menu"           CRLF
"       ---------------------------------"           CRLF
"Press"                                        CRLF
"1:     P Controller"                          CRLF
"2:     Kalman Filter w/ Tordoff"              CRLF
"3:     Choose Kalman process noise (Q_k)"     CRLF
"4:     Choose Kalman measurement noise (R_k)" CRLF
"5:     Choose fixation P gain"                CRLF;
/*****************************************************************
 * MenuFixationAlg ctor
 *****************************************************************
 */
MenuFixationAlg::MenuFixationAlg() :
  Menu(textInMenuFixationAlg)
{
}

/*****************************************************************
 * MenuFixationAlg::performAction
 *****************************************************************
 */
Menu *MenuFixationAlg::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Select P Controller
    case '1':
      options->fixationAlg = FIX_P;
      fixationTypeToString.convert( options->fixationAlg );
      sprintf(  mnuTempText, "   New fixation algorithm: %s",
        fixationTypeToString.conString );
      print( mnuTempText );      
      break;
    
    //Select Kalman Controller
    case '2':
      options->fixationAlg = FIX_KALMAN;
      fixationTypeToString.convert( options->fixationAlg );
      sprintf(  mnuTempText, "   New fixation algorithm: %s",
        fixationTypeToString.conString );
      print( mnuTempText );      
      break;  
      
    //Enter process noise
    case '3':
      options->kalmanProcessNoise = readFLOAT();
      sprintf(  mnuTempText, "   New process noise: %f",
            options->kalmanProcessNoise);
      print( mnuTempText );
      break;
    
    //Enter process noise
    case '4':
      options->kalmanMeasurementNoise = readFLOAT();
      sprintf(  mnuTempText, "   New measurement noise: %f",
            options->kalmanMeasurementNoise);
      print( mnuTempText );
      break;  
      
    //Enter P Gain
    case '5':
      options->fixationGain = readFLOAT();
      sprintf(  mnuTempText, "   New fixation P gain: %f",
            options->fixationGain);
      print( mnuTempText );
      break;  
             
    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuFixationAlg::displayOptions
 *****************************************************************
 */
 void MenuFixationAlg::displayOptions()
 {
   fixationTypeToString.convert( options->fixationAlg );
   //Create output notification
   sprintf( mnuTempText,
     "   Fixation:"          CRLF
     "      type:              %s" CRLF
     "      process noise:     %f" CRLF
     "      measurement noise: %f" CRLF
     "      fixation P gain:   %f" CRLF,
     fixationTypeToString.conString,
     options->kalmanProcessNoise,
     options->kalmanMeasurementNoise,
     options->fixationGain);
   print( mnuTempText );
 }

const CHAR textInMenuZoomAlg[] =
"       Zoom Control Algorithm Parameters Menu"CRLF
"       --------------------------------------"CRLF
"Press"                                        CRLF
"1:     Choose SI psi"                         CRLF
"2:     Choose SI gamma1"                      CRLF
"3:     Choose SI gamma2"                      CRLF
"4:     Choose Zoom memory coeff"              CRLF
"5:     Choose Zoom delay coeff"               CRLF
"6:     Choose Optical target object area"     CRLF
"7:     Choose Digital target object area"     CRLF
"8:     Toggle erring on zoom in / out"        CRLF;
/*****************************************************************
 * MenuZoomAlg ctor
 *****************************************************************
 */
MenuZoomAlg::MenuZoomAlg() :
  Menu(textInMenuZoomAlg)
{
}

/*****************************************************************
 * MenuZoomAlg::performAction
 *****************************************************************
 */
Menu *MenuZoomAlg::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {     
    //Enter Psi
    case '1':
      options->tordoffPsi = readFLOAT();
      sprintf(  mnuTempText, "   New SI psi: %f",
            options->tordoffPsi);
      print( mnuTempText );
      break;
    
    //Enter Gamma 1
    case '2':
      options->tordoffGamma1 = readFLOAT();
      sprintf(  mnuTempText, "   New SI gamma1: %f",
            options->tordoffGamma1);
      print( mnuTempText );
      break;  
    
    //Enter Gamma 2
    case '3':
      options->tordoffGamma2 = readFLOAT();
      sprintf(  mnuTempText, "   New SI gamma2: %f",
            options->tordoffGamma2);
      print( mnuTempText );
      break;  
    
    //Enter zoom coeff
    case '4':
      options->zoomCoeff = readFLOAT();
      sprintf(  mnuTempText, "   New zoom coefficient: %f",
            options->zoomCoeff);
      print( mnuTempText );
      break;  
      
    //Enter zoom delay coeff
    case '5':
      options->zoomDelayCoeff = readFLOAT();
      sprintf(  mnuTempText, "   New zoom delay coefficient: %f",
            options->zoomDelayCoeff);
      print( mnuTempText );
      break;  
      
    //Choose Optical target area
    case '6':
      options->targetAreaOpticalZoom = readUINT32();
      sprintf(  mnuTempText, "   New optical target area: %u",
            options->targetAreaOpticalZoom);
      print( mnuTempText );
      break;  
    
    //Choose Digital target area
    case '7':
      options->targetAreaDigitalZoom = readUINT32();
      sprintf(  mnuTempText, "   New digital target area: %u",
            options->targetAreaDigitalZoom);
      print( mnuTempText );
      break;  
      
    //Toggle digital fixation
    case '8':
      //Toggle option
      options->tordoffErrToHigherZoom = !options->tordoffErrToHigherZoom;
      //Create output notification
      sprintf( mnuTempText, "Err on zoom in %s" CRLF,
        ENDISABLED_STRING(options->tordoffErrToHigherZoom));
      print( mnuTempText );
      break;

                   
    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuZoomAlg::displayOptions
 *****************************************************************
 */
 void MenuZoomAlg::displayOptions()
 {
   fixationTypeToString.convert( options->fixationAlg );
   //Create output notification
   sprintf( mnuTempText,
     "   Zoom:"                    CRLF
     "      psi:               %f" CRLF
     "      gamma1:            %f" CRLF
     "      gamma2:            %f" CRLF
     "      zoom coeff:        %f" CRLF
     "      zoom delay coeff:  %f" CRLF
     "      opt target area:   %u" CRLF
     "      dig target area:   %u" CRLF
     "      err on zoom in:    %s" CRLF,
     options->tordoffPsi,
     options->tordoffGamma1,
     options->tordoffGamma2,
     options->zoomCoeff,
     options->zoomDelayCoeff,
     options->targetAreaOpticalZoom,
     options->targetAreaDigitalZoom,
     ENDISABLED_STRING(options->tordoffErrToHigherZoom));
   print( mnuTempText );
 }
 

CHAR textInMenuTrackerType[MNU_BUFFER_LENGTH];
/*****************************************************************
 * MenuTrackerType ctor
 *****************************************************************
 */
MenuTrackerType::MenuTrackerType() :
  Menu(textInMenuTrackerType)
{
  //Generate the menu based on the names of the TrackerType
  strcpy( textInMenuTrackerType,
    "       Tracker Type Menu" CRLF
    "       -----------------" CRLF
    "Press" CRLF);
  for( TrackerType ttCounter = TRKR_FIRST;
       ttCounter <= TRKR_LAST;
       ttCounter = (TrackerType)((UINT8)ttCounter+1 ))
  {
    trackerTypeToString.convert( ttCounter );
    sprintf( textInMenuTrackerType,
      "%s%d:     %s" CRLF,
      textInMenuTrackerType, (UINT8)ttCounter,
      trackerTypeToString.conString);
  }

  //Set to default
  ttToChange = NULL;
}

/*****************************************************************
 * MenuTrackerType::performAction
 *****************************************************************
 */
Menu *MenuTrackerType::performAction( CHAR command )
{
  Menu *currentMenu = this;
  //Convert from ASCII to TrackerType
  UINT8 commandInt = command - (UINT8)'0' - (UINT8)TRKR_FIRST;

  if( ttToChange == NULL )
  {
    Menu::print( "Incorrect implementation of MenuTrackerType" CRLF
                 "Contact developer." CRLF);
  }
  else
  {
    //Make sure it is a legal input
    if( commandInt <= (UINT8) TRKR_LAST )
    {
      //Set the new vidSrc
      *ttToChange = (TrackerType)commandInt;
      //Output the new value
      trackerTypeToString.convert( *ttToChange );
      sprintf(  mnuTempText, "   %s",
        trackerTypeToString.conString );
      print( mnuTempText );

      //Go to previous menu since option selected
      if( parentMenu != NULL )
      {
        currentMenu = parentMenu;
      }
    }
    else
    {
      currentMenu = handleCommon( command );
    }
  }
  return currentMenu;
}

/*****************************************************************
 * MenuTrackerType::setTrackerType
 *****************************************************************
 */
void MenuTrackerType::setTrackerType( TrackerType *tt )
{
  ttToChange = tt;
}

CHAR textInMenuTrackerColorType[MNU_BUFFER_LENGTH];
/*****************************************************************
 * MenuTrackerColorType ctor
 *****************************************************************
 */
MenuTrackerColorType::MenuTrackerColorType() :
  Menu(textInMenuTrackerColorType)
{
  //Generate the menu based on the names of the TrackerType
  strcpy( textInMenuTrackerColorType,
    "       Tracker Color Menu" CRLF
    "       ------------------" CRLF
    "Press" CRLF);
  for( TrackerColorType ttCounter = TRKR_COLOR_FIRST;
       ttCounter <= TRKR_COLOR_LAST;
       ttCounter = (TrackerColorType)((UINT8)ttCounter+1 ))
  {
    trackerColorTypeToString.convert( ttCounter );
    sprintf( textInMenuTrackerColorType,
      "%s%d:     %s" CRLF,
      textInMenuTrackerColorType, (UINT8)ttCounter,
      trackerColorTypeToString.conString);
  }

  //Set to default
  ttToChange = NULL;
}

/*****************************************************************
 * MenuTrackerColorType::performAction
 *****************************************************************
 */
Menu *MenuTrackerColorType::performAction( CHAR command )
{
  Menu *currentMenu = this;
  //Convert from ASCII to TrackerType
  UINT8 commandInt = command - (UINT8)'0' - (UINT8)TRKR_COLOR_FIRST;

  if( ttToChange == NULL )
  {
    Menu::print( "Incorrect implementation of MenuTrackerColorType" CRLF
                 "Contact developer." CRLF);
  }
  else
  {
    //Make sure it is a legal input
    if( commandInt <= (UINT8) TRKR_COLOR_LAST )
    {
      //Set the new vidSrc
      *ttToChange = (TrackerColorType)commandInt;
      //Output the new value
      trackerColorTypeToString.convert( *ttToChange );
      sprintf(  mnuTempText, "   %s",
        trackerColorTypeToString.conString );
      print( mnuTempText );

      //Go to previous menu since option selected
      if( parentMenu != NULL )
      {
        currentMenu = parentMenu;
      }
    }
    else
    {
      currentMenu = handleCommon( command );
    }
  }
  return currentMenu;
}

/*****************************************************************
 * MenuTrackerColorType::setTrackerType
 *****************************************************************
 */
void MenuTrackerColorType::setTrackerColorType( TrackerColorType *tt )
{
  ttToChange = tt;
}


const CHAR textInMenuMFS[] =
"       Measure Final Size Menu"           CRLF
"       -----------------------"           CRLF
"Press"                          CRLF
"1:     Select Tracker"          CRLF
"2:     Select Tracker Color"      CRLF
"3:     Toggle MFS logging"      CRLF
"4:     Select Log directory"    CRLF
"5:     Select Log file"         CRLF;
/*****************************************************************
 * MenuMFS ctor
 *****************************************************************
 */
MenuMFS::MenuMFS() :
  Menu(textInMenuMFS)
{
}

/*****************************************************************
 * MenuMFS::performAction
 *****************************************************************
 */
Menu *MenuMFS::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Go to select tracker options
    case '1':
      currentMenu = &menuTrackerType;
      menuTrackerType.setTrackerType(&options->mfsTrkr);
      currentMenu->display( this );
      break;

    //Go to select tracker options
    case '2':
      currentMenu = &menuTrackerColorType;
      menuTrackerColorType.setTrackerColorType(&options->mfsTrkrColor);
      currentMenu->display( this );
      break;      
      
    //Toggle MFS
    case '3':
      //Toggle option
      options->mfs = !options->mfs;
      //Create output notification
      sprintf( mnuTempText, "Measure final size %s" CRLF,
        ENDISABLED_STRING(options->mfs));
      print( mnuTempText );
      break;

    //Select log directory
    case '4':
      Menu::print("Enter new directory: ");
      strcpy( options->mfsDirectory, readLine() );
      sprintf(  mnuTempText, "   New directory: %s",
            options->mfsDirectory );
      print( mnuTempText );
      break;

    //Select log file
    case '5':
      Menu::print("Enter new file: ");
      strcpy( options->mfsFile, readLine() );
      sprintf(  mnuTempText, "   New file: %s",
            options->mfsFile );
      print( mnuTempText );
      break;

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuMFS::displayOptions
 *****************************************************************
 */
 void MenuMFS::displayOptions()
 {
   //Get a conversion for the trackerType enum
   trackerTypeToString.convert(options->mfsTrkr);
   trackerColorTypeToString.convert(options->mfsTrkrColor);
   //Create output notification
   sprintf( mnuTempText,
     "Measure final size:"          CRLF
     "   type:      %s"      CRLF
     "   color:     %s"      CRLF
     "   logging:   %s"      CRLF
     "   directory: %s"      CRLF
     "   file:      %s"      CRLF,
     trackerTypeToString.conString,
     trackerColorTypeToString.conString,
     ENDISABLED_STRING(options->mfs),
     options->mfsDirectory,
     options->mfsFile);
   print( mnuTempText );
 }

const CHAR textInMenuPTZ[] =
"       Pan/Tilt/Zoom Menu"           CRLF
"       ------------------"           CRLF
"Press"                          CRLF
"1:     Pan"                     CRLF
"2:     Tilt"                    CRLF
"3:     Zoom"                    CRLF;
/*****************************************************************
 * MenuPTZ ctor
 *****************************************************************
 */
MenuPTZ::MenuPTZ() :
  Menu(textInMenuPTZ)
{
}

/*****************************************************************
 * MenuPTZ::performAction
 *****************************************************************
 */
Menu *MenuPTZ::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Go to select pan menu
    case '1':
      currentMenu = &menuPan;
      currentMenu->display( this );
      break;

    //Go to select pan menu
    case '2':
      currentMenu = &menuTilt;
      currentMenu->display( this );
      break;

    //Go to select pan menu
    case '3':
      currentMenu = &menuZoom;
      currentMenu->display( this );
      break;

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuPTZ::displayOptions
 *****************************************************************
 */
 void MenuPTZ::displayOptions()
 {
   menuPan.displayOptions();
   menuTilt.displayOptions();
   menuZoom.displayOptions();
 }

const CHAR textInMenuPan[] =
"       Pan Menu"           CRLF
"       --------"           CRLF
"Press"                          CRLF
"1:     Set primary position"    CRLF
"2:     Set secondary position"  CRLF
"3:     Set pulse iterations"    CRLF
"4:     Do step"                 CRLF
"5:     Do pulse"                CRLF
"6:     Toggle pan angle inquiry"CRLF;
/*****************************************************************
 * MenuPan ctor
 *****************************************************************
 */
MenuPan::MenuPan() :
  Menu(textInMenuPan)
{
}

/*****************************************************************
 * MenuPan::performAction
 *****************************************************************
 */
Menu *MenuPan::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Set primary position
    case '1':
      Menu::print("Enter new primary position: ");
      options->panPos1 = readDOUBLE();
      sprintf(  mnuTempText, "   New primary position: %3.2f",
            options->panPos1 );
      print( mnuTempText );
      break;

    //Set secondary position
    case '2':
      Menu::print("Enter new secondary position: ");
      options->panPos2 = readDOUBLE();
      sprintf(  mnuTempText, "   New secondary position: %3.2f",
            options->panPos2 );
      print( mnuTempText );
      break;

    //Set pulse iterations
    case '3':
      Menu::print("Enter pulse iterations: ");
      options->panIterations = readUINT32();
      sprintf(  mnuTempText, "   New pulse iterations: %u",
            options->panIterations );
      print( mnuTempText );
      break;

    //Do step
    case '4':
      siKernel->panTiltStep();
      break;
      
    //Do pulse
    case '5':
      siKernel->panTiltPulse();
      break;
        
    //Get position
    case '6':
      //Toggle option
      options->panInquiry = !options->panInquiry;
      //Create output notification
      sprintf( mnuTempText, "Pan Inquiry %s" CRLF,
        ENDISABLED_STRING(options->panInquiry));
      print( mnuTempText ); 
      break;

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuPan::displayOptions
 *****************************************************************
 */
 void MenuPan::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Pan:"          CRLF
     "   Primary Pos:        %3.2f"     CRLF
     "   Secondary Pos:      %3.2f"     CRLF
     "   Pulse Iterations:   %u"        CRLF
     "   Pan Inquiry:        %s"        CRLF,
     options->panPos1, options->panPos2,
     options->panIterations, 
     ENDISABLED_STRING(options->panInquiry));
   print( mnuTempText );
 }

const CHAR textInMenuTilt[] =
"       Tilt Menu"           CRLF
"       ---------"           CRLF
"Press"                          CRLF
"1:     Set primary position"    CRLF
"2:     Set secondary position"  CRLF
"3:     Set pulse iterations"    CRLF
"4:     Do step"                 CRLF
"5:     Do pulse"                CRLF
"6:     Toggle tilt angle inquiry"CRLF;
/*****************************************************************
 * MenuTilt ctor
 *****************************************************************
 */
MenuTilt::MenuTilt() :
  Menu(textInMenuTilt)
{
}

/*****************************************************************
 * MenuTilt::performAction
 *****************************************************************
 */
Menu *MenuTilt::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Set primary position
    case '1':
      Menu::print("Enter new primary position: ");
      options->tiltPos1 = readDOUBLE();
      sprintf(  mnuTempText, "   New primary position: %3.2f",
            options->tiltPos1 );
      print( mnuTempText );
      break;

    //Set secondary position
    case '2':
      Menu::print("Enter new secondary position: ");
      options->tiltPos2 = readDOUBLE();
      sprintf(  mnuTempText, "   New secondary position: %3.2f",
            options->tiltPos2 );
      print( mnuTempText );
      break;

    //Set pulse iterations
    case '3':
      Menu::print("Enter pulse iterations: ");
      options->tiltIterations = readUINT32();
      sprintf(  mnuTempText, "   New pulse iterations: %u",
            options->tiltIterations );
      print( mnuTempText );
      break;

    //Do step
    case '4':
      siKernel->panTiltStep();
      break;
      
    //Do pulse
    case '5':
      siKernel->panTiltPulse();
      break;
            
    //Get position
    case '6':
      //Toggle option
      options->tiltInquiry = !options->tiltInquiry;
      //Create output notification
      sprintf( mnuTempText, "Tilt Inquiry %s" CRLF,
        ENDISABLED_STRING(options->tiltInquiry));
      print( mnuTempText ); 
      break;
      
    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuTilt::displayOptions
 *****************************************************************
 */
 void MenuTilt::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Tilt:"          CRLF
     "   Primary Pos:        %3.2f"     CRLF
     "   Secondary Pos:      %3.2f"     CRLF
     "   Pulse Iterations:   %u"        CRLF
     "   Tilt Inquiry:        %s"        CRLF,     
     options->tiltPos1, options->tiltPos2,
     options->tiltIterations,
     ENDISABLED_STRING(options->tiltInquiry));
   print( mnuTempText );
 }

const CHAR textInMenuZoom[] =
"       Zoom Menu"           CRLF
"       --------"           CRLF
"Press"                          CRLF
"1:     Set primary position"    CRLF
"2:     Set secondary position"  CRLF
"3:     Set pulse iterations"    CRLF
"4:     Do step"                 CRLF
"5:     Do pulse"                CRLF
"6:     Toggle zoom magnification inquiry"     CRLF
"7:     Toggle optical zoom"     CRLF
"8:     Toggle digital zoom"     CRLF
"9:     Toggle splicing on zoom out"  CRLF
"0:     Calibrate optical zoom"     CRLF;
/*****************************************************************
 * MenuZoom ctor
 *****************************************************************
 */
MenuZoom::MenuZoom() :
  Menu(textInMenuZoom)
{
}

/*****************************************************************
 * MenuZoom::performAction
 *****************************************************************
 */
Menu *MenuZoom::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Set primary position
    case '1':
      Menu::print("Enter new primary position: ");
      options->zoomPos1 = readDOUBLE();
      sprintf(  mnuTempText, "   New primary position: %3.2f",
            options->zoomPos1 );
      print( mnuTempText );
      break;

    //Set secondary position
    case '2':
      Menu::print("Enter new secondary position: ");
      options->zoomPos2 = readDOUBLE();
      sprintf(  mnuTempText, "   New secondary position: %3.2f",
            options->zoomPos2 );
      print( mnuTempText );
      break;

    //Set pulse iterations
    case '3':
      Menu::print("Enter pulse iterations: ");
      options->zoomIterations = readUINT32();
      sprintf(  mnuTempText, "   New pulse iterations: %u",
            options->zoomIterations );
      print( mnuTempText );
      break;

    //Do step
    case '4':
      siKernel->zoomStep();
      break;
      
    //Do pulse
    case '5':
      siKernel->zoomPulse();
      break;
      
    //Get position
    case '6':
      //Toggle option
      options->zoomInquiry = !options->zoomInquiry;
      //Create output notification
      sprintf( mnuTempText, "Zoom Inquiry %s" CRLF,
        ENDISABLED_STRING(options->zoomInquiry));
      print( mnuTempText ); 
      break;

    //Toggle optical zoom
    case '7':
      //Toggle option
      options->opticalZoom = !options->opticalZoom;
      //Create output notification
      sprintf( mnuTempText, "Optical zoom %s" CRLF,
        ENDISABLED_STRING(options->opticalZoom));
      print( mnuTempText );
      break;

    //Toggle digital zoom
    case '8':
      //Toggle option
      options->digitalZoom = !options->digitalZoom;
      //Create output notification
      sprintf( mnuTempText, "Digital zoom %s" CRLF,
        ENDISABLED_STRING(options->digitalZoom));
      print( mnuTempText );
      break;

    //Toggle splicing on zoom out
    case '9':
      //Toggle option
      options->spliceZoomOut = !options->spliceZoomOut;
      //Create output notification
      sprintf( mnuTempText, "Splicing of Zoom Out %s" CRLF,
        ENDISABLED_STRING(options->spliceZoomOut));
      print( mnuTempText );
      break;

    case '0':
      currentMenu = &menuCalibrateZoom;
      currentMenu->display( this );
      break;

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuZoom::displayOptions
 *****************************************************************
 */
 void MenuZoom::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Zoom:"          CRLF
     "   Primary Pos:        %3.2f"     CRLF
     "   Secondary Pos:      %3.2f"     CRLF
     "   Pulse Iterations:   %u"        CRLF
     "   Zoom Inquiry:       %s"        CRLF
     "   Optical zoom:       %s"        CRLF
     "   Digital zoom:       %s"        CRLF
     "   Splice zoom out:    %s"        CRLF,
     options->zoomPos1, options->zoomPos2,
     options->zoomIterations,
     ENDISABLED_STRING(options->zoomInquiry),
     ENDISABLED_STRING(options->opticalZoom),
     ENDISABLED_STRING(options->digitalZoom),
     ENDISABLED_STRING(options->spliceZoomOut));
   print( mnuTempText );
   menuCalibrateZoom.displayOptions();
 }


const CHAR textInMenuCalibrateZoom[] =
"       Calibrate Zoom Menu"           CRLF
"       -------------------"           CRLF
"Press"                          CRLF
"1:     Run calibration"         CRLF
"2:     Set lookup table length" CRLF
"3:     Load lookup table"       CRLF
"4:     Save lookup table"       CRLF
"5:     Set directory"           CRLF
"6:     Set file"                CRLF;
/*****************************************************************
 * MenuCalibrateZoom ctor
 *****************************************************************
 */
MenuCalibrateZoom::MenuCalibrateZoom() :
  Menu(textInMenuCalibrateZoom)
{
}

/*****************************************************************
 * MenuCalibrateZoom::performAction
 *****************************************************************
 */
Menu *MenuCalibrateZoom::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Run calibration
    case '1':
      siKernel->startCalibration();
      break;

    //Set table length
    case '2':
      Menu::print("Enter table length: ");
      options->calTableLength = readUINT16();
      sprintf(  mnuTempText, "   New table length: %u",
            options->calTableLength );
      print( mnuTempText );
      break;

    //Load table
    case '3':
       siKernel->loadZoomCalibrationTable();
       break;
    //Save table
    case '4':
       siKernel->saveZoomCalibrationTable();
       break;

    //Change directory
    case '5':
      Menu::print("Enter new directory: ");
      strcpy( options->calDirectory, readLine() );
      sprintf(  mnuTempText, "   New directory: %s",
            options->calDirectory );
      print( mnuTempText );
      break;

    //Change file
    case '6':
      Menu::print("Enter new file: ");
      strcpy( options->calFile, readLine() );
      sprintf(  mnuTempText, "   New file: %s",
            options->calFile );
      print( mnuTempText );
      break;

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuCalibrateZoom::displayOptions
 *****************************************************************
 */
 void MenuCalibrateZoom::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Calibrate zoom:"          CRLF
     "   Table length:       %u"        CRLF
     "   Directory:          %s"        CRLF
     "   File:               %s"        CRLF,
     options->calTableLength,
     options->calDirectory, options->calFile);
   print( mnuTempText );
 }

const CHAR textInMenuLoadSaveConfiguration[] =
"       Load/Save Configuration Menu"           CRLF
"       ----------------------------"           CRLF
"Press"                          CRLF
"1:     Load config"             CRLF
"2:     Save config"             CRLF
"3:     Set directory"           CRLF
"4:     Set file"                CRLF;
/*****************************************************************
 * MenuLoadSaveConfiguration ctor
 *****************************************************************
 */
MenuLoadSaveConfiguration::MenuLoadSaveConfiguration() :
  Menu(textInMenuLoadSaveConfiguration)
{
}

/*****************************************************************
 * MenuLoadSaveConfiguration::performAction
 *****************************************************************
 */
Menu *MenuLoadSaveConfiguration::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Load
    case '1':
      sprintf( mnuTempText, "%s%s",
        options->conDirectory, options->conFile );
      options->loadOptions(mnuTempText);
      break;

    //Save
    case '2':
      sprintf( mnuTempText, "%s%s",
        options->conDirectory, options->conFile );
      options->saveOptions(mnuTempText);
      break;

    //Change directory
    case '3':
      Menu::print("Enter new directory: ");
      strcpy( options->conDirectory, readLine() );
      sprintf(  mnuTempText, "   New directory: %s",
            options->conDirectory );
      print( mnuTempText );
      break;

    //Change file
    case '4':
      Menu::print("Enter new file: ");
      strcpy( options->conFile, readLine() );
      sprintf(  mnuTempText, "   New file: %s",
            options->conFile );
      print( mnuTempText );
      break;

    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuLoadSaveConfiguration::displayOptions
 *****************************************************************
 */
 void MenuLoadSaveConfiguration::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Load/Save Configuration:"         CRLF
     "   Directory:          %s"        CRLF
     "   File:               %s"        CRLF,
     options->conDirectory, options->conFile);
   print( mnuTempText );
 }

const CHAR textInMenuHzo[] =
"       Hybrid Zoom Out Menu"           CRLF
"       ----------------------------"           CRLF
"Press"                             CRLF
"1:     Toggle control arbitration" CRLF
"2:     Toggle display arbitration" CRLF
"3:     Configure cameras for HZO"  CRLF
"4:     Toggle statistic logging"   CRLF
"5:     Enable stat and video logging" CRLF
"6:     Set minimum focal length for zooming cam" CRLF;
/*****************************************************************
 * MenuHzo ctor
 *****************************************************************
 */
MenuHzo::MenuHzo() :
  Menu(textInMenuHzo)
{
}

/*****************************************************************
 * MenuHzo::performAction
 *****************************************************************
 */
Menu *MenuHzo::performAction( CHAR command )
{
  Menu *currentMenu = this;
  switch( command )
  {
    //Toggle control arbitration
    case '1':
      //Toggle option
      siHzo.options.arbitrateControl = !siHzo.options.arbitrateControl;
      //Create output notification
      sprintf( mnuTempText, "Control arbitration %s" CRLF,
        ENDISABLED_STRING(siHzo.options.arbitrateControl));
      print( mnuTempText );
      break;

    //Toggle display arbitration
    case '2':
      //Toggle option
      siHzo.options.arbitrateDisplay = !siHzo.options.arbitrateDisplay;
      //Create output notification
      sprintf( mnuTempText, "Display arbitration %s" CRLF,
        ENDISABLED_STRING(siHzo.options.arbitrateDisplay));
      print( mnuTempText );
      break;
      
    case '3':
      siHzo.setupCameras();
      break;
      
    //Toggle statistic logging
    case '4':
      //Toggle option
      siHzo.options.saveStats = !siHzo.options.saveStats;
      //Create output notification
      sprintf( mnuTempText, "Stat logging %s" CRLF,
        ENDISABLED_STRING(siHzo.options.saveStats ));
      print( mnuTempText );
      break;
    
    //Enable all logging
    case '5':
      siHzo.options.saveStats = true;
      siHzo.options.vidDstSaveOrigStream = true;
      siHzo.options.vidDstSaveDigStream = true;
      siHzo.options.vidDstSaveSplicedStream = true;     
      break;

    //Set minimum magnification
    case '6':
      Menu::print("Enter new minimum magnification: ");
      siHzo.options.minZoomMagZoomCam = readDOUBLE();
      sprintf(  mnuTempText, "   New minimum magnification: %3.2f",
            siHzo.options.minZoomMagZoomCam );
      print( mnuTempText );
      break;  
          
    //Unrecognized command, so check with common commands
    default:
      currentMenu = handleCommon( command );
    break;
  }
  return currentMenu;
}

/*****************************************************************
 * MenuHzo::displayOptions
 *****************************************************************
 */
 void MenuHzo::displayOptions()
 {
   //Create output notification
   sprintf( mnuTempText,
     "Hybrid zoom out arbitration:"         CRLF
     "   control:          %s"        CRLF
     "   display:          %s"        CRLF
     "   stat logging:     %s"        CRLF
     "   min zoom mag:     %3.2f"     CRLF,
     ENDISABLED_STRING(siHzo.options.arbitrateControl),
     ENDISABLED_STRING(siHzo.options.arbitrateDisplay),
     ENDISABLED_STRING(siHzo.options.saveStats),
     siHzo.options.minZoomMagZoomCam);
   print( mnuTempText );
 }

 
 /*****************************************************************
 * VideoSrcTypeToString::convert
 *****************************************************************
 */
void VideoSrcTypeToString::convert( VideoSrcType vst )
{
  switch( vst )
  {
    case SRC_NONE:
      strcpy( conString, "SRC_NONE" );
      break;
    case SRC_FILE:
      strcpy( conString, "SRC_FILE" );
      break;
    case SRC_FRAME_GRABBER:
      strcpy( conString, "SRC_FRAME_GRABBER" );
      break;
    default:
      sprintf( conString, "Unknown VideoSrcType (%d)",
        (UINT8) vst );
      break;
  }
}


/*****************************************************************
 * VideoDestTypeToString::convert
 *****************************************************************
 */
void VideoDstTypeToString::convert( VideoDstType vdt )
{
  switch( vdt )
  {
    case DST_NONE:
      strcpy( conString, "DST_NONE" );
      break;
    case DST_FILE:
      strcpy( conString, "DST_FILE" );
      break;
    case DST_SCREEN:
      strcpy( conString, "DST_SCREEN" );
      break;
    default:
      sprintf( conString, "Unknown VideoDstType (%d)",
        (UINT8) vdt );
      break;
  }
}

/*****************************************************************
 * TrackerTypeToString::convert
 *****************************************************************
 */
void TrackerTypeToString::convert( TrackerType tt )
{
  switch( tt )
  {
    case TRKR_NONE:
      strcpy( conString, "TRKR_NONE" );
      break;
    case TRKR_CSEG:
      strcpy( conString, "TRKR_CSEG" );
      break;
    case TRKR_CAMSHIFT:
      strcpy( conString, "TRKR_CAMSHIFT" );
      break;
    default:
      sprintf( conString, "Unknown TrackerType (%d)",
        (UINT8) tt );
      break;
  }
}

/*****************************************************************
 * TrackerColorTypeToString::convert
 *****************************************************************
 */
void TrackerColorTypeToString::convert( TrackerColorType tt )
{
  switch( tt )
  {
    case TRKR_COLOR_RED:
      strcpy( conString, "TRKR_COLOR_RED" );
      break;
    case TRKR_COLOR_BLUE:
      strcpy( conString, "TRKR_COLOR_BLUE" );
      break;
    default:
      sprintf( conString, "Unknown TrackerColorType (%d)",
        (UINT8) tt );
      break;
  }
}

/*****************************************************************
 * FixationTypeToString::convert
 *****************************************************************
 */
void FixationTypeToString::convert( FixationType tt )
{
  switch( tt )
  {
    case FIX_P:
      strcpy( conString, "FIX_P" );
      break;
    case FIX_KALMAN:
      strcpy( conString, "FIX_KALMAN" );
      break;
    default:
      sprintf( conString, "Unknown FixationType (%d)",
        (UINT8) tt );
      break;
  }
}

// File: $Id: Menu.cpp,v 1.34 2005/09/12 23:40:20 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: Menu.cpp,v $
// Revision 1.34  2005/09/12 23:40:20  edn2065
// Renamed SIKernel and SIHzo to SISingleCam and SIDualCam
//
// Revision 1.33  2005/09/10 02:42:48  edn2065
// Added minimum zoom option. Added timer to apply in menu.
//
// Revision 1.32  2005/09/09 01:24:37  edn2065
// Added HZO logging
//
// Revision 1.31  2005/09/08 23:07:41  edn2065
// Fixed saving wrong stream bug
//
// Revision 1.30  2005/09/07 22:43:32  edn2065
// Added option to save spliced stream
//
// Revision 1.29  2005/09/07 18:44:04  edn2065
// Added options to make zoom more aggressive
//
// Revision 1.28  2005/09/06 01:57:12  edn2065
// Added zoom delay handling
//
// Revision 1.27  2005/09/06 00:08:00  edn2065
// Fixed digital zooming when optical zooming off. Added weighted averaging to zoom control
//
// Revision 1.26  2005/08/26 20:27:13  edn2065
// Pickoff points work. Implemented HZO camera setup
//
// Revision 1.25  2005/08/26 00:57:21  edn2065
// Added menu options for HZO. Setup control arbitration
//
// Revision 1.24  2005/08/25 23:55:59  edn2065
// Removed deadlock in SIDualCam and separated SISingleCams from GUI
//
// Revision 1.23  2005/08/25 22:12:23  edn2065
// Made commandLevel a class. Added HZO pickoff point in SISingleCam
//
// Revision 1.22  2005/08/24 20:47:36  edn2065
// Added switchable options
//
// Revision 1.21  2005/08/24 19:44:29  edn2065
// Added multiple windows by putting window handling in its own thread
//
// Revision 1.20  2005/08/16 02:16:38  edn2065
// Added menu options for selecting digital zoom type and for easy image type switching
//
// Revision 1.19  2005/08/03 02:34:54  edn2065
// Added tordoff psi and gammas to menu
//
// Revision 1.18  2005/07/29 00:11:14  edn2065
// Added menu options for Kalman filter and fixation gains
//
// Revision 1.17  2005/07/26 18:46:43  edn2065
// Added scale invariance
//
// Revision 1.16  2005/06/28 18:33:26  edn2065
// Completed addition of blue and red trackers
//
// Revision 1.15  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
// Revision 1.14  2005/06/10 11:55:51  edn2065
// Fixed reverseLookup error. Added reset option
//
// Revision 1.13  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.12  2005/06/08 15:08:00  edn2065
// Add P/T/Z inquiry
//
// Revision 1.11  2005/06/03 17:31:31  edn2065
// Added missing notImplemented() call
//
// Revision 1.10  2005/06/03 12:49:46  edn2065
// Changed Controller to SISingleCam
//
// Revision 1.9  2005/05/27 13:20:53  edn2065
// Added ability to change number of ROIs in output
//
// Revision 1.8  2005/05/26 19:15:02  edn2065
// Added VideoStream. tested. Still need to get tracker working
//
// Revision 1.7  2005/05/26 14:39:47  edn2065
// Made execute from Menu a static class variable.
//
// Revision 1.6  2005/05/25 15:58:46  edn2065
// Added option to prevent the horizontal line from appearing on the screen output
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

