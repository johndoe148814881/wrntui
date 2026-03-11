#ifndef TUI_INFO_H
#define TUI_INFO_H
#include <frac.h>

typedef struct {
	int row;
	int col;
	int cols;
	char* clr;
	char* title;
	char* odraw;
	frac_t* value;
	frac_t ovalue;} info_t;

extern info_t** infos; extern int ninfos;

extern void infodraw(int, int, int, char*, char*, frac_t*);
extern void infofreeall();

#endif
