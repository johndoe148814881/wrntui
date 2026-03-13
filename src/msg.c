#include "../include/msg.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// local type defs
typedef struct {
	int row;
	int col;
	int cols;
	char* buf;
	char* odraw;} msg_t;

// local vars
static msg_t** msgv = 0; static int msgc = 0;

// local func defs
static msg_t* newmsg(int, int, int, char*);
static void delmsg(msg_t*);
static int cmpmsg(msg_t*, msg_t*);
static msg_t* getexistingmsg(msg_t*);
static int updatemsg(msg_t*);
static void drawmsg(msg_t*);

// global funcs
void msgnew(int row, int col, int cols, char* buf) {
	pthread_mutex_lock(&tuiflushmutex);
	
	msg_t* msg = newmsg(row, col, cols, buf);

	msg_t* duplicate = getexistingmsg(msg);
	if (duplicate)
		delmsg(msg);

	pthread_mutex_unlock(&tuiflushmutex);}

void msgdrawall() {
	pthread_mutex_lock(&tuiflushmutex);

	for (int i = 0; i < msgc; ++i)
		if (!updatemsg(msgv[i]))
			drawmsg(msgv[i]);

	pthread_mutex_unlock(&tuiflushmutex);}

void msgfreeall() {
	pthread_mutex_lock(&tuiflushmutex);

	for (; msgc > 0;)
		delmsg(msgv[0]);

	pthread_mutex_unlock(&tuiflushmutex);}

// local funcs
static msg_t* newmsg(int row, int col, int cols, char* buf) {
	char* odraw = malloc(cols + 1);
	if (!odraw) {
		abort();
		return 0;}
	memset(odraw, '\0', cols + 1);
	
	msg_t* msg = malloc(sizeof(msg_t));
	if (!msg) {
		abort();
		return 0;}
	msg->row = row;
	msg->col = col;
	msg->cols = cols;
	msg->buf = buf;
	msg->odraw = odraw;

	msgv = realloc(msgv, ++msgc * sizeof(msg_t*));
	if (!msgv) {
		abort();
		return 0;}
	msgv[msgc - 1] = msg;

	return msg;}

static void delmsg(msg_t* msg) {
	int msgi = -1;
	for (int i = 0; i < msgc; ++i) { 
		if (msgv[i] == msg && msgi == -1)
			msgi = i;
		if (i > msgi && msgi != -1)
			msgv[i - 1] = msgv[i];}
	if (msgi == -1)
		return;

	free(msg->odraw);
	free(msg);
	
	msgv = realloc(msgv, --msgc * sizeof(msg_t*));
	if (!msgv && msgc > 0) {
		abort();
		return;}}

static int cmpmsg(msg_t* a, msg_t* b) {
	return !(a->row == b->row &&
		a->col == b->col &&
		a->cols == b->cols &&
		a->buf == b->buf);}

static msg_t* getexistingmsg(msg_t* msg) {
	for (int i = 0; i < msgc; ++i)
		if (cmpmsg(msg, msgv[i]) == 0)
			return msgv[i];

	return 0;}

static int updatemsg(msg_t* msg) {
	char draw[msg->cols + 1];
	int len = (int)strnlen(msg->buf, msg->cols);
	memcpy(draw, msg->buf, len);
	memset(draw + len, ' ', msg->cols - len);
	draw[msg->cols] = '\0';

	if (strcmp(draw, msg->odraw) != 0 || *msg->odraw == 0) {
		strcpy(msg->odraw, draw);
		return 0;}
	return 1;}

static void drawmsg(msg_t* msg) {
	printf("%s%s", MOVECURS(msg->row, msg->col), msg->odraw);}

