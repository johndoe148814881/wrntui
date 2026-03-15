#include "../include/cmd.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// local type defs
typedef struct {
	char* cmd;
	int (*func)(int, char**);} cmd_t;

// global vars
int cmdprefix = ':';

// local vars
static cmd_t* cmdv = 0; static int cmdc = 0;

// local func defs
static void newcmd(char*, int (*)(int, char**));
static void delallcmds();
static void alloctokens(char*, int*, char***);
static void freetokens(int, char**);

// global funcs
void cmdnew(char* cmd, int (*func)(int, char**)) {
	newcmd(cmd, func);}

void cmdfreeall() {
	delallcmds();}

int cmdexecute(char* cmdbuf) {
	int argc = 0; char** argv = 0;
	alloctokens(cmdbuf, &argc, &argv);

	if (argc < 1 || !argv) {
		freetokens(argc, argv);
		return 0;}

	int cmdi = -1;
	for (; cmdi < cmdc; ++cmdi)
		if (strcmp(cmdv[cmdi].cmd, argv[0] + 1) == 0) 
			break;

	if (cmdi == -1) {
		freetokens(argc, argv);
		return CMDINVALID;}

	int ret = cmdv[cmdi].func(argc, argv);
	freetokens(argc, argv);
	return ret;}

// local funcs
static void newcmd(char* cmdstr, int (*func)(int, char**)) {
	char* heapcmdstr = malloc(strlen(cmdstr) + 1);
	cmdv = realloc(cmdv, ++cmdc * sizeof(cmd_t*));
	
	if (!heapcmdstr || !cmdv) 
		abort();
		
	strcpy(heapcmdstr, cmdstr);
	cmdv[cmdc - 1] = (cmd_t){heapcmdstr, func};}
	
static void delallcmds() {
	for (int i = 0; i < cmdc; ++i)  
		free(cmdv[i].cmd);
	free(cmdv);
	cmdv = 0;
	cmdc = 0;}
	
/* author: chatgpt & claude */ static void alloctokens(char* cmd, int* argcp, char*** argvp) {
	size_t len = strlen(cmd);
	int capacity = 8;
	char* copy = malloc(len + 1);
	char** argv = malloc(capacity * sizeof(char*));
	if (!copy || !argv) 
		abort();

	strcpy(copy, cmd);
	int argc = 0;
	char* p = copy;
	while (*p) {
		while (isspace((unsigned char)*p))
			p++;
		if (*p == 0)
			break;
		char* start = p;
		while (*p && !isspace((unsigned char)*p))
			p++;
		size_t toklen = p - start;
		argv[argc] = malloc(toklen + 1);
		if (!argv[argc]) 
			abort();

		memcpy(argv[argc], start, toklen);
		argv[argc][toklen] = '\0';
		argc++;
		if (argc + 1 >= capacity) {
			capacity *= 2;
			char** tmp = realloc(argv, capacity * sizeof(char*));
			if (!tmp)
				abort();
				
			argv = tmp;}}
	free(copy);
	
	*argcp = argc;
	*argvp = argv;}

static void freetokens(int argc, char** argv) {
	for (int i = 0; i < argc; i++)
		free(argv[i]);
	free(argv);}

