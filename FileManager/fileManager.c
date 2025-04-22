#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "fileManager.h"

int initializeFileData(FileData* fileData, FILE* source){
    fileData->quantLines = countLines(source);

    if(fileData->quantLines <= 0){
        fileData->lines = NULL;
        printf("Empty file.\n");
        return 0;
    }

    fileData->lines = (FileLine*)malloc(sizeof(FileLine) * fileData->quantLines);
    if(!fileData->lines){
        printf("Failed to alloc memory.\nfile.c:fileData->lines.\n");
        return 0;
    }

    for(int i = 0; i < fileData->quantLines; i++){
        fileData->lines[i].original = (char*)malloc(countChar(source, i) + 1);
        fileData->lines[i].binary = NULL;

        if(!fileData->lines[i].original){
            printf("Failed to alloc memory.\nfile.c:fileData->lines[%d].original.\n", i);
        
            for(int j = 0; j < i; j++){
                free(fileData->lines[j].binary);
            }
            free(fileData->lines);
            return 0;
        }
    }
}

int countLines(FILE *file){
    int quantLines = 0;
    char ch;
    
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') quantLines++;
    }
    quantLines++;
    
    rewind(file);
    return quantLines;
}

int countChar(FILE *file, int lineTarget){
    int quantchar = 0;
    int currentLine = 0;
    char ch;

    while(currentLine < lineTarget && (ch = fgetc(file)) != EOF){
        if(ch == '\n') currentLine++;
    }

    while((ch = fgetc(file)) != EOF && ch != '\n'){
        quantchar++;
    }

    rewind(file);

    return quantchar;
}

void tokenizeLine(FileLine* line){
    if (!line || !line->original) return;

    char* copy = strdup(line->original);
    if (!copy) return;

    int count = 0;
    char* temp = strtok(copy, " ,");

    while (temp != NULL){
        count++;
        temp = strtok(NULL, " ,");
    }
    free(copy);

    line->tokens = (char**)malloc(count * sizeof(char*));
    if (!line->tokens) return;

    copy = strdup(line->original);
    for (int i = 0; i < count; i++) {
        if (i == 0) {
            line->tokens[i] = strtok(copy, " ,");
        } else {
            line->tokens[i] = strtok(NULL, " ,");
        }
    }
    line->numTokens = count;
}

void readFile(char* fileName, FileData* fileData){
    FILE* file = fopen(fileName, "r"); //TODO: Tratar possíveis casos de nomes errados passados para essa função
    char buffer[1024];
    int currentLine = 0;

    if(!file){
        printf("Error opening input file");
        return;
    }

    if(!initializeFileData(fileData, file)){
        printf("Failed to initialize struct 'FileData'.\n");
        return;
    }
    
    rewind(file);
    while (fgets(buffer, sizeof(buffer), file) && currentLine < fileData->quantLines){
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        
        fileData->lines[currentLine].original = strdup(buffer);
        tokenizeLine(&fileData->lines[currentLine]);
        currentLine++;
    }

    fclose(file);
}

void printFileData(FileData fileData){
    for(int i = 0; i < fileData.quantLines; i++){
        printf("%s\n", fileData.lines[i]);
    }
}

void writeBinaryOutput(const char* filename, FileData* fileData){
    if (!filename || !fileData) return;
    
    FILE* output = fopen(filename, "wb");
    if (!output) {
        perror("Error opening output file");
        return;
    }
    
    for (int i = 0; i < fileData->quantLines; i++) {
        if (fileData->lines[i].binary && strlen(fileData->lines[i].binary) == 32) {
            unsigned int instruction = 0;
            
            for (int j = 0; j < 32; j++) {
                if (fileData->lines[i].binary[j] == '1') {
                    instruction |= (1U << (31 - j));
                }
            }
            
            uint8_t bytes[4];
            bytes[0] = (instruction >> 0) & 0xFF;
            bytes[1] = (instruction >> 8) & 0xFF;
            bytes[2] = (instruction >> 16) & 0xFF;
            bytes[3] = (instruction >> 24) & 0xFF;
            
            fwrite(bytes, sizeof(bytes), 1, output);
        }
    }
    
    fclose(output);
}

void freeFileData(FileData* fileData){
    for (int i = 0; i < fileData->quantLines; i++){
        free(fileData->lines[i].original);
        
        if(fileData->lines[i].tokens){
            free(fileData->lines[i].tokens[0]);
            free(fileData->lines[i].tokens);
        }

        if(fileData->lines[i].binary) free(fileData->lines[i].binary);
    }
    free(fileData->lines);
}