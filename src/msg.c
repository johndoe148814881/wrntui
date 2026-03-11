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
	msg_t msg = newm(row, col, cols, buf);

	// if arguments equal an existing msg, use that one instead, else add a new entry to msgs
	int isnew = 0;
	for (int i = 0; i < nmsgs; ++i)
		if ((isnew = cmpm(&msg, msgs[i]) == 0)) {
			msg_t omsg = msg;
			msg = *msgs[i];
			free(omsg.odraw);
			break;}

	if (isnew) {
		msgs = realloc(msgs, ++nmsgs * sizeof(msg_t*));
		msgs[nmsgs - 1] = malloc(sizeof(msg_t));
		*msgs[nmsgs - 1] = msg;}

	drawm(&msg);}

void msgfreeall() {
	for (int i = 0; i < nmsgs; ++i)
		freem(msgs[i]);

	free(msgs);
	msgs = 0; nmsgs = 0;}

// local funcs
static msg_t newm(int row, int col, int cols, char* buf) {
	char* odraw = malloc(cols + 1);
	memset(odraw, '\0', cols + 1);

	return (msg_t){row, col, cols, buf, odraw};}

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
	char draw[msg->cols + 1];
	strcpy(draw, msg->buf);
	memset(draw + strlen(msg->buf), ' ', msg->cols - strlen(msg->buf));
	draw[msg->cols] = '\0';

	if (strcmp(draw, msg->odraw) == 0 && *msg->odraw != 0)
		return;
	strcpy(msg->odraw, draw);

	printf("%s%s", MOVECURS(msg->row, msg->col), msg->odraw);
	doflush = 1;}
