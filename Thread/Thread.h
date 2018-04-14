#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread
{
 public:
  Thread();
  ~Thread();
  int start(void *);
  int join(void **);
  
 protected:
  static void * entryPoint(void *);
  int run(void *);
  //virtual void setup();
  virtual void execute(void *);
  pthread_t getThreadID() const { return threadID; }
  void * getArg() const { return arg; }
  void setArg(void *a) { arg = a; }

 private:
  pthread_t threadID; // thread ID
  void     *arg;      // used to pass arguments to thread functions
};

#endif

/*
 * Thread.h
 *
 * Description: C++ Wrapper for <pthread.h>
 *
 * Author: John Ruppert
 *
 * Version:
 *     $Id: Thread.h,v 1.7 2005/05/25 16:59:14 edn2065 Exp $
 *
 * Revisions:
 *     $Log: Thread.h,v $
 *     Revision 1.7  2005/05/25 16:59:14  edn2065
 *     Implemented FPS calculation
 *
 *     Revision 1.6  2005/05/25 14:45:36  edn2065
 *     Changed Thread to a pthread implementation
 *
 *     Revision 1.1  2004/12/22 15:21:57  jar4773
 *     Initial revision
 *
 */

