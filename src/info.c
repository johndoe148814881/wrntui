#include "../include/info.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// local type defs
typedef struct {
	int row;
	int col;
	int cols;
	int type;
	char* clr;
	char* name;
	char* odraw;
	void* value;
	void* ovalue;} info_t;

// local vars
static info_t** infov = 0; static int infoc = 0;

// local func defs
static void newinfo(int, int, int, char*, char*, void*, int);
static void delinfo(info_t*);
static int cmpinfo(info_t*, info_t*);
static info_t* getexistinginfo(int, int, int, char*, char*, void*, int);
static int updateinfo(info_t*);
static void drawinfo(info_t*);

// global funcs
void infonew(int row, int col, int cols, char* clr, char* name, void* value, int type) {
	info_t* duplicate = getexistinginfo(row, col, cols, clr, name, value, type);
	if (duplicate) {
		pthread_mutex_lock(&tuiflushmutex);
		newinfo(row, col, cols, clr, name, value, type);
		pthread_mutex_unlock(&tuiflushmutex);}}

void infodrawall() {
	pthread_mutex_lock(&tuiflushmutex);
	for (int i = 0; i < infoc; ++i)
		if (!updateinfo(infov[i]))
			drawinfo(infov[i]);
	pthread_mutex_unlock(&tuiflushmutex);}

void infofreeall() {
	pthread_mutex_lock(&tuiflushmutex);
	for (; infoc > 0;)
		delinfo(infov[0]);
	pthread_mutex_unlock(&tuiflushmutex);}

// local funcs
static void newinfo(int row, int col, int cols, char* clr, char* name, void* value, int type) {
	char* odraw = malloc(cols + 1);
	info_t* info = malloc(sizeof(info_t));
	infov = realloc(infov, ++infoc * sizeof(info_t*));
	
	if (!odraw || !info || !infov) {
		abort();
		return;}
	
	void* ovalue;
	switch (type) {
	case INFOINT: {
		int* ointvalue = malloc(sizeof(int));
		if (!ointvalue) {
			abort();
			return;}
		*ointvalue = 0;
		ovalue = (void*)ointvalue;
		break;}
	case INFOFRAC: {
		frac_t* ofracvalue = malloc(sizeof(frac_t));
		if (!ofracvalue) {
			abort();
			return;}
		*ofracvalue = fracnew(0, 1);
		ovalue = (void*)ofracvalue;
		break;}
	default:
		abort();
		return;}

	memset(odraw, '\0', cols + 1);
	
	info->row = row;
	info->col = col;
	info->cols = cols;
	info->clr = clr;
	info->name = name;
	info->odraw = odraw;
	info->value = value;
	info->ovalue = ovalue;
	info->type = type;

	infov[infoc - 1] = info;} 

static void delinfo(info_t* info) {
	int infoi = -1;
	for (int i = 0; i < infoc; ++i) { 
		if (infov[i] == info && infoi == -1)
			infoi = i;
		if (i > infoi && infoi != -1)
			infov[i - 1] = infov[i];}
	if (infoi == -1)
		return;

	free(info->odraw);
	free(info->ovalue);
	free(info);
	
	infov = realloc(infov, --infoc * sizeof(info_t*));
	if (!infov && infoc > 0) {
		abort();
		return;}}

static int cmpinfo(info_t* a, info_t* b) {
	return !(a->row == b->row &&
		a->col == b->col &&
		a->cols == b->cols &&
		strcmp(a->clr, b->clr) == 0 &&
		strcmp(a->name, b->name) == 0 &&
		a->value == b->value &&
		a->type == b->type);}

static info_t* getexistinginfo(int row, int col, int cols, char* clr, char* name, void* value, int type) {
	info_t info = (info_t){row, col, cols, type, clr, name, 0, value, 0};
	for (int i = 0; i < infoc; ++i)
		if (cmpinfo(&info, infov[i]) == 0)
			return infov[i];
	return 0;}

static int updateinfo(info_t* info) {
	int len;
	switch (info->type) {
	case INFOINT:
		if (*(int*)info->value == *(int*)info->ovalue && *info->odraw != 0)
			return 1;
		*(int*)info->ovalue = *(int*)info->value;
		len = snprintf(info->odraw, info->cols, "%s: %d", info->name, *(int*)info->value);
		break;
	case INFOFRAC:
		if (fraccmp((frac_t*)info->value, (frac_t*)info->ovalue) == 0 && *info->odraw != 0)
			return 1;
		*(frac_t*)info->ovalue = *(frac_t*)info->value;
		len = snprintf(info->odraw, info->cols, "%s: %.17g", info->name, fractod((frac_t*)info->value));
		break;
	default:
		return 1;}

	len = len < 0 ? 0 : len;
	len = len >= info->cols ? info->cols - 1 : len;
	memset(info->odraw + len, ' ', info->cols - 1 - len);

	return 0;}
	
static void drawinfo(info_t* info) {
	int namelen = (int)strlen(info->name) + 2;
	namelen = namelen >= info->cols ? info->cols : namelen;

	if (namelen != info->cols)
		printf("%s%.*s%s%s%s%s", 
			MOVECURS(info->row, info->col), 
			namelen, info->odraw, 
			info->clr, 
			BOLD, 
			info->odraw + namelen, 
			CLRATTRS);
	else 
		printf("%s%.*s%s", 
			MOVECURS(info->row, info->col), 
			namelen, info->odraw,   
			CLRATTRS);}

