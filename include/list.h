#ifndef TUI_LIST_H
#define TUI_LIST_H
#define LISTINT 0
#define LISTFRAC 1

extern void listnew(int row, int column, int rows, int columns);
extern void listaddfield(int cols, char* colour, char* name, int typeidentifier);
extern void listaddrecord(void** values);
extern void listdelrecord(void* firstvalue);
extern void listbindselect(void (*func)(int valuec, void** recordvaluev));
extern void listdraw();
extern void listfree();

#endif

