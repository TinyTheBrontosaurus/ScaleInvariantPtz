#include "PartialBox.h"

/*****************************************************************
 * PartialBox ctor
 *****************************************************************
 */
PartialBox::PartialBox()
{
  //Do nothing
}

/*****************************************************************
 * PartialBox dtor
 *****************************************************************
 */
PartialBox::~PartialBox()
{
  //Do nothing
}

/*****************************************************************
 * PartialBox::setOriginalBox
 *****************************************************************
 */
void PartialBox::setOriginalBox( CvRect *originalBox, SINT16 myOriginX, 
  SINT16 myOriginY, CvRect *subBox )
{

  //The common X and Y edge between the first original and sub boxes.
  //The coordinates are changed so that (0,0) is at the origin passed in
  SINT16 xCommon1;
  SINT16 yCommon1;
  
  //The X and Y edge of the "sub" box that is not common with the 
  //original box
  //The coordinates are changed so that (0,0) is at the origin passed in
  SINT16 xOther1;
  SINT16 yOther1;
  
  //Save the passed in origin
  originX = myOriginX;
  originY = myOriginY;
  
  //Find which edges of the original box are closest to the origin.
  //"If the left edge is closer to the origin than the right edge"
  if( abs( originalBox->x - originX ) < 
      abs( originalBox->x + originalBox->width - originX ) )
  {
    //The left edge is common
    xCommon1 = originalBox->x - originX;
    commonLeft = true;
  }
  else
  {
    //The right edge is common
    xCommon1 = originalBox->x + originalBox->width - originX;
    commonLeft = false;
  }

  //"If the top edge is closer to the origin than the bottom edge"
  if( abs( originalBox->y - originY ) < 
      abs( originalBox->y + originalBox->height - originY ) )
  {
    //The top edge is common
    yCommon1 = originalBox->y - originY;
    commonTop = true;
  }
  else
  {
    //The bottom edge is common
    yCommon1 = originalBox->y + originalBox->height - originY;
    commonTop = false;
  }
  
  //Now calculate the other edges
  xOther1 = -xCommon1;
  yOther1 = -yCommon1;
  
  //Calculate height and width
  subBox->width  = abs(xCommon1 - xOther1);
  subBox->height = abs(yCommon1 - yOther1); 

  //Set the X and Y coordinates of "sub" box
  subBox->x = (commonLeft ? xCommon1 : xOther1) + originX;
  subBox->y = (commonTop  ? yCommon1 : yOther1) + originY;
  
  //Calculate the ratio of width and height between the original and "sub" boxes
  origToSubRatioWidth  = (FLOAT) originalBox->width  / subBox->width;
  origToSubRatioHeight = (FLOAT) originalBox->height / subBox->height;
  
} 

/*****************************************************************
 * PartialBox::getNewSubBox
 *****************************************************************
 */
void PartialBox::getNewSubBox( CvRect *originalBox2, CvRect *subBox2, 
      SINT16 *originX2, SINT16 *originY2 )
{
  //The common X and Y edge between the second original and sub boxes.
  //The coordinates are changed so that (0,0) is at the origin passed in
  SINT16 xCommon2;
  SINT16 yCommon2;
  
  //The X and Y edge of the "sub" box that is not common with the 
  //original box
  SINT16 xOther2;
  SINT16 yOther2;
  
  //Calculate new width and height
  subBox2->width = (SINT16) (originalBox2->width / origToSubRatioWidth);
  subBox2->height = (SINT16) (originalBox2->height / origToSubRatioHeight);
  
  //Calculate the left edge of the new box
  if( commonLeft )
  {
    subBox2->x = originalBox2->x;
  }
  else
  {
    subBox2->x = originalBox2->x + originalBox2->width - subBox2->width; 
  }

  //Calculate the top edge of the new box
  if( commonTop )
  {
    subBox2->y = originalBox2->y;
  }
  else
  {
    subBox2->y = originalBox2->y + originalBox2->height - subBox2->height; 
  }  
  
  //Now calculate the center of the new "sub" box w.r.t. the passed in origin
  *originX2 = subBox2->x + subBox2->width / 2 - originX;
  *originY2 = subBox2->y + subBox2->height / 2 - originY; 
  
} 

// File: $Id: PartialBox.cpp,v 1.1 2005/07/14 20:19:08 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: PartialBox.cpp,v $
// Revision 1.1  2005/07/14 20:19:08  edn2065
// Adding PartialBox to repository
//
//
