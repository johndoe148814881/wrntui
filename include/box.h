#ifndef TUI_BOX_H
#define TUI_BOX_H

typedef struct opt_t {
	char* name;
	void (*func)(void);} opt_t;

extern void boxdraw(int, int, int, int, char*, char*, opt_t**);

#endif

