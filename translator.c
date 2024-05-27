#include "translator.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define streq(str1, str2, n) (strncmp(str1, str2, n) == 0)

static int lineCount = 0;
static int fileCount = 0;
static int eqCount = 0;
static int gtCount = 0;
static int ltCount = 0;
static int andCount = 0;
static char staticPrefix[MAX_FILE_NAME_LENGTH];
static function_table_item_t *currentFunction;

void error(const char *message) {
  perror(message);
  exit(1);
}

char **translate(char **lines) {
  char *line = malloc(MAX_LINE_LENGTH * sizeof(char));
  char *translatedInstruction;
  char **instructions = malloc(lineCount * sizeof(char *));
  for (int i = 0; i < lineCount; i++) {
    instructions[i] = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
    line = lines[i];
    if (streq("push", line, 4) || streq("pop", line, 3))
      translatedInstruction = translateMemoryInstruction(line);
    else if (streq("label", line, 5) || streq("goto", line, 4) ||
             streq("if-goto", line, 7))
      translatedInstruction = translateBranchingInstruction(line);
    else if (streq("function", line, 8) || streq("call", line, 4))
      translatedInstruction = translateFunctionInstruction(line);
    else if (streq("return", line, 6))
      translatedInstruction = translateReturnInstruction(line);
    else
      translatedInstruction = translateArithmeticAndLogicalInstruction(line);
    sprintf(instructions[i], "%s", translatedInstruction);
  }
  return instructions;
}

char *translateArithmeticAndLogicalInstruction(char *instruction) {
  char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
  if (streq("add", instruction, 3))
    sprintf(
        translatedInstruction,
        "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nM=M+D\n@SP\nM=M+1");
  else if (streq("sub", instruction, 3))
    sprintf(
        translatedInstruction,
        "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nM=M-D\n@SP\nM=M+1");
  else if (streq("neg", instruction, 3))
    sprintf(translatedInstruction, "@SP\nAM=M-1\nM=-M\n@SP\nM=M+1");
  else if (streq("eq", instruction, 2)) {
    sprintf(translatedInstruction,
            "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=M-D\n@TRUE_"
            "CONDITION_EQ_%d\nD;JEQ\n@SP\nA=M\nM=0\n(TRUE_CONDITION_EQ_%d)\n@"
            "SP\nA=M\nM=1\n@SP\nM=M+1",
            eqCount, eqCount);
    eqCount++;
  } else if (streq("gt", instruction, 2)) {
    sprintf(translatedInstruction,
            "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=M-D\n@TRUE_"
            "CONDITION_GT_%d\nD;JGT\n@SP\nA=M\nM=0\n(TRUE_CONDITION_GT_%d)\n@"
            "SP\nA=M\nM=1\n@SP\nM=M+1",
            gtCount, gtCount);
    gtCount++;
  } else if (streq("lt", instruction, 2)) {
    sprintf(translatedInstruction,
            "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=M-D\n@TRUE_"
            "CONDITION_LT_%d\nD;JLT\n@SP\nA=M\nM=0\n(TRUE_CONDITION_LT_%d)\n@"
            "SP\nA=M\nM=1\n@SP\nM=M+1",
            ltCount, ltCount);
    ltCount++;
  } else if (streq("and", instruction, 3)) {
    sprintf(translatedInstruction,
            "@SP\nAM=M-1\nD=M\n@13\nM=D\n@SP\nAM=M-1\nD=M\n@13\nD=D&M\n@TRUE_"
            "CONDITION_AND_%d\nD;JNE\n@SP\nA=M\nM=0\n(TRUE_CONDITION_AND_%d)\n@"
            "SP\nA=M\nM=1\n@SP\nM=M+1",
            andCount, andCount);
    andCount++;
  } else if (streq("not", instruction, 3))
    sprintf(translatedInstruction, "@SP\nAM=M-1\nM=!M\n@SP\nM=M+1");
  else
    error("[translateArithmeticAndLogicalInstruction] Invalid instruction");
  return translatedInstruction;
}

char *translateBranchingInstruction(char *instruction) {
  char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
  char operation[7];
  char location[MAX_LABEL_SIZE];

  if (sscanf(instruction, "%s %s", operation, location) != 2)
    error("[translateBranchingInstruction] Invalid instruction");
  if (streq("label", instruction, 5))
    sprintf(translatedInstruction, "(%s)", location);
  else if (streq("goto", instruction, 4))
    sprintf(translatedInstruction, "@%s\nnull=null;JMP", location);
  else if (streq("if-goto", instruction, 7))
    sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M-1\n\n@%s\nnull=D;JEQ",
            location);
  else
    error("[translateBranchingInstruction] Invalid instruction");
  return translatedInstruction;
}

char *translateReturnInstruction(char *instruction) {
  char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
  sprintf(translatedInstruction,
          "@LCL\nD=M\n@13\nM=D\n@5\nD=D-A\n@14\nM=D\n@SP\nAM=M-1\nD=M\n@ARG\nM="
          "D\nD=M\n@SP\nM=D+1\n@13\nD=M\n@THAT\nMD=D-1\n@THIS\nMD=D-1\n@"
          "ARG\nMD=D-1\n@LCL\nMD=D-1\n@14\nA=M\nnull=null;JMP");
  return translatedInstruction;
}

char *translateFunctionInstruction(char *instruction) {
  char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
  char operation[7];
  char functionName[MAX_FILE_NAME_LENGTH];
  int n;

  if (sscanf(instruction, "%s %s %d", operation, functionName, &n) != 3)
    error("[translateFunctionInstruction] Invalid instruction");
  if (streq("function", instruction, 8)) {
    currentFunction = searchFunctionTable(functionName);
    sprintf(translatedInstruction,
            "(%s)\n@%d\nD=A\n@13\nM=D\n(%s_SET_LCL)\n@SP\nA=M\nM=0\n@SP\nM=M+"
            "1\n@13\nM=M-1\nD=M\n@%s_SET_LCL\nnull=D;JNE",
            functionName, n, functionName, functionName);

  } else if (streq("call", instruction, 4))
    sprintf(translatedInstruction,
            "@%s$ret.%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@LCL\nD=M\n@SP\nA="
            "M\nM=D\n@SP\nM=M+1\n@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@"
            "THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n@THAT\nD=M\n@SP\nA=M\nM=D\n@"
            "SP\nM=M+1\n@%d\nD=A\n@5\nD=A+D\n@SP\nD=M-D\n@ARG\nM=D\n@SP\nD=M\n@"
            "LCL\nM=D\n@%s\nnull=null;JMP\n(%s$ret.%d)",
            currentFunction->name, currentFunction->calls, n, functionName,
            currentFunction->name, currentFunction->calls++);
  else
    error("[translateFunctionInstruction] Invalid instruction");
  return translatedInstruction;
}

char *translateMemoryInstruction(char *instruction) {
  char *translatedInstruction = malloc(MAX_ASSEMBLY_LINE_LENGTH * sizeof(char));
  char operation[4];
  char segment[10];
  int n;

  if (sscanf(instruction, "%s %s %d", operation, segment, &n) != 3)
    error("[translateMemoryInstruction] Invalid instruction");
  if (streq("push", operation, 4)) {
    if (streq("local", segment, 5) || streq("argument", segment, 8) ||
        streq("this", segment, 4) || streq("that", segment, 4) ||
        streq("temp", segment, 4)) {
      lookup_table_item_t *item = searchSegmentTable(segment);
      sprintf(translatedInstruction,
              "@%d\nD=A\n@%s\nA=D+A\nD=M\n@SP\nM=D\nM=M+1", n, item->data);
    } else if (streq("constant", segment, 8))
      sprintf(translatedInstruction, "@%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1", n);
    else if (streq("static", segment, 6))
      sprintf(translatedInstruction, "@%s.%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1",
              staticPrefix, n);
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
    } else
      error("[translateMemoryInstruction] Invalid memory segment");
  } else if (streq("pop", operation, 3)) {
    if (streq("local", segment, 5) || streq("argument", segment, 8) ||
        streq("this", segment, 4) || streq("that", segment, 4) ||
        streq("temp", segment, 4)) {
      lookup_table_item_t *item = searchSegmentTable(segment);
      sprintf(
          translatedInstruction,
          "@%d\nD=A\n@%s\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D",
          n, item->data);
    } else if (streq("static", segment, 6))
      sprintf(translatedInstruction, "@SP\nAM=M-1\nD=M\n@%s.%d\nM=D",
              staticPrefix, n);
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
    } else
      error("[translateMemoryInstruction] Invalid memory segment");
  } else
    error("[translateMemoryInstruction] Invalid memory operation");
  return translatedInstruction;
}

char **initialize(const char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL)
    error("[initialize] File not found");

  char **lines;
  char buffer[100];

  lines = (char **)malloc(MAX_LINES * sizeof(char *));
  for (int i = 0; i < MAX_LINES; i++) {
    lines[i] = (char *)malloc(100 * sizeof(char));
    if (lines[i] == NULL)
      error("Memory allocation error");
  }

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    if (lineCount < MAX_LINES) {
      size_t length = strlen(buffer);
      if (buffer[length - 1] == '\n')
        buffer[length - 1] = '\0';

      strcpy(lines[lineCount++], buffer);
    } else
      error("Too many lines in the file. Increase MAX_LINES.\n");
  }

  insertSegmentTable("local", "LCL");
  insertSegmentTable("argument", "ARG");
  insertSegmentTable("this", "THIS");
  insertSegmentTable("that", "THAT");
  insertSegmentTable("temp", "5");

  return lines;
}

unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % SEGMENT_REPRESENTATION_TABLE_SIZE;
}

lookup_table_item_t *searchSegmentTable(char *key) {
  int hashIndex = hash(key);

  while (segmentRepresentationTable[hashIndex] != NULL) {
    if (segmentRepresentationTable[hashIndex]->key == hashIndex)
      return segmentRepresentationTable[hashIndex];

    ++hashIndex;
    hashIndex %= SEGMENT_REPRESENTATION_TABLE_SIZE;
  }

  return NULL;
}

void insertSegmentTable(char *key, char *data) {
  lookup_table_item_t *item =
      (lookup_table_item_t *)malloc(sizeof(lookup_table_item_t));
  int hashIndex = hash(key);
  item->key = hashIndex;
  strcpy(item->data, data);
  while (segmentRepresentationTable[hashIndex] != NULL) {
    ++hashIndex;
    hashIndex %= SEGMENT_REPRESENTATION_TABLE_SIZE;
  }
  segmentRepresentationTable[hashIndex] = item;
}

function_table_item_t *searchFunctionTable(char *key) {
  int hashIndex = hash(key);

  while (functionDataTable[hashIndex] != NULL) {
    if (functionDataTable[hashIndex]->key == hashIndex)
      return functionDataTable[hashIndex];

    ++hashIndex;
    hashIndex %= MAX_FUNCTIONS;
  }

  return NULL;
}

void insertFunctionTable(char *key, int args, int local) {
  function_table_item_t *item = searchFunctionTable(key);
  if (item == NULL)
    item = (function_table_item_t *)malloc(sizeof(function_table_item_t));
  else
    goto assignment;

  int hashIndex = hash(key);
  item->key = hashIndex;
  while (functionDataTable[hashIndex] != NULL) {
    ++hashIndex;
    hashIndex %= MAX_FUNCTIONS;
  }
  functionDataTable[hashIndex] = item;

assignment:
  if (args != -1)
    item->args = args;
  if (local != -1)
    item->local = local;
  item->calls = 0;
  strncpy(item->name, key, MAX_FILE_NAME_LENGTH);
}

void writeToFile(char **instructions, const char *fileName) {
  FILE *file;

  file = fopen(fileName, "w");
  if (file == NULL)
    error("Error in opening out file");
  printf("Writing to %s ...\n\n", fileName);

  for (int i = 0; i < lineCount; i++) {
    fprintf(file, "%s\n", instructions[i]);
  }

  fclose(file);
}

void firstPass(char **lines) {
  char *line = malloc(MAX_LINE_LENGTH * sizeof(char));
  for (int i = 0; i < lineCount; i++) {
    line = lines[i];
    if (streq("function", line, 8)) {
      char functionName[MAX_FILE_NAME_LENGTH];
      int args;
      sscanf(line, "function %s %d", functionName, &args);
      insertFunctionTable(functionName, args, -1);
    } else if (streq("call", line, 4)) {
      char functionName[MAX_FILE_NAME_LENGTH];
      int local;
      sscanf(line, "call %s %d", functionName, &local);
      insertFunctionTable(functionName, -1, local);
    }
  }
}

void translateFile(const char *readStream, const char *writeStream) {
  switch (sscanf(readStream, "%[^.]", staticPrefix)) {
    char *lastSlash;
    const char *lastDot;

  case 1:
    break;

  case 0:
    lastSlash = strrchr(readStream, '/');
    if (lastSlash == NULL)
      error("[initialize] Invalid directory-file format");
    lastSlash++;

    lastDot = strrchr(lastSlash, '.');
    if (lastDot == NULL || !streq(lastDot, ".vm", 3)) {
      printf("[translateFile] Skipping %s: Invalid file extension", readStream);
      return;
    }
    strncpy(staticPrefix, lastSlash, lastDot - lastSlash);
    staticPrefix[lastDot - lastSlash] = '\0';
    break;

  default:
    printf("[translateFile] Skipping %s: Invalid file naming convention",
           readStream);
    return;
  }

  char **lines = initialize(readStream);
  printf("\nTranslating %s...\n\n", readStream);
  firstPass(lines);
  char **instructions = translate(lines);
  writeToFile(instructions, writeStream);
  printf("Successfully translated!\n");
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    error("[main] Not enough input arguments");
  const char *readStream = argv[1];
  const char *writeStream = argv[2];
  const char *extensionStarting = strstr(readStream, "/");
  if (extensionStarting == NULL) {
    // handle single file case
    char *extension = strstr(readStream, ".");
    if (extension == NULL || !streq(extension, ".vm", 3))
      error("[main] Error in file naming convention");
    translateFile(readStream, writeStream);
  } else {
    // handle directory case
    DIR *dir = opendir(readStream);
    if (!dir)
      error("[main] Error in opening directory");

    struct dirent *entry;
    struct stat statbuf;
    char fullpath[1024];
    char **files;
    files = (char **)malloc(MAX_FILES * sizeof(char *));
    for (int i = 0; i < MAX_FILES; i++) {
      files[i] = (char *)malloc(100 * sizeof(char));
      if (files[i] == NULL)
        error("Memory allocation error");
    }

    while ((entry = readdir(dir)) != NULL) {
      if (streq(entry->d_name, ".", 1) || streq(entry->d_name, "..", 2))
        continue;

      sprintf(fullpath, "%s/%s", readStream, entry->d_name);

      if (stat(fullpath, &statbuf) == -1)
        error("[main] stat");

      strcpy(files[fileCount++], entry->d_name);
    }

    closedir(dir);
    for (int i = 0; i < fileCount; i++) {
      char readFile[MAX_FILE_NAME_LENGTH];
      sprintf(readFile, "%s", files[i]);
      char readFilePath[MAX_FILE_NAME_LENGTH];
      sprintf(readFilePath, "%s/%s", readStream, readFile);
      char *vmExtension = strstr(readFile, ".");
      char readFileName[MAX_FILE_NAME_LENGTH];
      strncpy(readFileName, readFile, vmExtension - readFile);
      readFileName[vmExtension - readFile] = '\0';
      char writeFilePath[MAX_FILE_NAME_LENGTH];
      sprintf(writeFilePath, "%s/%s.asm", readStream, readFileName);
      translateFile(readFilePath, writeFilePath);
    }
  }
  return 0;
}
