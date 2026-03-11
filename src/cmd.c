#include "../include/cmd.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// local func defs
cmd_t newc(char*, int (*)(int, char**));
void freec(cmd_t*);
void freeta();

// global vars
cmd_t** cmds = 0; int ncmds = 0;
char** tokens = 0; int ntokens = 0;

// global funcs
void cmdnew(char* cmd, int (*func)(int, char**)) {
	cmds = realloc(cmds, ++ncmds * sizeof(cmd_t*));
	cmds[ncmds - 1] = malloc(sizeof(cmd_t));
	*cmds[ncmds - 1] = newc(cmd, func);}

void cmdfreeall() {
	for (int i = 0; i < ncmds; ++i)
		freec(cmds[i]);

	free(cmds);
	cmds = 0; ncmds = 0;}

/* author: chatgpt & claude */ void newt(char* cmd, int* argcp, char*** argvp) {
	size_t len = strlen(cmd);
	char* copy = malloc(len + 1);
	if (!copy)
		return;
	strcpy(copy, cmd);
	int capacity = 8;
	char** argv = malloc(capacity * sizeof(char*));
	if (!argv) {
		free(copy);
		return;}
	int argc = 0;
	char* p = copy;
	while (*p) {
		while (isspace((unsigned char)*p))
			p++;
		if (*p == '\0')
			break;
		char* start = p;
		while (*p && !isspace((unsigned char)*p))
			p++;
		size_t toklen = p - start;
		argv[argc] = malloc(toklen + 1);
		if (!argv[argc]) {
			for (int i = 0; i < argc; i++) free(argv[i]);
			free(argv);
			free(copy);
			return;}
		memcpy(argv[argc], start, toklen);
		argv[argc][toklen] = '\0';
		argc++;
		if (argc + 1 >= capacity) {
			capacity *= 2;
			char** tmp = realloc(argv, capacity * sizeof(char*));
			if (!tmp) {
				for (int i = 0; i < argc; i++) free(argv[i]);
				free(argv);
				free(copy);
				return;}
			argv = tmp;}}
	free(copy);
	
	/* author: me */ ntokens += argc;
	tokens = realloc(tokens, ntokens * sizeof(char*));
	
	for (int i = 0; i < argc; ++i) 
		tokens[ntokens - argc + i] = argv[i];
	
	*argcp = argc;
	*argvp = argv;}

int cmdexec(char* str) {
	cmd_t* cmd = 0;
	for (int i = 0; i < ncmds; ++i)
		if (strcmp(cmds[i]->cmd, str + 1) == 0) {
			cmd = cmds[i];
			break;}

	if (!cmd)
		return CMDINVALID;

	int argc; char** argv;
	newt(str, &argc, &argv);
	int ret = cmd->func(argc, argv);
	freeta()
		;
	return ret;}

// local funcs
cmd_t newc(char* cmd, int (*func)(int, char**)) {
	char* heapcmd = malloc(strlen(cmd) + 1);
	strcpy(heapcmd, cmd);
	return (cmd_t){heapcmd, func};}

void freec(cmd_t* cmd) {
	free(cmd->cmd);
	free(cmd);
	cmd = 0;}

void freeta() {
	for (int i = 0; i < ntokens; i++)
		free(tokens[i]);

	free(tokens);
	tokens = 0; ntokens = 0;}

