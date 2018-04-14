#ifndef PARTIALBOX_H
#define PARTIALBOX_H

#include "types.h"
#include "cv.h"


/**************************************************************
 *
 * PartialBox class
 * Description: A class that takes a box centered around one location, then
 *              creates a "sub" box that is entirely contained within the original 
 *              but the center of the "sub" box is different than the original box's. The "sub"
 *              box has at least one common horizontal edge and one common vertical edge with the
 *              original box.
 *              When a second box that is similar to the original box is entered, a second
 *              "sub" box is generated that is similar to the original "sub" box. The ratio 
 *              of the size of the first "sub" box to the second "sub" box is the same as the
 *              ratio of the first original box to the second original box. 
 *              See notes 6/21 pp 1-3
 *
 **************************************************************
 */

class PartialBox {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults. 
    ******************************************
    */ 
    PartialBox();
    
   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */     
    ~PartialBox();
  
  
   /******************************************
    * setOriginalBox
    * Description: Sets the first original box. Calculates a "sub" box based upon 
    *              the coordinates of the origin sent in. Operation is undefined if
    *              the originalBox does not contain the origin.
    * Parameter:   originalBox - Passed in as the original box. The 'x' and 'y' coordinates
    *                            of this use the upper left corner of the image as the origin
    *              myOriginX   - Passed in as the x-coordinate of the center of the "sub" box
    *              myOriginY   - Passed in as the y-coordinate of the center of the "sub" box
    *              subBox      - Returned as the "sub" box centered at (originX, originY). 
    *                            The 'x' and 'y' coordinates of this use the upper left corner 
    *                            of the image as the origin
    ******************************************
    */     
    void setOriginalBox( CvRect *originalBox, SINT16 myOriginX, SINT16 myOriginY, 
      CvRect *subBox );
    
   /******************************************
    * getNewSubBox
    * Description: Sets the second original box. Calculates a "sub" box similar to the 
    *              original "sub" box, but with a different size. Operation is undefined
    *              if called before setOriginalBox.
    * Parameter:   originalBox2 - Passed in as the second original box. The 'x' and 'y' coordinates
    *                            of this use the upper left corner of the image as the origin
    *              subBox      - Returned as the second "sub" box. The 'x' and 'y' coordinates
    *                            of this use the upper left corner of the image as the origin
    *              originX     - Returned as the x-coordinate of the center of the 
    *                            second "sub" box
    *              originY     - Returned as the y-coordinate of the center of the 
    *                            second "sub" box
    ******************************************
    */     
    void getNewSubBox( CvRect *originalBox2, CvRect *subBox2, 
      SINT16 *originX2, SINT16 *originY2 );

private:  

  //The ratio of the width of the original box to the "sub" box
  FLOAT origToSubRatioWidth;
  //The ratio of the height of the original box to the "sub" box
  FLOAT origToSubRatioHeight;
    
  //True if the first original and "sub" boxes have a common left edge, false if
  //common right
  BOOL commonTop;
  //True if the first original and "sub" boxes have a common top edge, false if
  //common bottom
  BOOL commonLeft;

  //The new origin and center of the first 'sub' box.
  SINT16 originX;
  SINT16 originY;

};

#endif

// File: $Id: PartialBox.h,v 1.1 2005/07/14 20:19:08 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: PartialBox.h,v $
// Revision 1.1  2005/07/14 20:19:08  edn2065
// Adding PartialBox to repository
//
//
