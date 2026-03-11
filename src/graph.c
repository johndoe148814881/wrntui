#include "../include/graph.h"
#include "../include/tui.h"

/*typedef struct {
	char* title;
	frac_t value;
	frac_t* valuep;
	unsigned int col;
	char* olddraw;} graph_t;

void graphdraw(graph_t* graph, box_t* parent) {
	char* draw = malloc(10 * parent->rows - 2);
	
	// move rest up
	for (unsigned int i = 1; i < parent->rows - 3; ++i)
		strncpy(draw + 10 * i, graph->olddraw + 10 * i, 10);
	
	// draw latest change
	snprintf(draw + 10 * (parent->rows - 3), 10, "%d", graph->value.num);

	// draw title
	strncpy(draw + 10 * (parent->rows - 2), graph->title, 10);
	
	// update olddraw
	graph->olddraw = draw;

	// blit
	for (unsigned int i = 0; i < parent->rows - 2; ++i) {
		char* row = malloc(10);
		strncpy(row, draw + 10 * i, 10);
		printf("%s%s", movecurs(parent->row + 1 + i, parent->col + 1 + 10 * graph->col), row);}}

int graphcmp(graph_t* a, graph_t* b) {
	if (!a || !b || !a->title || !b->title || !a->valuep || !b->valuep || !a->olddraw || !b->olddraw)
		return 0;

	return strcmp(a->title, b->title) == 0 &&
		fraccmp(&a->value, &b->value) &&
		fraccmp(a->valuep, b->valuep) &&
		a->col == b->col &&
		strcmp(a->olddraw, b->olddraw) == 0;}
*/
