#include "Thread.h"

/*
 * Constructor
 */
Thread::Thread() {}

/*
 * Destructor
 */
Thread::~Thread() {}

/*
 * Creates the thread and starts it running
 */
int Thread::start(void *a)
{
  setArg(a);
  return pthread_create(&threadID, NULL, Thread::entryPoint, this);
}

/*
 * Waits for the thread to die
 */
int Thread::join( void **valuePtr)
{
  return pthread_join(threadID, valuePtr);
}

/*
 * Serves as the entry point to the thread - casts pthis to Thread*
 */
void *Thread::entryPoint(void *pthis)
{
	Thread *pt = (Thread*)pthis;
	pt->run(pt->getArg());
        return( pthis);
}

/*
 * Sets up and executes thread
 * This is a protected function that should not be tampered with
 */
int Thread::run(void *a)
{
  // setup();
  execute(a);
  return((int)a);
}

/*
 * This is a virtual function that should be overridden by inhereting
 * class
 */
void Thread::execute(void *a) {}

/*
 * thread.cpp
 *
 * Description: Thread class implementation
 *
 * Author: John Ruppert
 *
 * Version:
 *     $Id: Thread.cpp,v 1.7 2005/08/10 02:48:03 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Thread.cpp,v $
 *     Revision 1.7  2005/08/10 02:48:03  edn2065
 *     Changed to allow warning and error free ICC compile
 *
 *     Revision 1.6  2005/05/25 16:59:14  edn2065
 *     Implemented FPS calculation
 *
 *     Revision 1.5  2005/05/25 14:45:36  edn2065
 *     Changed Thread to a pthread implementation
 *
 *     Revision 1.1  2004/12/22 15:26:07  jar4773
 *     Initial revision
 *
 *
 */
