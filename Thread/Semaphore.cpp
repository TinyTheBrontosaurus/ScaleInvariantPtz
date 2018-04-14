#include "Semaphore.h"


/*****************************************************************
 * Semaphore ctor
 *****************************************************************
 */
Semaphore::Semaphore() 
{
  //Not an interprocess thread. Has initial value of 1.
  sem_init( &pSemaphore, 0, 1 );
}

/*****************************************************************
 * Semaphore dtor
 *****************************************************************
 */
Semaphore::~Semaphore() 
{
  sem_destroy(&pSemaphore);
}

/*****************************************************************
 * Semaphore::Init
 *****************************************************************
 */
void Semaphore::init( UINT8 initialVal )
{
  sem_destroy(&pSemaphore);
  sem_init( &pSemaphore, 0, initialVal );
}

/*****************************************************************
 * Semaphore::acquire
 *****************************************************************
 */
void Semaphore::acquire() 
{
  sem_wait( &pSemaphore );
}

/*****************************************************************
 * Semaphore::tryAcquire
 *****************************************************************
 */
SINT16 Semaphore::tryAcquire() 
{
  return sem_trywait( &pSemaphore );
}

/*****************************************************************
 * Semaphore::release
 *****************************************************************
 */
void Semaphore::release() 
{
  sem_post( &pSemaphore );
}


/*
 * Author: Eric D Nelson
 *
 * Version:
 *     $Id: Semaphore.cpp,v 1.2 2005/07/01 16:43:05 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Semaphore.cpp,v $
 *     Revision 1.2  2005/07/01 16:43:05  edn2065
 *     Created sine wave generator for camera
 *
 *     Revision 1.1  2005/06/03 18:10:25  edn2065
 *     Created a Semaphore class
 *
 */
