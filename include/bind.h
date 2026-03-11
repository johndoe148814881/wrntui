#ifndef TUI_BIND_H
#define TUI_BIND_H
typedef struct {
	int key;
	void (*func)(void);} bind_t;

extern bind_t** binds; extern int nbinds;

extern void bindkey(int, void(*)(void));
extern void bindfreeall();

#endif
