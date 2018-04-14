#ifndef _STRUCTFLAG_H_
#define _STRUCTFLAG_H_
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define DEBUG 0 //Set to 1 to enter debug mode
#define TIME 0 //Set to 1 to enter timing mode
/*
 *
 * The instruction to send to the camera
 * and the expected reply with the length 
 * of both instruction as well as the
 * appropriate END character.
 *
 */
typedef struct visca_command {

  unsigned char send[15];
  
  unsigned char reply[20];

  int sendLength;
  
  int replyLength;

  unsigned char end;

} command;

/*
 *
 * Flag structure to be used by all libraries.
 * sony_canon = 1 for sony camera.
 * sony_canon = 0 for canon camera.
 * sync_Async = 1 for synchronous data transfer.
 * sync_Async = 0 for asynchronous data transfer.
 *
 */
typedef struct flag_all {

  int sony_canon;
  int camera;

} flag;

extern FILE *logfile;

#define STARTTIME time1 = gettimeofday(&bt,NULL);
#define ENDTIME time2 = gettimeofday(&et,NULL);delay1 = (float) (et.tv_sec - bt.tv_sec);if( (delay2 = (float) (et.tv_usec - bt.tv_usec)/1000000) < 0 ) delay2 += 1;printf("\nThis program segment took %f seconds to execute.\n", (float) delay2+delay1 );

extern struct timeval bt, et;
extern double time1, time2;
extern float delay1, delay2;

#endif
