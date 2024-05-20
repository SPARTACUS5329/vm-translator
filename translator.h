#pragma once
#define MAX_FILE_NAME_LENGTH 20
#define SEGMENT_REPRESENTATION_TABLE_SIZE 10
#define MAX_LINES 500
#define MAX_LINE_LENGTH 20
#define MAX_ASSEMBLY_LINE_LENGTH 300
#define MAX_LABEL_SIZE 50
#define MAX_FUNCTIONS 500

typedef struct {
  int key;
  char data[10];
} lookup_table_item_t;

typedef struct {
  int key;
  int args;
  int local;
  int calls;
  char name[MAX_FILE_NAME_LENGTH];
} function_table_item_t;

lookup_table_item_t
    *segmentRepresentationTable[SEGMENT_REPRESENTATION_TABLE_SIZE];
function_table_item_t *functionDataTable[MAX_FUNCTIONS];

void error(const char *);
char **translate(char **lines);
char *translateArithmeticAndLogicalInstruction(char *instruction);
char *translateMemoryInstruction(char *line);
char *translateBranchingInstruction(char *line);
char *translateFunctionInstruction(char *line);
char *translateReturnInstruction(char *line);
char **initialize(const char *);
void insertSegmentTable(char *key, char *data);
lookup_table_item_t *searchSegmentTable(char *key);
void insertFunctionTable(char *key, int args, int local);
function_table_item_t *searchFunctionTable(char *key);
void writeToFile(char **instructions, const char *fileName);
