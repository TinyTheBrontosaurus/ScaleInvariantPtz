#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>
#include "types.h"

/**************************************************************
 *
 * Barrier class 
 * Description: A barrier for synchronization between threads.
 *
 *
 **************************************************************
 */
class Barrier
{
  public:
  
   /******************************************
    * ctor
    * Description: Sets defaults
    ******************************************
    */  
    Barrier();
    Barrier(UINT32 numThreads);
    
   /******************************************
    * dtor
    * Description: Deallocates memory
    ******************************************
    */      
    ~Barrier();
    
   /******************************************
    * init
    * Description: Sets up the barrier for a number of threads
    * Parameters:  numThreads - the number of threads this barrier
    *                           keeps track of
    ******************************************
    */      
    void init( UINT32 numThreads );
    
   /******************************************
    * wait
    * Description: Increments the number of threads waiting on the barrier.
    *              Once the number of threads waiting equals the number of 
    *              threads this barrier is responsible for, then all threads
    *              are released.
    ******************************************
    */          
    void wait();
  
  private:
 
    //The barrier type
    pthread_barrier_t pBarrier;   
    
    //Whether this object has been initialized
    BOOL initialized;
};

#endif

/*
 * Author: Eric D Nelson
 *
 * Version:
 *     $Id: Barrier.h,v 1.3 2005/08/25 22:12:23 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Barrier.h,v $
 *     Revision 1.3  2005/08/25 22:12:23  edn2065
 *     Made commandLevel a class. Added HZO pickoff point in SIKernel
 *
 *     Revision 1.2  2005/06/09 13:48:03  edn2065
 *     Fixed synchronization errors caused by not initializing Barrier in SIKernal
 *
 *     Revision 1.1  2005/06/03 17:31:07  edn2065
 *     Implemented Barrier
 *
 */

