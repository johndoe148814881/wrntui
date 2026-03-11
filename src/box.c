#include "../include/box.h"
#include "../include/tui.h"
#include <stdio.h>
#include <string.h>

void boxdraw(int row, int col, int rows, int cols, char* clr, char* title, int clear, int nopts, char** opts) {
	// clear area
	if (clear)
		for (int i = 0; i < rows - 2; ++i) {
			printf("%s%*s", MOVECURS(row + i + 1, col + 1), cols - 2, "");}

	// bars
	printf("%s%s", CLRATTRS, clr);
	for (int i = 0; i < cols - 2; ++i) {
		printf("%s─", MOVECURS(row, col + i + 1));
		printf("%s─", MOVECURS(row + rows - 1, col + i + 1));}
	
	for (int i = 0; i < rows - 2; ++i) {
		printf("%s│", MOVECURS(row + i + 1, col));
		printf("%s│", MOVECURS(row + i + 1, col + cols - 1));}
	
	// corners
	printf("%s┌", MOVECURS(row, col));
	printf("%s┐", MOVECURS(row, col + cols - 1));
	printf("%s└", MOVECURS(row + rows - 1, col));
	printf("%s┘", MOVECURS(row + rows - 1, col + cols - 1));	
	
	// title
	int elementsize = strlen(title) + 4;
	int offset = col + 1;
	if (elementsize <= cols) 
		printf("%s┤ %s%s%s ├", MOVECURS(row, offset), CLRATTRS, title, clr);
	offset += elementsize;
	
	// options
	if (nopts > 0)
		for (int i = 0; i < nopts; ++i) {
			elementsize = strlen(opts[i]) + 4;
			if (offset + elementsize <= cols)
				printf("%s┤ %s%s%c%s%s%s%s%s ├", MOVECURS(row, offset), FOREERR, BOLD, *opts[i], CLRATTRS, BOLD, opts[i] + 1, CLRATTRS, clr);
			offset += elementsize;}

	printf("%s", CLRATTRS);
	doflush = 1;}


