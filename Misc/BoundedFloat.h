#ifndef BOUNDEDFLOAT_H
#define BOUNDEDFLOAT_H

/**************************************************************
 *
 * BoundedFloat class (abstract)
 * Description: A class that makes sure a value stays within given bounds
 *
 *
 **************************************************************
 */
class BoundedFloat {

  public:
   /******************************************
    * setValue
    * Description: Sets the value. Checks for bounds. If newValue is outside
    *              the bounds, it is set to the nearest bound. This should only
    *              be called AFTER the bounds are set.
    * Parameters:  newValue - the new value to be set
    ******************************************
    */
    void setValue( FLOAT newValue )
    {
      if( newValue > max )
      {
        value = max;
      }
      else if( newValue < min )
      {
        value = min;
      }
      else
      {
        value = newValue;
      }
    }
   
   /******************************************
    * getValue
    * Description: Returns the bounded value
    * Return:      The value
    ******************************************
    */     
    FLOAT getValue() 
    {
      return value;
    }

   /******************************************
    * setMax
    * Description: Sets the maximum bound without error checking.
    * Parameters:  newMax - the new maximum bound
    ******************************************
    */      
    void setMax( FLOAT newMax )
    {
      max = newMax;
    }
  
   /******************************************
    * setMax
    * Description: Sets the minimum bound without error checking.
    * Parameters:  newMin - the new maximum bound
    ******************************************
    */          
    void setMin( FLOAT newMin )
    {
      min = newMin;
    }

  private:
    //The bounded value
    FLOAT value;
    //The upper bound
    FLOAT max;
    //The lower bound
    FLOAT min;

};

#endif

/*
 * Author: Eric D Nelson
 * Version:
 *     $Id: BoundedFloat.h,v 1.2 2005/05/27 16:09:06 edn2065 Exp $
 *
 * Revisions:
 *     $Log: BoundedFloat.h,v $
 *     Revision 1.2  2005/05/27 16:09:06  edn2065
 *     Added accessor
 *
 *
 */
