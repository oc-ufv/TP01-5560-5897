#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../FileManager/fileManager.h"
#include "../Assembler/assembler.h"

int main(int argc, char* argv[]) {
    char* inputFilename = argv[1];
    char* outputFilename = argv[2];

    FileData fileData;
    readFile(inputFilename, &fileData);
    
    if (fileData.quantLines <= 0) {
        printf("Error: Empty file or file cannot be readed.\n");
        return 1;
    }

    for (int i = 0; i < fileData.quantLines; i++) {
        Instruction inst;
        intializeInstruction(&inst, fileData.lines[i]);
        fillInstructionFields(&inst, fileData.lines[i]);
        buildBinaryInstruction(&fileData.lines[i], &inst);
        
        free(inst.name);
        if (inst.funct7) free(inst.funct7);
        if (inst.imm) free(inst.imm);
        if (inst.rs2) free(inst.rs2);
        if (inst.rs1) free(inst.rs1);
        if (inst.funct3) free(inst.funct3);
        if (inst.rd) free(inst.rd);
        if (inst.opcode) free(inst.opcode);
    }

    writeBinaryOutput(outputFilename, &fileData);

    freeFileData(&fileData);

    return 0;
}