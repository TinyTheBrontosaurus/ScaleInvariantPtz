#include "LookupTable.h"

/*****************************************************************
 * LookupTable ctor
 *****************************************************************
 */
LookupTable::LookupTable()
{

}

/*****************************************************************
 * LookupTable dtor
 *****************************************************************
 */
LookupTable::~LookupTable()
{

}


/*****************************************************************
 * LookupTable::load
 *****************************************************************
 */
void LookupTable::load ( const FLOAT *myTable, UINT32 myLength, FLOAT myMin,
                          FLOAT myMax, BOOL myDoLookupForward ){

  //The previous value in the lookup table array during iteration.
  FLOAT lastValue;
  //The loop counter for copying
  UINT32 tablePos;
  
  //Copy the table parameters
  max = myMax;
  min = myMin;
  tableLength = myLength;
  doLookupForward = myDoLookupForward;
  
  if( min > max )
  {
    printf( "Warning, min(%f) > max(%f) (%s @ %d)"CRLF, 
      min, max, __FILE__, __LINE__ );
  }
  
  //Copy the table itself
  table[0] = myTable[0];
  lastValue = myTable[0];
  for( tablePos = 1; tablePos < tableLength; tablePos++ ) {
    //Copy element
    table[tablePos] = myTable[tablePos];
    
    // Make sure it's sorted and print an error if table is not sorted correctly.
    if( myTable[tablePos] <= lastValue ) {
      cerr << "LookupTable not sorted. table[" << tablePos - 1 << "] = "
           << lastValue << ", table[" << tablePos << "] = "
           << myTable[tablePos] << endl;
    }
    
    //Remember the previous value
    lastValue = myTable[tablePos];   
  }
 
}

/*****************************************************************
 * LookupTable::load
 *****************************************************************
 */
void LookupTable::load( const CHAR *fileName )
{
  //The file to be loaded  
  fstream file;
  //The current position in the table for copying
  UINT32 tablePos;
  //The previous value in the lookup table array during iteration.
  FLOAT lastValue;
    
  try {
    //Open the file
    file.open(fileName, ios::in);
    
    //Turn exceptions on
    file.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    
    // First get the table size from the file
    file >> tableLength >> max >> min >> doLookupForward;
    //Get the first data of the table
    file >> table[0];
    lastValue = table[0];    
    
    //Get the rest of the table from the file
    for( tablePos = 1; tablePos < tableLength; tablePos++ ) {
      file >> table[tablePos];
      // Print an error if table is not sorted correctly.
      if( table[tablePos] <= lastValue ) {
        cerr << "LookupTable not sorted. table[" << tablePos - 1 << "] = "
             << lastValue << ", table[" << tablePos << "] = "
             << table[tablePos] << endl;
      }
      
      //Remember the previous value
      lastValue = table[tablePos];
    }
    
    printf( "Lookup table successfully loaded from %s (%s @ %d)"CRLF, 
      fileName, __FILE__, __LINE__ );
    
  } catch( ifstream::failure e ) {
    printf( "Error in loading Lookup table from %s (%s @ %d)"CRLF, 
      fileName, __FILE__, __LINE__ );
    
  }
  
  //Close the file
  if( file.is_open() ) {
    file.close();
  }
  
}

/*****************************************************************
 * LookupTable::lookup
 *****************************************************************
 */
FLOAT LookupTable::lookup( FLOAT val )
{
  if( doLookupForward )
  {
    return forwardLookup(val);
  }
  else
  {
    return backwardLookup(val);
  }
}
/*****************************************************************
 * LookupTable::lookupReverse
 *****************************************************************
 */  
FLOAT LookupTable::lookupReverse( FLOAT idx )
{
  if( !doLookupForward )
  {
    return forwardLookup(idx);
  }
  else
  {
    return backwardLookup(idx);
  }
}
  
/*****************************************************************
 * LookupTable::forwardLookup
 *****************************************************************
 */
FLOAT LookupTable::forwardLookup( FLOAT val )
{

  if( val < min || val > max ) {
    cerr << "Bad value in LookupTable::forwardLookup: " << val << endl;
  }
  
  //Find what the index of the value would be if it were in the array
  FLOAT indexOfVal = ((val - min)/(max-min)*(tableLength-1));
  
  //The floor of that value  
  UINT32 flVal = (UINT32)indexOfVal;
  
  //Calculate the value to be returned
  return lInterpolation( flVal, flVal + 1, table[flVal],
                         table[flVal + 1], indexOfVal ) ;
}

/*****************************************************************
 * LookupTable::backwardLookup
 *****************************************************************
 */
FLOAT LookupTable::backwardLookup( FLOAT val )
{

  //Determined to be the position in the array closest to val while being
  //less than or equal to val
  UINT32 curPos;
  
  if( val < table[0] || val > table[tableLength-1] ) {
    cerr << "Backward lookup out of range: " << val << endl;
  }
  //Do linear search after which curPos will be the index within
  //the focal length lookup table corresponding to the value that is
  //the largest less than reading.
  for( curPos = 0; curPos < tableLength-1; curPos++ )
  {
    if( val < table[curPos+1] )
    {
      break;
    }
  }

  //Do interpolation to figure out value  
  return (curPos+((FLOAT)(val-table[curPos]))/
         ((FLOAT)(table[curPos+1] - table[curPos])))*(max-min)/
         (tableLength-1) + min;                    
  
}

/*****************************************************************
 * LookupTable::save
 *****************************************************************
 */
UINT8 LookupTable::save( const CHAR *fileName ){

  //Default to a successful save
  UINT8 retval = 0;
  
  //Create the file that will be opened
  fstream file;
  
  //The current position in the table being written
  UINT32 tablePos;
  try {
    //Open the file
    file.open(fileName, ios::out);
    //Set the exceptions
    file.exceptions(ofstream::eofbit | ofstream::failbit | ofstream::badbit);
    

    // First send the table size to the file
    file << tableLength << " " << max << " " << min << " " << doLookupForward << " ";
    //Send the whole table to the file
    for( tablePos = 0; tablePos < tableLength; tablePos++ ) {
      file << table[tablePos] << " ";
    }
  } catch( ofstream::failure e ) {
    cerr << "LookupTable File write error (" << fileName << ")" << endl;
    retval = 1;
   
  }
  
  //Close the file
  if( file.is_open() ) {
    file.close();
  }
  
  if( retval == 0 )
  {
    printf( "Lookup table successfully saved to %s (%s @ %d)"CRLF, 
      fileName, __FILE__, __LINE__ );
  }
  else
  {
    printf( "Error in saving Lookup table to %s (%s @ %d)"CRLF, 
      fileName, __FILE__, __LINE__ );
  }
  
  return retval;
  
}

/*****************************************************************
 * LookupTable::print
 *****************************************************************
 */
void LookupTable::print()
{
  UINT32 tablePos;

  cout << "Forward table (min = " << min << ", max = " << max << ")"<<endl;
  for( tablePos = 0; tablePos < tableLength; tablePos++ ) 
  {
    cout << "table[" << tablePos << "] = " << table[tablePos] << endl;
  }  
}

/*****************************************************************
 * LookupTable::lInterpolation
 *****************************************************************
 */
FLOAT LookupTable::lInterpolation(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2,
                                  FLOAT x){

  FLOAT y;
  y = y1 + (y2-y1)*((x - x1)/(x2-x1));
  return y;
  
}
 
/*****************************************************************
 * LookupTable::getFTable
 *****************************************************************
 */
void LookupTable::getTable( FLOAT **myTable, UINT32 *myTableLength, FLOAT *myTableMin,
                             FLOAT *myTableMax) 
{
  //Give a pointer to the table
  *myTable = table;
  
  //But copy the other attributes
  *myTableLength = tableLength;
  *myTableMin = min;
  *myTableMax = max;
}

// File: $Id: LookupTable.cpp,v 1.6 2005/06/13 21:04:19 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: LookupTable.cpp,v $
// Revision 1.6  2005/06/13 21:04:19  edn2065
// Implemented fixating. Needs to be fine tuned
//
// Revision 1.5  2005/06/13 18:47:14  edn2065
// Calibration implemented without principle point finder.
//
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
// Revision 1.6  2005/03/11 21:28:01  edn2065
// Works without reverseTable
//
// Revision 1.5  2005/03/11 20:41:26  edn2065
// Reverse lookup table proving more trouble than worth. reverting to linear search
//
// Revision 1.4  2005/03/11 04:05:01  edn2065
// Reverse Lookup table apparently works
//
// Revision 1.3  2005/03/11 02:04:52  edn2065
// forward lookup works, but backward is messed up. Going to make reverse table
// a LookupTable and setting min and max for looking up.
//
// Revision 1.2  2005/03/11 01:07:15  edn2065
// Correctly creates the lookup table and saves it to a file. Does good
// error checking for file I/O.
//
// Revision 1.1  2005/03/11 00:03:59  edn2065
// Initial revision
//
