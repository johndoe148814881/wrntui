#include "../include/info.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// local func defs
static info_t newi(int, int, int, char*, char*, frac_t*);
static void freei(info_t*);
static int cmpi(info_t*, info_t*);
static void drawi(info_t*);

// global vars
info_t** infos = 0; int ninfos = 0;

// global funcs
void infodraw(int row, int col, int cols, char* clr, char* title, frac_t* value) {
	info_t info = newi(row, col, cols, clr, title, value);

	// if arguments equal an existing info, use that one instead, else add a new entry to infos
	int isnew = 0;
	for (int i = 0; i < ninfos; ++i)
		if ((isnew = cmpi(&info, infos[i]) == 0)) {
			info_t oinfo = info;
			info = *infos[i];
			free(oinfo.odraw);
			break;}

	if (isnew) {
		infos = realloc(infos, ++ninfos * sizeof(info_t*));
		infos[ninfos - 1] = malloc(sizeof(info_t));
		*infos[ninfos - 1] = info;}

	drawi(&info);}

void infofreeall() {
	for (int i = 0; i < ninfos; ++i)
		freei(infos[i]);

	free(infos);
	infos = 0; ninfos = 0;}

// local funcs
static info_t newi(int row, int col, int cols, char* clr, char* title, frac_t* value) {
	char* odraw = malloc(cols + 1);
	memset(odraw, '\0', cols + 1);

	return (info_t){row, col, cols, clr, title, odraw, value, fracnew(0, 1)};}

static void freei(info_t* info) {
	free(info->odraw);
	free(info);
	info = 0;}

static int cmpi(info_t* a, info_t* b) {
	return !(a->row == b->row &&
		a->col == b->col &&
		a->cols == b->cols &&
		strcmp(a->clr, b->clr) == 0 &&
		strcmp(a->title, b->title) == 0 &&
		a->value == b->value);}

static void drawi(info_t* info) {
	if (fraccmp(info->value, &info->ovalue) == 0 && *info->odraw != 0)
		return;
	info->ovalue = *info->value;

	int len = snprintf(info->odraw, info->cols, "%s: %.17g", info->title, fractod(info->value));
	len = len < 0 ? 0 : len;
	len = len >= info->cols ? info->cols - 1 : len;

	memset(info->odraw + len, ' ', info->cols - 1 - len);
	
	printf("%s%.*s%s%s%s%s", 
			MOVECURS(info->row, info->col), 
			(int)strlen(info->title) + 2, info->odraw, 
			info->clr, 
			BOLD, 
			info->odraw + strlen(info->title) + 2, 
			CLRATTRS);
	doflush = 1;}
