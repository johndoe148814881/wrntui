#ifndef WRNTUI_H
#define WRNTUI_H

#include <frac.h>

#define CMDSUCCESS 0
#define CMDINVALID -1
#define CMDINVALIDARGV -2
#define CMDINVALIDARGC -3

typedef struct {
	char* cmd;
	int (*func)(int, char**);
} cmd_t;

typedef struct {
	int key;
	void (*func)(void);
} bind_t;

typedef struct opt_t {
	char* name;
	void (*func)(void);
} opt_t;

typedef struct {
	int row;
	int col;
	int cols;
	char* buf;
	char* odraw;
} msg_t;

typedef struct {
	int row;
	int col;
	int cols;
	char* clr;
	char* title;
	char* odraw;
	frac_t* value;
	frac_t ovalue;
} info_t;

extern cmd_t** cmds;
extern int ncmds;
extern bind_t** binds;
extern int nbinds;
extern msg_t** msgs;
extern int nmsgs;
extern info_t** infos;
extern int ninfos;

extern void cmdnew(char*, int (*)(int, char**));
extern void cmdfreeall(void);
extern int cmdexec(char*);
extern void bindkey(int, void (*)(void));
extern void bindfreeall(void);
extern void boxdraw(int, int, int, int, char*, char*, opt_t**);
extern void msgdraw(int, int, int, char*);
extern void msgfreeall(void);
extern void infodraw(int, int, int, char*, char*, frac_t*);
extern void infofreeall(void);
extern void graphdraw(void);
extern void graphfreeall(void);

#define CMDPREFIX '/'
#define FRAMERATE 30

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
#define FOREERR24BIT "\033[38;2;163;74;74m"
#define FORESUC24BIT "\033[38;2;92;158;100m"
#define FOREERR8BIT "\033[38;5;160m"
#define FORESUC8BIT "\033[38;5;34m"
// variable attributes influenced by terminal context
extern char* FORE1;
extern char* FORE2;
extern char* FORE3;
extern char* FOREERR;
extern char* FORESUC;

extern void tuibindkey(int key, void (*func)(void));
extern int doflush;
extern void* tui(void*);

#endif
