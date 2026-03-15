#include "../include/win.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <string.h>

// local type defs
typedef struct {
	char* name;
	void (*onshow)(void);} win_t;

// local vars
static win_t* winv = 0; static int winc = 0;

// local func defs
static void newwin(char*, void (*)(void));
static void delallwins();
static int showwin(char*); 

// global funcs
void winnew(char* name, void (*onshow)(void)) {
	pthread_mutex_lock(&tuiwinmutex);
	newwin(name, onshow);
	pthread_mutex_unlock(&tuiwinmutex);}

int winshow(char* name) {
	pthread_mutex_lock(&tuiwinmutex);
	int ret = showwin(name);
	pthread_mutex_unlock(&tuiwinmutex);
	return ret;}

void winfreeall() {
	pthread_mutex_lock(&tuiwinmutex);
	delallwins();
	pthread_mutex_unlock(&tuiwinmutex);}

// local funcs
static void newwin(char* name, void (*onshow)(void)) {
	winv = realloc(winv, sizeof(win_t) * ++winc);
	winv[winc - 1] = (win_t){name, onshow};}

static void delallwins() {
	free(winv);}

static int showwin(char* name) {
	for (int i = 0; i < winc; ++i)
		if (strcmp(winv[i].name, name) == 0) {
			bindfreeall();
			infofreeall();
			listfree();
			msgfreealluser();
			winv[i].onshow();
			return 0;}
	return 1;}
