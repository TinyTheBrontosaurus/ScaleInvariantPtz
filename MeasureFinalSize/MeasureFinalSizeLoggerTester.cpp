#include "MeasureFinalSizeLogger.h"

/*****************************************************************
 * MeasureFinalSizeLogger ctor
 *****************************************************************
 */
int main()
{
  
  printf( "Welcome to MFS Logger Tester" CRLF );

  MeasureFinalSizeLogger mfs;  
  mfs.startFile( "./mfsTst.csv" );
  mfs.log( 1, 2, 3, 4, 5, 6, 7.7, 8, 9.9 );
  mfs.log( 10, 20, 30, 40, 50, 60, 70.7, 80, 90.9 );
  mfs.log( 100, 200, 300, 400, 500, 600, 700.7, 800, 900.9 );
  mfs.noLog();
  mfs.log( 1000, 2000, 3000, 4000, 5000, 6000, 7000.7, 8000, 9000.9 );
  mfs.endFile();
  
  printf( "MFS Logger Test Complete." CRLF );

}

// File: $Id: MeasureFinalSizeLoggerTester.cpp,v 1.2 2005/06/10 17:03:47 edn2065 Exp $
// Author: Eric D Nelson
// Revisions:
// $Log: MeasureFinalSizeLoggerTester.cpp,v $
// Revision 1.2  2005/06/10 17:03:47  edn2065
// Added MFS
//
// Revision 1.1  2005/06/10 15:26:04  edn2065
// Created and debugged MFS
//
// Revision 1.1  2005/06/10 14:28:47  edn2065
// Adding to repository
//
