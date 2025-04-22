#ifndef RISCV_ASSEMBLER_H
#define RISCV_ASSEMBLER_H

#include "../FileManager/fileManager.h"

typedef enum{
    R_TYPE,
    I_TYPE,
    S_TYPE,
    B_TYPE,
    U_TYPE,
    INVALID_TYPE
} InstructionFormat;

typedef struct Instruction{
    char* name;
    InstructionFormat format;

    char* funct7; //7 bits
    char* imm; //tam variável: 12 bits (formato I), 13 bits (formato B), 20 bits (formato U/J)
    char* rs2; //5 bits (R, S, B)
    char* rs1; //5 bits (R, I, S, B)
    char* funct3; //3 bits
    char* rd; //5 bits (R, I, U, J)
    char* opcode; //7 bits
}Instruction;

void intializeInstruction(Instruction* instruction, FileLine source);
void instructionMalloc(Instruction* instruction);
void fillInstructionFields(Instruction* instruction, FileLine source);

int extractRegistrator(char* token);
void intToBinary(int number, char* stringTarget, int bits);

void buildBinaryInstruction(FileLine* fileLine, Instruction* instruction);

#endif