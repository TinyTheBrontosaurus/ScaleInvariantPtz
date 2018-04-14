
#ifndef VIDEODESTINATION_H
#define VIDEODESTINATION_H

#include <string.h>
#include "cv.h"
#include "types.h"

#define VD_TEXT_LENGTH (100)

/**************************************************************
 *
 * VideoDestination class (abstract)
 * Description: An interface for outputting images into video.
 **************************************************************
 */
class VideoDestination {

  public:

    /******************************************
    * ctor
    * Description: Sets defaults
    ******************************************
    */
    VideoDestination();

    /******************************************
    * dtor
    ******************************************
    */
    ~VideoDestination();


    /******************************************
    * sendFrame
    * Description: Record the frame on whatever media available
    ******************************************
    */
    virtual void sendFrame() = 0;

    /******************************************
    * setText
    * Description: Set the text to be displayed in the frame
    * Parameters:  myText - the string to be displayed
    ******************************************
    */
    void setText( CHAR *myText );

    /******************************************
    * setBoxes
    * Description: Set the boxes to be displayed in the frame
    * Parameters:  myBoxes - the array of boxes
    *              myNumBoxes - size of the array
    ******************************************
    */
    void setBoxes( CvRect *myBoxes, UINT8 myNumBoxes );

    /******************************************
    * setImg
    * Description: Set the image to be displayed in the frame
    * Parameters:  myImg - the image to be displayed
    ******************************************
    */
    void setImg( IplImage *myImg );

  protected:
    /******************************************
    * prepareImage
    * Description: Superimpose text and boxes on frame
    ******************************************
    */
    void prepareImage();

  private:
    /******************************************
    * addText
    * Description: Superimpose text on frame
    ******************************************
    */
    void addText(IplImage *, CHAR *text, CvPoint);

    /******************************************
    * addRects
    * Description: Superimpose boxes on frame
    ******************************************
    */
    void addRects(IplImage *, CvRect[], UINT8);

  protected:
    //The text to go in the frame
    CHAR text[VD_TEXT_LENGTH];
    //An array of boxes in the frame
    CvRect *boxes;
    //Size of array of boxes
    UINT8 numBoxes;
    //The original image before text and boxes added
    IplImage *imgOrig;
    //The original image with text and boxes added
    IplImage *imgSuperimposed;
    //ffor color images
    IplImage *imgSuperimposedFullColor;
    //for images that are black and white
    IplImage *imgSuperimposedFullBW;
    //For density map-related images
    IplImage *imgSuperimposedSmallDM;
    //The final image to be displayed
    IplImage *imgToDisp;

  private:
    // true if boxes should be superimposed
    bool doBoxes;
    // true if text should be superimposed
    bool doText;
    CvFont      font;        // font for text
    CvScalar    color;       // color of text

};

#endif

// File: $Id: VideoDestination.h,v 1.4 2005/06/09 13:48:03 edn2065 Exp $
// Author: Eric D Nelson
// Description:
// Revisions:
// $Log: VideoDestination.h,v $
// Revision 1.4  2005/06/09 13:48:03  edn2065
// Fixed synchronization errors caused by not initializing Barrier in SIKernal
//
// Revision 1.3  2005/04/23 14:26:32  edn2065
// Commented Video classes
//
// Revision 1.2  2005/04/02 18:46:24  edn2065
// fixed comments.
//
// ----------------------------------------------------------------------
//
