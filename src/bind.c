#include "bind.h"
#include <stdlib.h>

// local func defs
void freeb(bind_t*);

// global vars
bind_t** binds = 0; int nbinds = 0;

// global funcs
void bindkey(int key, void (*func)(void)) {
	binds = realloc(binds, ++nbinds * sizeof(bind_t*));
	binds[nbinds - 1] = malloc(sizeof(bind_t));
	*binds[nbinds - 1] = (bind_t){key, func};};

void bindfreeall() {
	for (int i = 0; i < nbinds; ++i)
		freeb(binds[i]);

	free(binds);
	binds = 0; nbinds = 0;}

// local funcs
void freeb(bind_t* bind) {
	free(bind);
	bind = 0;}


