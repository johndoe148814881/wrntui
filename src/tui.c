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
#define NANOSECS_PER_SEC 1000000000

// global vars
char* tuiforev[TUIFOREC] = {0}; 
char* tuiforewhi = 0;
char* tuiforeerr = 0;
char* tuiforesuc = 0;
char* MOVECURS(int row, int col) {
	static char rstrs[16][64];
	static int rstri;
	
	rstri++;
	if (rstri > 15)
		rstri = 0;

	memset(rstrs[rstri], '\0', 64);
	snprintf(rstrs[rstri], 64, "\033[%d;%dH", row, col);	
	return rstrs[rstri];}
pthread_mutex_t tuiflushmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tuiwinmutex = PTHREAD_MUTEX_INITIALIZER;
int* tuirunning = 0;
int tuiwidth; int tuiheight;
int tuiframerate = 30;

// local vars
static struct termios oldtermattrs;
static int istyping = 0;
static char* cmdbuf = 0;
static char* cmdbufvis = 0;
static char* msgbuf = 0;
static struct timespec ts;
static char exitmsg[128] = {0};
static int showedhomewin = 0;

// local func defs
static void sighandler(int);
static void parseargs(void*);
static void initin();
static void initout();
static void iterin();
static void iterout();
static void exitin();
static void exitout();
static int q(int, char**);

// global funcs
void* (*tuiinit(int* running))(void*) {
	tuirunning = running;
	initin();
	initout();

	return tui;}

void tuisetframerate(int framerate) {
	tuiframerate = framerate;}

void* tui(void* arg) {
	parseargs(arg);

	while (*tuirunning) {
		struct timespec tpre, tpost;
		clock_gettime(CLOCK_MONOTONIC, &tpre);
		
		iterin();
		iterout();
		
		clock_gettime(CLOCK_MONOTONIC, &tpost);
		int elapsed = (tpost.tv_sec - tpre.tv_sec) * 1000000 + (tpost.tv_nsec - tpre.tv_nsec) / 1000;
		int remaining = (1000000 / tuiframerate) - elapsed;
		if (remaining > 0) {
			struct timespec sleep = {0, remaining * 1000};
			nanosleep(&sleep, 0);}}
	
	exitin();
	exitout();

	return 0;}

// local funcs
static void sighandler(int sig) {
	snprintf(exitmsg, 128, "caught signal: %s (%d)", strsignal(sig), sig);
	*tuirunning = 0;}

static void parseargs(void* args) {
	(void)args;}

static void initin() {
	signal(SIGINT, sighandler); // handle sigint and sigabrt safely
	signal(SIGABRT, sighandler);
	
	setvbuf(stdout, 0, _IOFBF, 8192);  // disable printf automatically flushing to stdout
	
	struct termios newtermattrs; // disable echo and canonical mode
	tcgetattr(STDIN_FILENO, &oldtermattrs);
	newtermattrs = oldtermattrs;
	newtermattrs.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newtermattrs);
	
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0); // disable stdin funcs yeilding
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	cmdnew("q", q); // default commands
	cmdnew("quit", q);}

static void initout() {
	char* ct = getenv("COLORTERM"); // set forecolors to 24BIT versions if terminal supports 24BIT colors
	char* term = getenv("TERM");
	if (ct && term && strcmp(term, "linux") != 0 &&(strcmp(ct, "truecolor") == 0 || strcmp(ct, "24BIT") == 0)) {
		tuiforev[0] = FORE124BIT;
		tuiforev[1] = FORE224BIT;
		tuiforev[2] = FORE324BIT;
		tuiforev[3] = FORE424BIT;
		tuiforev[4] = FORE524BIT;
		tuiforeerr = FOREERR24BIT;
		tuiforesuc = FORESUC24BIT;}
	else {
		tuiforev[0] = FOREWHITTY;
		tuiforev[1] = FOREWHITTY;
		tuiforev[2] = FOREWHITTY;
		tuiforev[3] = FOREWHITTY;
		tuiforev[4] = FOREWHITTY;
		tuiforeerr = FOREERRTTY;
		tuiforesuc = FORESUCTTY;}
	tuiforewhi = FOREWHITTY;

	struct winsize w; // get width and height of terminal/tty
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_row == 0 || w.ws_col == 0) {
		abort();
		return;}
	tuiheight = w.ws_row; tuiwidth = w.ws_col;

	printf("%s%s%s%s%s", SAVECURS, HIDECURS, ALTBUF, CLRBUF, CLRATTRS); // enter alt buffer

	cmdbuf = malloc(tuiwidth); // allocate memory for buffers
	cmdbufvis = malloc(tuiwidth);
	msgbuf = malloc(tuiwidth);

	if (!cmdbuf || !cmdbufvis || !msgbuf) {
		abort();
		return;}

	memset(cmdbuf, 0, tuiwidth);
	memset(cmdbufvis, 0, tuiwidth);
	memset(msgbuf, 0, tuiwidth);
	
	msgnew(tuiheight - 1, 1, tuiwidth, msgbuf); // draw cmd and msg bufs
	msgnew(tuiheight, 1, tuiwidth, cmdbufvis);}
	
static void iterin() {
	for (int ch; !istyping && (ch = getchar()) != EOF;) {
		switch (ch) { // any other input
		default:
			if (ch == cmdprefix) { // cmd prefix entered, allow typing in cmdbuffer
				istyping = 1;
				if ((int)strlen(cmdbuf) < tuiwidth - 1)
					cmdbuf[strlen(cmdbuf)] = ch;
				break;}
			
			bindexecute(ch);
			break;}}
	
	for (int ch; istyping && (ch = getchar()) != EOF;) // cmdbuffer related input
		switch (ch) {
		case '\n': { // if enter pressed, execute cmd
			int ret = cmdexecute(cmdbuf);
			switch (ret) {
			case CMDSUCCESS: 
				snprintf(msgbuf, tuiwidth, "%s%s%s%s", tuiforesuc, BOLD, cmdbuf, CLRATTRS);
				break;
			case CMDINVALID: case CMDINVALIDARGC: case CMDINVALIDARGV: {
				char* generalmsg = ret == CMDINVALID ? "invalid command" : 
					(ret == CMDINVALIDARGC ? "invalid arg count" : 
					 "invalid arg values");
				snprintf(msgbuf, tuiwidth, "%s%s%s: %s%s", tuiforeerr, BOLD, generalmsg, cmdbuf, CLRATTRS);
				break;}}

			istyping = 0;
			memset(cmdbuf, 0, tuiwidth);
			
			for (; (ch = getchar()) != EOF;); // clear stdin

			break;}
		case 127: case 8: // if backspace pressed, delete last char in cmdbuffer
			istyping = strlen(cmdbuf) > 1;
			cmdbuf[strlen(cmdbuf) - 1] = '\0';
			break;
		default:  // if valid char pressed, add to cmdbuffer
			if ((int)strlen(cmdbuf) < tuiwidth - 1 && isprint(ch)) 
				cmdbuf[strlen(cmdbuf)] = ch;
			break;}}

static void iterout() {
	clock_gettime(CLOCK_MONOTONIC, &ts);

	if (!showedhomewin)
		if (!(showedhomewin = !winshow("main")))
			return;

	pthread_mutex_lock(&tuiwinmutex);

	if (istyping) { // draw underscore at end of cmdbuffer
		time_t time = (ts.tv_sec * NANOSECS_PER_SEC + ts.tv_nsec);
		char endchar = time % (NANOSECS_PER_SEC / 2) > NANOSECS_PER_SEC / 4 ? '_' : ' ';
		snprintf(cmdbufvis, strlen(cmdbuf) + 2, "%s%c", cmdbuf, endchar);}
	else if (!istyping && *cmdbufvis)
		memset(cmdbufvis, 0, strlen(cmdbufvis));

	msgdrawall();
	infodrawall();
	listdraw();
	
	pthread_mutex_lock(&tuiflushmutex);
	fflush(stdout);
	pthread_mutex_unlock(&tuiflushmutex);
	pthread_mutex_unlock(&tuiwinmutex);}

static void exitin() {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldtermattrs);} // reenable echo and canonical mode

static void exitout() {
	free(cmdbuf);
	free(cmdbufvis);
	free(msgbuf);

	cmdfreeall();
	bindfreeall();
	msgfreeall();
	infofreeall();
	listfree();
	winfreeall();

	printf("%s%s%s%s%s\n%s\n", CLRATTRS, CLRBUF, REGBUF, LOADCURS, SHOWCURS, exitmsg);
	fflush(stdout);} // restore regular buffer

static int q(int argc, char** argv) {
	(void)argv;
	if (argc != 1)
		return CMDINVALIDARGC;
	
	*tuirunning = 0;
	return CMDSUCCESS;}

