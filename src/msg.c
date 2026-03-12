#include "../include/msg.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// local func defs
static msg_t newm(int, int, int, char*);
static void freem(msg_t*);
static int cmpm(msg_t*, msg_t*);
static void drawm(msg_t*);

// local vars
msg_t** msgs = 0; int nmsgs = 0;

// global funcs
void msgdraw(int row, int col, int cols, char* buf) {
	pthread_mutex_lock(&flushmutex);
	msg_t msg = newm(row, col, cols, buf);

	// if arguments equal an existing msg, use that one instead, else add a new entry to msgs
	int exists = 0;
	for (int i = 0; i < nmsgs; ++i)
		if ((exists = cmpm(&msg, msgs[i]) == 0)) {
			msg_t omsg = msg;
			msg = *msgs[i];
			free(omsg.odraw);
			break;}

	if (!exists) {
		msgs = realloc(msgs, ++nmsgs * sizeof(msg_t*));
		msgs[nmsgs - 1] = malloc(sizeof(msg_t));
		*msgs[nmsgs - 1] = msg;}

	drawm(&msg);
	pthread_mutex_unlock(&flushmutex);}

void msgfreeall() {
	pthread_mutex_lock(&flushmutex);
	for (int i = 0; i < nmsgs; ++i)
		freem(msgs[i]);

	free(msgs);
	msgs = 0; nmsgs = 0;
	pthread_mutex_unlock(&flushmutex);}

// local funcs
static msg_t newm(int row, int col, int cols, char* buf) {
	size_t odrawcap = (size_t)cols * 8 + 1;
	char* odraw = malloc(odrawcap);
	memset(odraw, '\0', odrawcap);

	return (msg_t){row, col, cols, buf, odraw, odrawcap};}

static void freem(msg_t* msg) {
	free(msg->odraw);
	free(msg);
	msg = 0;}

static int cmpm(msg_t* a, msg_t* b) {
	return !(a->row == b->row &&
		a->col == b->col &&
		a->cols == b->cols &&
		a->buf == b->buf);}

static void drawm(msg_t* msg) {
	if (strcmp(msg->buf, msg->odraw) == 0 && *msg->odraw != 0)
		return;

	snprintf(msg->odraw, msg->odrawcap, "%s", msg->buf);
	printf("%s%s%s", MOVECURS(msg->row, msg->col), msg->odraw, CLRTOEOL);
	doflush = 1;}
