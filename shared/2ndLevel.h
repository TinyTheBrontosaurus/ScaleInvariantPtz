#ifndef _2NDLEVEL_H_
#define _2NDLEVEL_H_
#include "structFlag.h"
#include "1stLevel.h"

//Initialize the serial port
int Initialize(char *serial, flag *aflag, int cameranum);
int Initialize38400(char *serial, flag *aflag, int cameranum);

//Send an intruction to the SONY and wait until command is done
int SendSynchSony(int fd, command *inst, int retries);

//Send an instruction to the SONY and return immediately
int SendAsynch(int fd, command *inst);

//Send an inquiry instruction to the SONY camera
int Inquiry(int fd, command *inq);

//Send an intruction to the CANON and wait until command is done
int SendSynchCanon(int fd, command *inst);

//Send an inquiry instruction to the CANON camera
int InquiryCanon(int fd, command *inq);

//Wait until CANON command is done
int WaitForNotification(int fd);

//Reset the port to it's original setting
int ResetPort(int fd);

int SetupConnection(char *serial);

int Initialize2(char *serial, flag *aflag, int cameranum);

int InitializeSony(int fd, int cameranum);

int ClearSonyInterface(int fd);

int InitializeCanon(int fd);

int IsEqual(command *aC, command *bC);

int IsAckSony(command *aC);

int IsCplSony(command *aC);

int IsErrorSony(command *err);

int IsErrorCanon(command *err);

void WhichErrorSony(command *err);

void WhichErrorCanon(command *err);

int IsIrSony(command *ir);

void PrintError(command *err);

void PrintCmd(command *aC);

void PrintReply(command *aC);

#endif
