#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <semaphore.h>
#include "types.h"

/**************************************************************
 *
 * Semaphore class 
 * Description: A semaphore for synchronization between threads.
 *
 *
 **************************************************************
 */
class Semaphore
{
  public:
  
   /******************************************
    * ctor
    * Description: Sets defaults
    ******************************************
    */  
    Semaphore();
    
   /******************************************
    * dtor
    * Description: Deallocates memory
    ******************************************
    */      
    ~Semaphore();
    
    // Sets initial value of semaphore
    void init( UINT8 initialVal );
    
   /******************************************
    * acquire
    * Description: Acquires the lock on the semaphore object. If it is
    *              already locked, it waits indefinitely until it is
    *              unlocked.
    ******************************************
    */          
    void acquire();
    
   /******************************************
    * tryAcquire
    * Description: Tries to acquires the lock on the semaphore object. 
    *              Does not block if it is already locked.
    * Returns:     0 if successfully locked. Nonzero otherwise.
    ******************************************
    */          
    SINT16 tryAcquire();    
    
   /******************************************
    * release
    * Description: Releases the lock on the semaphore object. 
    *              Does not block.
    ******************************************
    */          
    void release();        
  
  private:
 
    //The barrier type
    sem_t pSemaphore;   
};

#endif

/*
 * Author: Eric D Nelson
 *
 * Version:
 *     $Id: Semaphore.h,v 1.2 2005/07/01 16:43:05 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Semaphore.h,v $
 *     Revision 1.2  2005/07/01 16:43:05  edn2065
 *     Created sine wave generator for camera
 *
 *     Revision 1.1  2005/06/03 18:10:25  edn2065
 *     Created a Semaphore class
 *
 */

