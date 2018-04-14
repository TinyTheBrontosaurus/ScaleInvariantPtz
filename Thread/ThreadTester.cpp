#include <stdio.h>
#include <unistd.h>
#include "Thread.h"
#include "Barrier.h"
#include "Semaphore.h"


static bool executeThread;

class IncrementThread : public Thread {

  public:
    IncrementThread( int *myNum, Barrier *myBar, Semaphore *mySem ) 
      : num( myNum ), barrier(myBar), semaphore( mySem ) {}

    virtual void execute(void *)
    {
      while( executeThread )
      {
        semaphore->acquire();
        (*num)++;
        printf("Incremented (%d)"CRLF, *num);
        semaphore->release();
        barrier->wait();
        
      }
      printf( "IncrementThread done.\n" );
    }
    
  private:
    //The number to be incremented
    int *num;       
    Barrier *barrier;
    Semaphore *semaphore;
};


class DecrementThread : public Thread {

  public:
    DecrementThread( int *myNum, Barrier *myBar, Semaphore *mySem ) 
      : num( myNum ), barrier(myBar), semaphore( mySem ) {}

    virtual void execute(void *)
    {
      while( executeThread )
      {
        semaphore->acquire();
        (*num)--;
        printf("Decremented (%d)"CRLF, *num);
        semaphore->release();
        sleep(1);
        barrier->wait();
        
      }
      printf( "DecrementThread done.\n" );
    }
    
  private:
    //The number to be incremented
    int *num;       
    Barrier *barrier;
    Semaphore *semaphore;
};

int main() 
{
  int theNum = 17;
  Barrier bar;
  Semaphore sem;
  
  bar.init(2);
  
  IncrementThread *incT = new IncrementThread( &theNum, &bar, &sem );
  DecrementThread *decT = new DecrementThread( &theNum, &bar, &sem );
  
  
  
  executeThread = true;
  
  incT->start(0);
  decT->start(0);


  while( executeThread )
  {
    char c;
    scanf("%c", &c);
    
    
    switch( c )
    {
      //Quit
      case 'Q':
      case 'q':
        executeThread = false;
        break;
      
      //Check number
      case 'C':
      case 'c':
        printf( "theNum = %d.\n", theNum);
        break;
      case '\n':
        break;
      default:
        printf( "C: Check number\nQ: quit\n" );
        break;
    }
  }
  
  printf( "Waiting for incT...\n" );
  incT->join(0);
  printf( "Waiting for decT...\n" );
  decT->join(0);
  
  return 0;
}
