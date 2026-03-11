#ifndef CMD_H
#define CMD_H
#define CMDSUCCESS 0
#define CMDINVALID -1
#define CMDINVALIDARGV -2
#define CMDINVALIDARGC -3

typedef struct {
	char* cmd;
	int (*func)(int, char**);} cmd_t;

extern cmd_t** cmds; extern int ncmds;

extern void cmdnew(char*, int (*)(int, char**));
extern void cmdfreeall();
extern int cmdexec(char*);

#endif
