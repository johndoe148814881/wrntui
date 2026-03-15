#include "../include/list.h"
#include "../include/tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// local type defs
typedef struct {
	int id;
	void** valuev;
	void** ovaluev;
	char** valueodraws;} record_t;

typedef struct { 
	int cols;
	int typeid;
	char* clr;
	char* name;} field_t;

typedef struct {
	int row;
	int col;
	int rows;
	int cols;
	int recordc;
	int fieldc;
	record_t** recordv;
	field_t** fieldv;
	char** fieldodraws;} list_t;

// local vars
static int lastid = 0;
static list_t list = {0};

// local func defs
static void resetlist(int, int, int, int);
static void addfield(int, char*, char*, int);
static void addrecord(void**);
static void delrecord(record_t*);
static int updatelist();
static void drawlist();

// global funcs
void listnew(int row, int col, int rows, int cols) {
	pthread_mutex_lock(&tuiflushmutex);
	resetlist(row, col, rows, cols);
	pthread_mutex_unlock(&tuiflushmutex);}

void listaddfield(int cols, char* clr, char* name, int typeid) {
	pthread_mutex_lock(&tuiflushmutex);
	addfield(cols, clr, name, typeid);
	pthread_mutex_unlock(&tuiflushmutex);}

void listaddrecord(void** values) {
	pthread_mutex_lock(&tuiflushmutex);
	addrecord(values);
	pthread_mutex_unlock(&tuiflushmutex);}

void listdelrecord(void* firstvalue) {
	record_t* record = 0;
	for (int i = 0; i < list.recordc; ++i) {
		if (list.recordv[i]->valuev[0] == firstvalue) {
			record = list.recordv[i];
			break;}}
	if (record) {
		pthread_mutex_lock(&tuiflushmutex);
		delrecord(record);
		pthread_mutex_unlock(&tuiflushmutex);}}

void listdraw() {
	pthread_mutex_lock(&tuiflushmutex);
	if (!updatelist())
		drawlist();
	pthread_mutex_unlock(&tuiflushmutex);}

void listfree() {
	pthread_mutex_lock(&tuiflushmutex);
	resetlist(0, 0, 0, 0);
	pthread_mutex_unlock(&tuiflushmutex);}

// local funcs
static void resetlist(int row, int col, int rows, int cols) {
	// free fieldv and fieldodraws
	for (int i = 0; i < list.fieldc; ++i) {
		free(list.fieldv[i]);
		free(list.fieldodraws[i]);}
	free(list.fieldv);
	free(list.fieldodraws);
	list.fieldv = 0;
	list.fieldodraws = 0;

	// free recordv
	for (int i = 0; i < list.recordc; ++i) {
		for (int ii = 0; ii < list.fieldc; ++ii) {
			free(list.recordv[i]->valueodraws[ii]);
			free(list.recordv[i]->ovaluev[ii]);}
		free(list.recordv[i]->ovaluev);
		free(list.recordv[i]->valueodraws);
		free(list.recordv[i]);}
	free(list.recordv);
	list.recordv = 0;
	
	// set list stack values
	list.row = row;
	list.col = col;
	list.rows = rows;
	list.cols = cols;
	list.fieldc = 0;
	list.recordc = 0;}

static void addfield(int cols, char* clr, char* name, int typeid) {
	// allocate memory
	field_t* field = malloc(sizeof(field_t));
	list.fieldv = realloc(list.fieldv, (list.fieldc + 1) * sizeof(field_t*));
	list.fieldodraws = realloc(list.fieldodraws, (list.fieldc + 1) * sizeof(char*));

	// validate allocated memory
	if (!field || !list.fieldv || !list.fieldodraws) 
		abort();

	// add field to fieldv
	list.fieldv[list.fieldc++] = field;

	// allocate child memory
	list.fieldodraws[list.fieldc - 1] = malloc(cols);

	// validate child memory
	if (!list.fieldodraws[list.fieldc - 1])
		abort();

	// set child memory to identity value
	memset(list.fieldodraws[list.fieldc - 1], 0, cols);

	// set field stack values
	field->cols = cols;
	field->clr = clr;
	field->name = name;
	field->typeid = typeid;}

static void addrecord(void** values) {
	// allocate memory
	record_t* record = malloc(sizeof(record_t));
	list.recordv = realloc(list.recordv, (list.recordc + 1) * sizeof(record_t*));
	
	// validate allocated memory
	if (!record || !list.recordv) 
		abort();
	
	// add record to recordv
	list.recordv[list.recordc++] = record;

	// allocate children memory
	record->valuev = malloc(sizeof(void*) * list.fieldc);
	record->ovaluev = malloc(sizeof(void*) * list.fieldc);
	record->valueodraws = malloc(sizeof(char*) * list.fieldc);

	// validate allocated children memory
	if (!record->valuev || !record->ovaluev || !record->valueodraws) 
		abort();

	// allocate children memory of children, and/or set their values
	for (int i = 0; i < list.fieldc; ++i) {
		// valuev - set to parsed arguments
		record->valuev[i] = values[i];
		
		// ovaluev - allocate enough memory depending on each field's type, and set their identity values
		switch (list.fieldv[i]->typeid) {
		case LISTINT:
			record->ovaluev[i] = malloc(sizeof(int));
			if (!record->ovaluev[i]) 
				abort();
			*(int*)record->ovaluev[i] = 0;
			break;
		case LISTFRAC:
			record->ovaluev[i] = malloc(sizeof(frac_t));
			*(frac_t*)record->ovaluev[i] = fracnew(0, 1);
			break;}

		// valueodraws - allocate enough memory depending on each field's cols, and set their identity values
		record->valueodraws[i] = malloc(list.fieldv[i]->cols);
		memset(record->valueodraws[i], 0, list.fieldv[i]->cols);
		
		// validate allocated children memory
		if (!record->ovaluev[i] || !record->valueodraws[i]) 
			abort();}
	
	// set record stack values
	record->id = ++lastid;}

static void delrecord(record_t* record) {
	// bring following records down one place in recordv
	int recordi = -1;
	for (int i = 0; i < list.recordc; ++i) { 
		if (list.recordv[i] == record && recordi == -1) 
			recordi = i;
		if (i > recordi && recordi != -1)
			list.recordv[i - 1] = list.recordv[i];}
	
	// validate record exists
	if (recordi == -1)
		abort();
	
	// free allocated memory
	for (int i = 0; i < list.fieldc; ++i) {
		free(record->ovaluev[i]);
		free(record->valueodraws[i]);}
	free(record->ovaluev);
	free(record->valueodraws);
	free(record);
	
	// reallocate recordv to remove the duplicate record at the end
	if (list.recordc > 1) {
		list.recordv = realloc(list.recordv, --list.recordc * sizeof(record_t*));
		
		// validate reallocated memory
		if (!list.recordv && list.recordc > 0) 
			abort();}
	else {
		free(list.recordv);
		list.recordv = 0;}}

static int updatelist() {
	int updated = 1;

	// return if no fields
	if (list.fieldc < 1)
		return 1;
	
	// update record odraws
	for (int i = 0; i < list.recordc; ++i) {
	for (int ii = 0; ii < list.fieldc; ++ii) {
		record_t* record = list.recordv[i];
		field_t* field = list.fieldv[ii];

		// allocate memory for valuedraw
		char* valuedraw = malloc(field->cols);
		
		// validate allocated memory
		if (!valuedraw) 
			abort();
		
		// set valuedraw
		memset(valuedraw, 0, field->cols);
		switch (field->typeid) {
		case LISTINT:
			snprintf(valuedraw, field->cols, "%d", *(int*)record->ovaluev[ii]);
			break;
		case LISTFRAC:
			snprintf(valuedraw, list.fieldv[ii]->cols, "%.17g", fractod((frac_t*)record->ovaluev[ii]));
			break;}
		
		// update odraw and ovalues if valuedraw is different
		if (strcmp(valuedraw, record->valueodraws[ii]) != 0) {
			free(record->valueodraws[ii]);
			record->valueodraws[ii] = valuedraw;
			
			switch (field->typeid) {
			case LISTINT: 
				*(int*)record->ovaluev[ii] = *(int*)record->valuev[ii];
				break;	
			case LISTFRAC:
				*(frac_t*)record->ovaluev[ii] = *(frac_t*)record->valuev[ii];
				break;}

			updated = 0;}
		else 
			free(valuedraw);}}

	// update fieldodraws
	if (**list.fieldodraws == 0)
		for (int i = 0; i < list.fieldc; ++i) {
			snprintf(list.fieldodraws[i], list.fieldv[i]->cols, "%s", list.fieldv[i]->name);
			updated = 0;}

	return updated;}

static void drawlist() {
	for (int i = 0; i < list.recordc; ++i) {
	int coloffset = 0;
	for (int ii = 0; ii < list.fieldc; ++ii) {
		field_t* field = list.fieldv[ii];
		record_t* record = list.recordv[i];
	
		// draw formatted list fieldodraws as field titles
		if (i == 0) {
			char* fielddraw = malloc(field->cols + 1);
			int len = sprintf(fielddraw, "%.*s", field->cols, list.fieldodraws[ii]);
			memset(fielddraw + len, 0, field->cols - len + 1);

			char* whitespaces = malloc(field->cols - len + 1);
			memset(whitespaces, ' ', field->cols - len);
			whitespaces[field->cols - len] = 0;

			printf("%s%s%s%s%s%s", MOVECURS(list.row, list.col + coloffset), field->clr, BOLD, fielddraw, CLRATTRS, whitespaces);
			free(fielddraw);
			free(whitespaces);}
		
		// draw formatted record valueodraws as fields
		char* valuedraw = malloc(field->cols + 1);
		int len = sprintf(valuedraw, "%.*s", field->cols, record->valueodraws[ii]);
		memset(valuedraw + len, ' ', field->cols - len);
		valuedraw[field->cols] = 0;

		printf("%s%s%s%s", MOVECURS(list.row + 1 + i, list.col + coloffset), field->clr, valuedraw, CLRATTRS);
		free(valuedraw);

		coloffset += field->cols;}}}
