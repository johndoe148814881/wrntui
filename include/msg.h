#ifndef TUI_MSG_H
#define TUI_MSG_H

typedef struct {
	int row;
	int col;
	int cols;
	char* buf;
	char* odraw;} msg_t;

extern msg_t** msgs; extern int nmsgs;

extern void msgdraw(int, int, int, char*);
extern void msgfreeall();

#endif
