#ifndef TUI_TUI_H
#define TUI_TUI_H
#include "bind.h"
#include "box.h"
#include "cmd.h"
#include "graph.h"
#include "info.h"
#include "msg.h"
#include "list.h"
#include "win.h"
#include <pthread.h>
#define CMDSUCCESS 0
#define CMDINVALID -1
#define CMDINVALIDARGV -2
#define CMDINVALIDARGC -3
#define TUIFOREC 5

// cursor
#define HIDECURS "\033[?25l"
#define SHOWCURS "\033[?25h"
#define SAVECURS "\033[s"
#define LOADCURS "\033[u"
extern char* MOVECURS(int, int);
// buffer
#define ALTBUF "\033[?1049h"
#define REGBUF "\033[?1049l"
#define CLRBUF "\033[2J"
#define CLRTOEOL "\033[K"
// attributes
#define CLRATTRS "\033[0m"
#define BOLD "\033[1m"
#define FORE124BIT "\033[38;2;72;120;140m"
#define FORE224BIT "\033[38;2;130;105;155m"
#define FORE324BIT "\033[38;2;180;140;80m"
#define FORE424BIT "\033[38;2;90;150;120m"
#define FORE524BIT "\033[38;2;160;100;110m" 
#define FOREERR24BIT "\033[38;2;163;74;74m"
#define FORESUC24BIT "\033[38;2;92;158;100m"
#define FOREWHITTY "\033[37m"
#define FOREERRTTY "\033[31m"
#define FORESUCTTY "\033[32m" 
// variable attributes influenced by terminal context
extern char* tuiforev[TUIFOREC];
extern char* tuiforewhi;
extern char* tuiforeerr;
extern char* tuiforesuc;

// global vars
extern pthread_mutex_t tuiflushmutex;
extern pthread_mutex_t tuiwinmutex;
extern int* tuirunning;
extern int tuiwidth, tuiheight;
extern int tuiframerate;

// global funcs
extern void* (*tuiinit(int*))(void*);
extern void tuisetframerate(int);
extern void* tui(void*);

#endif
