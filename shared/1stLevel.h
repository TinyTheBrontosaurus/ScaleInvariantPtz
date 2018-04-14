#ifndef _1STLEVEL_H_
#define _1STLEVEL_H_

#include "structFlag.h"

int ReadFromCam(int fd, command *aCommand, int time); 

int SendToCam(int fd, command *aCommand);
 
#endif
