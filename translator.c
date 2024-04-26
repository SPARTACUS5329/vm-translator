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
static char staticPrefix[MAX_FILE_NAME_LENGTH];

void error(const char *message) {
    perror(message);
    exit(1);
}

char** translate(char **lines) {
    char *line = malloc(MAX_LINE_LENGTH * sizeof(char));
    char *translatedInstruction;
    char **instructions = malloc(lineCount * sizeof(char *));
    for (int i = 0; i < lineCount; i++) {
        instructions[i] = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
        line = lines[i];
        if (streq("push", line, 4) || streq("pop", line, 3))
            translatedInstruction = translateMemoryInstruction(line);
        else translatedInstruction = translateArithmeticAndLogicalInstruction(line);
        printf("%s\n", translatedInstruction);
        sprintf(instructions[i], "%s", translatedInstruction);
    }
    return instructions;
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
    char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
    char operation[4];
    char segment[10];
    int n;

    if (sscanf(instruction, "%s %s %d", operation, segment, &n) != 3) error("[translateMemoryInstruction] Invalid instruction");
    if (streq("push", operation, 4)) {
        if (streq("local", segment, 5) || streq("argument", segment, 8) || streq("this", segment, 4) || streq("that", segment, 4) || streq("temp", segment, 4)) {
            lookup_table_item_t *item = search(segment);
            sprintf(translatedInstruction, "@%d\nD=A\n@%s\nA=D+A\nD=M\n@SP\nM=D\nM=M+1", n, item->data);
        }
        else if (streq("constant", segment, 8))
            sprintf(translatedInstruction, "@%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1", n);
        else if (streq("static", segment, 6))
            sprintf(translatedInstruction, "@%s.%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1", staticPrefix, n);
        else if (streq("pointer", segment, 7)) {
            char *address;
            switch (n) {
                case 0:
                    strncpy(address, "THIS", 4);
                    break;
                case 1:
                    strncpy(address, "THAT", 4);
                    address = "THAT";
                    break;
                default:
                    error("[translateMemoryInstruction] Invalid pointer value");
            }
            sprintf(translatedInstruction, "@%s\nD=A\n@SP\nM=D\nM=M+1", address);
        }
        else error("[translateMemoryInstruction] Invalid memory segment");
    }
    else if (streq("pop", operation, 3)) {
        if (streq("local", segment, 5) || streq("argument", segment, 8) || streq("this", segment, 4) || streq("that", segment, 4) || streq("temp", segment, 4)) {
            lookup_table_item_t *item = search(segment);
            sprintf(translatedInstruction, "@%d\nD=A\n@%s\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D", n, item->data);
        }
        else if (streq("static", segment, 6))
            sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@%s.%d\nM=D", staticPrefix, n);
        else if (streq("pointer", segment, 7)) {
            char *address;
            switch (n) {
                case 0:
                    strncpy(address, "THIS", 4);
                    break;
                case 1:
                    strncpy(address, "THAT", 4);
                    address = "THAT";
                    break;
                default:
                    error("[translateMemoryInstruction] Invalid pointer value");
            }
            sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@%s\nM=D", address);
        }
        else error("[translateMemoryInstruction] Invalid memory segment");
    }
    else error("[translateMemoryInstruction] Invalid memory operation");
    return translatedInstruction;
}

char** initialize(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) error("[initialize] File not found");

    if (sscanf(fileName, "%[^.]", staticPrefix) != 1) error("[initialize] Invalid file naming convention");
    printf("staticPrefix: %s\n", staticPrefix);

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

    insert("local", "LCL");
    insert("argument", "ARG");
    insert("this", "THIS");
    insert("that", "THAT");
    insert("temp", "5");

    return lines;
}

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % SEGMENT_REPRESENTATION_TABLE_SIZE;
}

lookup_table_item_t *search(char *key) {
    int hashIndex = hash(key);

    while(segmentRepresentationTable[hashIndex] != NULL) {
        if(segmentRepresentationTable[hashIndex]->key == hashIndex)
            return segmentRepresentationTable[hashIndex]; 
			
        ++hashIndex;
        hashIndex %= SEGMENT_REPRESENTATION_TABLE_SIZE;
    }        

    return NULL;        
}

void insert(char *key, char *data) {
    lookup_table_item_t *item = (lookup_table_item_t *) malloc(sizeof(lookup_table_item_t));
    int hashIndex = hash(key);
    item->key = hashIndex;
    strcpy(item->data, data);
    while(segmentRepresentationTable[hashIndex] != NULL) {
        ++hashIndex;
        hashIndex %= SEGMENT_REPRESENTATION_TABLE_SIZE;
    }
    segmentRepresentationTable[hashIndex] = item;
}

void writeToFile(char **instructions, const char *fileName) {
    printf("Reaching\n");
    FILE *file;

    file = fopen(fileName, "w");
    if (file == NULL) error("Error in opening out file");
    printf("Writing to %s ...\n\n", fileName);

    for (int i = 0; i < lineCount; i++) {
        fprintf(file, "%s\n", instructions[i]);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) error("[main] Not enough input arguments");
    const char *readFileName = argv[1];
    const char *writeFileName = argv[2];
    printf("\nTranslating %s...\n\n", readFileName);
    char **lines = initialize(readFileName);
    char **instructions = translate(lines);
    writeToFile(instructions, writeFileName);
    printf("Successfully translated!\n");
    return 0;
}
