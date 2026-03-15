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
static void newmsg(int, int, int, char*);
static void delmsg(msg_t*);
static void delallmsgs();
static int updatemsg(msg_t*);
static void drawmsg(msg_t*);

// global funcs
void msgnew(int row, int col, int cols, char* buf) {
	pthread_mutex_lock(&tuiflushmutex);
	newmsg(row, col, cols, buf);
	pthread_mutex_unlock(&tuiflushmutex);}

void msgdrawall() {
	pthread_mutex_lock(&tuiflushmutex);
	for (int i = 0; i < msgc; ++i)
		if (!updatemsg(msgv[i]))
			drawmsg(msgv[i]);
	pthread_mutex_unlock(&tuiflushmutex);}

void msgfreeall() {
	pthread_mutex_lock(&tuiflushmutex);
	delallmsgs();
	pthread_mutex_unlock(&tuiflushmutex);}

void msgfreealluser() {
	pthread_mutex_lock(&tuiflushmutex);
	for (; msgc > 2;)
		delmsg(msgv[2]);
	pthread_mutex_unlock(&tuiflushmutex);}

// local funcs
static void newmsg(int row, int col, int cols, char* buf) {
	char* odraw = malloc(cols + 1);
	msg_t* msg = malloc(sizeof(msg_t));
	msgv = realloc(msgv, ++msgc * sizeof(msg_t*));
	
	if (!odraw || !msg || !msgv) {
		abort();
		return;}
	
	memset(odraw, '\0', cols + 1);
	
	msg->row = row;
	msg->col = col;
	msg->cols = cols;
	msg->buf = buf;
	msg->odraw = odraw;

	msgv[msgc - 1] = msg;}

static void delmsg(msg_t* msg) {
	int msgi = -1;
	for (int i = 0; i < msgc; ++i) { 
		if (msgv[i] == msg && msgi == -1) 
			msgi = i;
		if (i > msgi && msgi != -1)
			msgv[i - 1] = msgv[i];}
	
	if (msgi == -1) {
		abort();
		return;}

	free(msg->odraw);
	free(msg);

	if (msgc > 1) {
		msgv = realloc(msgv, --msgc * sizeof(msg_t*));
		
		if (!msgv)
			abort();

		return;}
	free(msgv);
	msgv = 0;}

static void delallmsgs() {
	for (int i = 0; i < msgc; ++i) {
		free(msgv[i]->odraw);
		free(msgv[i]);}
	free(msgv);
	msgv = 0;
	msgc = 0;}

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

