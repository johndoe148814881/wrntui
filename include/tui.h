#ifndef TUI_TUI_H
#define TUI_TUI_H
#define CMDSUCCESS 0
#define CMDINVALID -1
#define CMDINVALIDARGV -2
#define CMDINVALIDARGC -3
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

extern int doflush;
extern int* isrunning;
extern void* tui(void*);

#endif
