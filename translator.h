#pragma once
#define MAX_LINES 500
#define MAX_LINE_LENGTH 20
#define MAX_ASSEMBLY_LINE_LENGTH 300 

void error(const char *);
void translate(char **lines);
char* translateArithmeticAndLogicalInstruction(char *instruction);
char* translateMemoryInstruction(char *line);
char** initialize(const char *);
char* handleAdd(char *);
char* handleSub(char *);
char* handleNeg(char *);
char* handleEq(char *);
char* handleGt(char *);
char* handleLt(char *);
char* handleAnd(char *);
char* handleNot(char *);
