#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

typedef struct FileLine{
    char* original;
    char** tokens;
    int numTokens;

    char* binary;
}FileLine;

typedef struct FileData{
    FileLine* lines;
    int quantLines;
}FileData;

int initializeFileData(FileData* fileData, FILE* source);

int countLines(FILE *file);
int countChar(FILE *file, int lineTarget);

void tokenizeLine(FileLine* line);
void readFile(char* fileName, FileData* fileData);
void printFileData(FileData fileData);
void writeBinaryOutput(const char* filename, FileData* fileData);

void freeFileData(FileData* fileData);

#endif