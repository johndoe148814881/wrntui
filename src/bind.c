#include "../include/bind.h"
#include <stdlib.h>

// local type defs
typedef struct {
	int key;
	void (*func)(void);} bind_t;

// local vars
bind_t* bindv = 0; int bindc = 0;

// global funcs
void bindnew(int key, void (*func)(void)) {
	bindv = realloc(bindv, ++bindc * sizeof(bind_t));
	if (!bindv)
		abort();
	bindv[bindc - 1] = (bind_t){key, func};}

void bindexecute(int key) {
	for (int i = 0; i < bindc; ++i)
		if (key == bindv[i].key)
			bindv[i].func();}

void bindfreeall() {
	free(bindv);
	bindv = 0;
	bindc = 0;}

