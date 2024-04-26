#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translator.h"
#define streq(str1, str2, n) (strncmp(str1, str2, n) == 0)

static int lineCount = 0;
static int eqCount = 0;
static int gtCount = 0;
static int ltCount = 0;
static int andCount = 0;

void error(const char *message) {
    perror(message);
    exit(1);
}

void translate(char **lines) {
    char *line = malloc(MAX_LINE_LENGTH * sizeof(char));
    for (int i = 0; i < lineCount; i++) {
        line = lines[i];
        char *firstSpace = strchr(line, ' ');
        char *translatedInstruction = firstSpace == NULL ?
            translateArithmeticAndLogicalInstruction(line)
            : translateMemoryInstruction(line);
        printf("%s\n", translatedInstruction);
    }
}

char* translateArithmeticAndLogicalInstruction(char *instruction) {
    char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
    if (streq("add", instruction, 3))
        sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nM=M+D\n@SP\nM=M+1");
    else if (streq("sub", instruction, 3))
        sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nM=M-D\n@SP\nM=M+1");
    else if (streq("neg", instruction, 3))
        sprintf(translatedInstruction, "@SP\nAM=M-1\nM=-M\n@SP\nM=M+1");
    else if (streq("eq", instruction, 2)) {
        sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=M-D\n@TRUE_CONDITION_EQ_%d\nD;JEQ\n@SP\nA=M\nM=0\n(TRUE_CONDITION_EQ_%d)\n@SP\nA=M\nM=1\n@SP\nM=M+1", eqCount, eqCount);
        eqCount++;
    }
    else if (streq("gt", instruction, 2)) {
        sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=M-D\n@TRUE_CONDITION_GT_%d\nD;JGT\n@SP\nA=M\nM=0\n(TRUE_CONDITION_GT_%d)\n@SP\nA=M\nM=1\n@SP\nM=M+1", gtCount, gtCount);
        gtCount++;
    }
    else if (streq("lt", instruction, 2)) {
        sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=M-D\n@TRUE_CONDITION_LT_%d\nD;JLT\n@SP\nA=M\nM=0\n(TRUE_CONDITION_LT_%d)\n@SP\nA=M\nM=1\n@SP\nM=M+1", ltCount, ltCount);
        ltCount++;
    }
    else if (streq("and", instruction, 3)) {
        sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=D&M\n@TRUE_CONDITION_AND_%d\nD;JNE\n@SP\nA=M\nM=0\n(TRUE_CONDITION_AND_%d)\n@SP\nA=M\nM=1\n@SP\nM=M+1", andCount, andCount);
        andCount++;
    }
    else if (streq("not", instruction, 3))
        sprintf(translatedInstruction, "@SP\nAM=M-1\nM=!M\n@SP\nM=M+1");
    else error("[translateArithmeticAndLogicalInstruction] Invalid instruction");
    return translatedInstruction;
}

char* translateMemoryInstruction(char *instruction) {
    return instruction;
}

char** initialize(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) error("[initialize] File not found");
    char **lines;
    char buffer[100];

    if (lines == NULL) error("Memory allocation error");

    lines = (char **)malloc(MAX_LINES * sizeof(char *));
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = (char *)malloc(100 * sizeof(char));
        if (lines[i] == NULL) error("Memory allocation error");
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (lineCount < MAX_LINES) {
            size_t length = strlen(buffer);
            if (buffer[length - 1] == '\n')
                buffer[length - 1] = '\0';

            strcpy(lines[lineCount], buffer);
            lineCount++;
        } else error("Too many lines in the file. Increase MAX_LINES.\n");
    }

    return lines;
}

int main(int argc, char *argv[]) {
    if (argc < 2) error("[main] Not enough input arguments");
    const char *readFileName = argv[1];
    const char *writeFileName = argv[2];
    printf("\nTranslating %s...\n\n", readFileName);
    char **lines = initialize(readFileName);
    translate(lines);
    return 0;
}
