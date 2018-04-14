#include "Barrier.h"
#include <stdio.h>


/*****************************************************************
 * Barrier ctor
 *****************************************************************
 */
Barrier::Barrier() 
{
  initialized = false;
}

/*****************************************************************
 * Barrier ctor
 *****************************************************************
 */
Barrier::Barrier(UINT32 numThreads) 
{
  initialized = false;
  init( numThreads );
}


/*****************************************************************
 * Barrier dtor
 *****************************************************************
 */
Barrier::~Barrier() 
{
  pthread_barrier_destroy(&pBarrier);
}

/*****************************************************************
 * Barrier::init
 *****************************************************************
 */
void Barrier::init( UINT32 numThreads) 
{
  pthread_barrier_init( &pBarrier, NULL, numThreads );  
  initialized = true;
}

/*****************************************************************
 * Barrier::wait
 *****************************************************************
 */
void Barrier::wait() 
{
  if( initialized )
  {
    pthread_barrier_wait(&pBarrier);
  }
  else
  {
    printf( "Error: barrier not initialized %s @ %d" CRLF, __FILE__, __LINE__ );
  }
}


/*
 * Version:
 *     $Id: Barrier.cpp,v 1.3 2005/08/25 22:12:23 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Barrier.cpp,v $
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
