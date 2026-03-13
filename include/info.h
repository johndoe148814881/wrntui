#ifndef TUI_INFO_H
#define TUI_INFO_H
#include "/home/wrn/projs/wrnfrac/include/frac.h"
#define INFOINT 0
#define INFOFRAC 1

extern void infonew(int row, int column, int columns, char* colour, char* name, void* value, int typeidentifier);
extern void infodrawall();
extern void infofreeall();

#endif
