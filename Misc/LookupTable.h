#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include "types.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#include <string>
using std::string;

/**************************************************************
 *
 * LookupTable class
 * Description: A lookup table that converts a one number to another. The table
 *             is expected to be in order such that x[i] < x[i+1] where i is
 *             an index of the array. Index "0" is represented by a minimum value
 *             while index "length-1" is represented by a maximum value. This is done
 *             in such a way that index "i" is represented by 
 *             (i)*(maximum-minimum)/(length)
 *             The lookup table can optionally generate a reverse lookup table.
 *
 **************************************************************
 */

class LookupTable {

  public:
   /******************************************
    * ctor
    * Description: Sets defaults. 
    ******************************************
    */ 
    LookupTable();
    
   /******************************************
    * dtor
    * Description: Releases memory
    ******************************************
    */     
    ~LookupTable();
  
  
   /******************************************
    * load
    * Description: Copy the table from an array.
    * Parameter:   myTable - the array of values
    *              myLength - the length of that table
    *              min - the minimum value represented by myTable[0]
    *              max - the maximum value represented by myTable[myLength-1]
    *              myLookupForward - Sets lookupForward
    ******************************************
    */     
    void load( const FLOAT *myTable, const UINT32 myLength, FLOAT min, 
      FLOAT max, BOOL myLookupForward );
               
   /******************************************
    * load
    * Description: Load a new table from file
    * Parameter:   fileName - the file's path
    ******************************************
    */                
    void load( const CHAR *fileName );
  
   /******************************************
    * lookup
    * Description: Does forwardLookup or backwardLookup depending on 
    *              doLookupForward
    * Parameter:   idx - the array index
    * Returns:     The interpolated value corresponding to the array's values
    ******************************************
    */   
    FLOAT lookup( FLOAT idx );

   /******************************************
    * lookupReverse
    * Description: Does forwardLookup or backwardLookup depending on 
    *              doLookupForward
    * Parameter:   val - the value in the array
    * Returns:     The interpolated value corresponding to the array index
    ******************************************
    */       
    FLOAT lookupReverse( FLOAT val );
    
   /******************************************
    * save
    * Description: Saves the lookup table to a file.
    * Parameter:   fileName - the file to which it is saved
    * Returns:     Returns 0 if successful.
    ******************************************
    */            
    UINT8 save( const CHAR *fileName );
    
   /******************************************
    * print
    * Description: Prints the values of the lookupTable to screen
    ******************************************
    */       
    void print();
    
   /******************************************
    * GetTable
    * Description: Accessor to object variables
    * Parameter:   myTable - returned as a pointer to the table
    *              myTableLength - Length of table copied to this variable
    *              myTableMin - Min of table copied to this variable
    *              myTableMax - Max of table copied to this variable
    ******************************************
    */    
    void getTable( FLOAT **myTable, UINT32 *myTableLength, FLOAT *myTableMin,
                             FLOAT *myTableMax); 
    
private:
   /******************************************
    * lInterpolation
    * Description: Linear interpolation. Returns y for a given x.
    * Parameters:  x1 - Given points (x1, y1) and (x2,y2) along with
    *              x2 - the value of x in point (x,y), the corresponding y
    *              y1 - is found.
    *              y2 -
    *              x  - 
    * Returns:     The y corresponding to x.
    ******************************************
    */      
    FLOAT lInterpolation(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, FLOAT x);

   /******************************************
    * forwardLookup
    * Description: Do a conversion from array index to value. The units returned are
    *              the units in the array.
    * Parameter:   idx - the array index
    * Returns:     The interpolated value corresponding to the array's values
    ******************************************
    */   
    FLOAT forwardLookup( FLOAT idx );       

   /******************************************
    * backwardLookup
    * Description: Do a conversion from value to array index. The units returned are
    *              the units of the index.
    * Parameter:   val - the value in the array
    * Returns:     The interpolated value corresponding to the array index
    ******************************************
    */           
    FLOAT backwardLookup( FLOAT val );
    
private:
  //The lookup table
  FLOAT table[LKP_DEFAULT_LENGTH];
  
  //The length of the table
  UINT32 tableLength;
  
  //The value represented by table[0]
  FLOAT min;
  
  //The value represented by table[tableLength-1]
  FLOAT max;
  
  //True if lookup() does a forward look and 
  //        lookupReverse() does a backward lookup.
  //False if lookup() does a backward look and 
  //         lookupReverse() does a forward lookup.
  BOOL doLookupForward;
      
};

#endif

// File: $Id: LookupTable.h,v 1.4 2005/06/02 18:11:42 edn2065 Exp $
// Author: Eric D Nelson
// Description: Creates a lookup table from int's to FLOATs.
// Revisions:
// $Log: LookupTable.h,v $
// Revision 1.4  2005/06/02 18:11:42  edn2065
// Tested ZoomOptical
//
// Revision 1.3  2005/06/02 13:55:41  edn2065
// Cleaned LookupTable and ran test program
//
// Revision 1.2  2005/06/01 19:49:29  edn2065
// Checking in for night
//
// Revision 1.1  2005/05/27 16:27:22  edn2065
// Added to repository from OldProject
//
// Revision 1.5  2005/03/11 21:28:10  edn2065
// Works without reverseTable
//
// Revision 1.4  2005/03/11 04:05:15  edn2065
// Reverse lookup table apparently works
//
// Revision 1.3  2005/03/11 01:08:34  edn2065
// Correctly creates the lookup table and saves it to a file. Does good
// error checking for file I/O.
//
// Revision 1.2  2005/03/11 00:00:41  edn2065
// skeleton
//
// Revision 1.1  2005/03/10 23:46:38  edn2065
// Initial revision
//
