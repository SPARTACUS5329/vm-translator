#pragma once
#define MAX_FILE_NAME_LENGTH 20
#define SEGMENT_REPRESENTATION_TABLE_SIZE 10
#define MAX_LINES 500
#define MAX_LINE_LENGTH 20
#define MAX_ASSEMBLY_LINE_LENGTH 300 

typedef struct {
	int key;
	char data[10];
} lookup_table_item_t;

lookup_table_item_t *segmentRepresentationTable[SEGMENT_REPRESENTATION_TABLE_SIZE];

void error(const char *);
void translate(char **lines);
char* translateArithmeticAndLogicalInstruction(char *instruction);
char* translateMemoryInstruction(char *line);
char** initialize(const char *);
void insert(char *key, char *data);
lookup_table_item_t* search(char *key);

