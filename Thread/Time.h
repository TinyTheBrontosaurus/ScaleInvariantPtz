#ifndef EPROJ_TIME_H
#define EPROJ_TIME_H

#include <time.h>
#include <stdio.h>
#include "types.h"

class Time {
 public:
  
  Time() {};
  ~Time() {};
  
  static int getClock(timespec *t) {
    return clock_gettime(CLOCK_REALTIME, t);
  }
  
  static double getTimeDiff(timespec start, timespec end) {
    
    return ((end.tv_sec + ((double)(end.tv_nsec) / 1e9)) - 
	    (start.tv_sec + ((double)(start.tv_nsec) / 1e9)));
  }
  
  static UINT32 getTimestamp()
  {
    timespec curTime;
    getClock(&curTime);
    return (UINT32)(curTime.tv_sec*1e3 + curTime.tv_nsec/1e6);
  }
  
  void tick()
  {
    tickTime = getTimestamp();
  }
  
  UINT32 tock( BOOL output = true )
  {
    UINT32 timeDiff = getTimestamp() - tickTime;
    
    if( output )
    {
      printf( "%4d ms", timeDiff );
    }
  
  
  }
  
  private:
    UINT32 tickTime;
};

#endif

// File: $Id: Time.h,v 1.3 2005/09/11 01:13:59 edn2065 Exp $
// Author: Jared Holsopple
// Contributors: Justin Hnatow, John Ruppert
// Description: Implementation of simple time class
// Revisions:
// $Log: Time.h,v $
// Revision 1.3  2005/09/11 01:13:59  edn2065
// Added TxThread Watchdog
//
// Revision 1.2  2005/06/22 16:57:40  edn2065
// Tied pantilt and zoom readings together to make them faster
//
// Revision 1.1  2005/05/25 16:59:14  edn2065
// Implemented FPS calculation
//
// Revision 1.1  2005/01/14 06:13:32  jdh5443
// Initial revision
//

