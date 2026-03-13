#include "../include/bind.h"
#include <stdlib.h>

// local type defs
typedef struct {
	int key;
	void (*func)(void);} bind_t;

// local func defs
static bind_t* newbind(int, void (*)(void));
static void delbind(bind_t*);

// local vars
bind_t** bindv = 0; int bindc = 0;

// global funcs
void bindnew(int key, void (*func)(void)) {
	(void)newbind(key, func);}

void bindexecute(int key) {
	for (int i = 0; i < bindc; ++i)
		if (key == bindv[i]->key)
			bindv[i]->func();}

void bindfreeall() {
	for (; bindc > 0;)
		delbind(bindv[0]);}

// local funcs
static bind_t* newbind(int key, void (*func)(void)) {
	bind_t* bind = malloc(sizeof(bind_t));
	if (!bind) {
		abort();
		return 0;}
	bind->key = key;
	bind->func = func;

	bindv = realloc(bindv, ++bindc * sizeof(bind_t*));
	if (!bindv) {
		abort();
		return 0;}
	bindv[bindc - 1] = bind; 

	return bind;}

static void delbind(bind_t* bind) {
	int bindi = -1;
	for (int i = 0; i < bindc; ++i) { 
		if (bindv[i] == bind && bindi == -1)
			bindi = i;
		if (i > bindi && bindi != -1)
			bindv[i - 1] = bindv[i];}
	if (bindi == -1)
		return;

	free(bind);
	
	bindv = realloc(bindv, --bindc * sizeof(bind_t*));
	if (!bindv && bindc > 0) {
		abort();
		return;}}

