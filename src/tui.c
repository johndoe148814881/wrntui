#include "../include/tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <ctype.h>

// global vars
char* FORE1 = 0; 
char* FORE2 = 0;
char* FORE3 = 0;
char* FORE4 = 0;
char* FORE5 = 0;
char* FOREERR = 0;
char* FORESUC = 0;
char* MOVECURS(int row, int col) {
	static char rstrs[16][64];
	static int rstri;
	
	rstri++;
	if (rstri > 15)
		rstri = 0;

	memset(rstrs[rstri], '\0', 64);
	snprintf(rstrs[rstri], 64, "\033[%d;%dH", row, col);	
	return rstrs[rstri];}
int doflush = 1;
int* isrunning = 0;
int width; int height;

// local vars
static struct termios oldtermattrs;
static int istyping = 0;
static char* cmdbuf = NULL;
static char* msgbuf = NULL;
static int cmdbuft;

// local func defs
static void sigint(int);
static void parseargs(void*);
static void initin();
static int initout();
static void iterin();
static void iterout();
static void exitin();
static void exitout();
static int q(int, char**);

// global func tui thread
void* (*tuiinit(int* isrunningp))(void*) {
	isrunning = isrunningp;
	initin();
	*isrunning = !initout();

	return tui;}

void* tui(void* arg) {
	while (*isrunning) {
		struct timespec tpre, tpost;
		clock_gettime(CLOCK_MONOTONIC, &tpre);
		
		iterin();
		iterout();
		
		clock_gettime(CLOCK_MONOTONIC, &tpost);
		int elapsed = (tpost.tv_sec - tpre.tv_sec) * 1000000 + (tpost.tv_nsec - tpre.tv_nsec) / 1000;
		int remaining = (1000000 / FRAMERATE) - elapsed;
		if (remaining > 0) {
			struct timespec sleep = {0, remaining * 1000};
			nanosleep(&sleep, NULL);}}
	
	exitin();
	exitout();

	return NULL;}

// local funcs
static void sigint(int sig) {
	(void)sig;
	*isrunning = 0;}

static void parseargs(void* args) {
	(void)args;}

static void initin() {
	signal(SIGINT, sigint); // handle signal interrupt safely
	
	setvbuf(stdout, NULL, _IOFBF, 8192);  // disable printf automatically flushing to stdout
	
	struct termios newtermattrs; // disable echo and canonical mode
	tcgetattr(STDIN_FILENO, &oldtermattrs);
	newtermattrs = oldtermattrs;
	newtermattrs.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newtermattrs);
	
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0); // disable stdin funcs yeilding
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	cmdnew("q", q);} // default commands

static int initout() {
	char* ct = getenv("COLORTERM"); // set forecolors to 24BIT versions if terminal supports 24BIT colors
	char *term = getenv("TERM");
	if (ct && term && strcmp(term, "linux") != 0 &&(strcmp(ct, "truecolor") == 0 || strcmp(ct, "24BIT") == 0)) {
		FORE1 = FORE124BIT;
		FORE2 = FORE224BIT;
		FORE3 = FORE324BIT;
		FORE4 = FORE424BIT;
		FORE5 = FORE524BIT;
		FOREERR = FOREERR24BIT;
		FORESUC = FORESUC24BIT;}
	else {
		FORE1 = FOREWHITTY;
		FORE2 = FOREWHITTY;
		FORE3 = FOREWHITTY;
		FORE4 = FOREWHITTY;
		FORE5 = FOREWHITTY;
		FOREERR = FOREERRTTY;
		FORESUC = FORESUCTTY;}

	struct winsize w; // get width and height of terminal/tty
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_row == 0 || w.ws_col == 0)
		return 1;
	height = w.ws_row; width = w.ws_col;

	printf("%s%s%s%s%s", SAVECURS, HIDECURS, ALTBUF, CLRBUF, CLRATTRS); // enter alt buffer

	cmdbuf = malloc(width); // allocate memory for buffers
	msgbuf = malloc(width);

	cmdbuft = clock();
//	graphst = clock();
	
	memset(cmdbuf, 0, width);
	memset(msgbuf, 0, width);
	
	return 0;}

static void iterin() {
	int ch = getchar();
		
	if (istyping) { // cmdbuffer related input
		switch (ch) {
		case EOF:
			break;
		case '\n': { // if enter pressed, execute cmd
			int ret = cmdexec(cmdbuf);
			switch (ret) {
			case CMDSUCCESS: 
				snprintf(msgbuf, width, "%s%s%s%s", FORESUC, BOLD, cmdbuf, CLRATTRS);
				break;
			case CMDINVALID: case CMDINVALIDARGC: case CMDINVALIDARGV: {
				char* generalmsg = ret == CMDINVALID ? "invalid command" : 
					(ret == CMDINVALIDARGC ? "invalid arg count" : 
					 "invalid arg values");
				snprintf(msgbuf, width, "%s%s%s: %s%s", FOREERR, BOLD, generalmsg, cmdbuf, CLRATTRS);
				break;}}

			istyping = 0;
			memset(cmdbuf, 0, width);
			break;}
		case 127: case 8: // if backspace pressed, delete last char in cmdbuffer
			istyping = strlen(cmdbuf) > 1;
			cmdbuf[strlen(cmdbuf) - 1] = '\0';
			break;
		default:  // if valid char pressed, add to cmdbuffer
			if ((int)strlen(cmdbuf) < width - 1 && isprint(ch)) 
				cmdbuf[strlen(cmdbuf)] = ch;
			break;}	
		return;}

	switch (ch) { // any other input
	case EOF:
		break;
	default:
		if (ch == cmdprefix) { // cmd prefix entered, allow typing in cmdbuffer
			istyping = 1;
			if ((int)strlen(cmdbuf) < width - 1)
				cmdbuf[strlen(cmdbuf)] = ch;
			break;}

		for (int i = 0; i < nbinds; ++i) // binds 
			if (ch == binds[i]->key) {
				binds[i]->func();
				break;}}}

static void iterout() {
	for (int i = 0; i < ninfos; ++i) // draw changed infos
		infodraw(infos[i]->row, infos[i]->col, infos[i]->cols, infos[i]->clr, infos[i]->title, infos[i]->value);

	for (int i = 0; i < nmsgs; ++i) // draw changed msgs
		msgdraw(msgs[i]->row, msgs[i]->col, msgs[i]->cols, msgs[i]->buf);

	msgdraw(height - 1, 1, width, msgbuf); // draw cmd and msg bufs
	msgdraw(height, 1, width, cmdbuf);

	if (istyping && clock() - cmdbuft > CLOCKS_PER_SEC / 4) { // draw underscore at end of cmdbuffer
		cmdbuft = clock();
		char endchar = cmdbuft % (CLOCKS_PER_SEC / 2) > CLOCKS_PER_SEC / 4 ? '_' : ' ';
		printf("%s%c%s", MOVECURS(height, strlen(cmdbuf) + 1), endchar, CLRTOEOL);
		doflush = 1;}

	if (doflush) { // flush stdout if needed
		doflush = 0;
		fflush(stdout);}}	

static void exitin() {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldtermattrs);} // reenable echo and canonical mode

static void exitout() {
	free(cmdbuf);
	free(msgbuf);
	bindfreeall();
	msgfreeall();
	infofreeall();
	cmdfreeall();
//	graphfreeall();

	printf("%s%s%s%s%s", CLRATTRS, CLRBUF, REGBUF, LOADCURS, SHOWCURS);} // restore regular buffer

static int q(int argc, char** argv) {
	(void)argv;
	if (argc != 1)
		return CMDINVALIDARGC;
	
	*isrunning = 0;
	return CMDSUCCESS;}

