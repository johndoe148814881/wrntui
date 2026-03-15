#ifndef CMD_H
#define CMD_H
#define CMDSUCCESS 0
#define CMDINVALID -1
#define CMDINVALIDARGV -2
#define CMDINVALIDARGC -3

extern int cmdprefix;

extern void cmdnew(char* command, int (*function)(int argc, char** argv));
extern int cmdexecute(char* string);
extern void cmdfreeall();

#endif
