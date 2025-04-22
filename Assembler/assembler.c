#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "assembler.h"
#include "../FileManager/fileManager.h"

void instructionMalloc(Instruction* instruction){
    instruction->name = NULL;
    
    // Alocação (7 bits + null terminator), esse padrão se repetirá em toda a implementação
    instruction->opcode = (char*)calloc(8, sizeof(char));
    if (!instruction->opcode) return;

    switch (instruction->format) {
    case R_TYPE:
        instruction->funct7 = (char*)calloc(8, sizeof(char));
        instruction->rs2 = (char*)calloc(6, sizeof(char));
        instruction->rs1 = (char*)calloc(6, sizeof(char));
        instruction->funct3 = (char*)calloc(4, sizeof(char));
        instruction->rd = (char*)calloc(6, sizeof(char));

        instruction->imm = NULL;
        break;
    case I_TYPE:
        instruction->imm = (char*)calloc(13, sizeof(char));
        instruction->rs1 = (char*)calloc(6, sizeof(char));
        instruction->funct3 = (char*)calloc(4, sizeof(char));
        instruction->rd = (char*)calloc(6, sizeof(char));

        instruction->funct7 = NULL;
        instruction->rs2 = NULL;
        break;

    case S_TYPE:
        instruction->imm = (char*)calloc(13, sizeof(char));
        instruction->rs2 = (char*)calloc(6, sizeof(char));
        instruction->rs1 = (char*)calloc(6, sizeof(char));
        instruction->funct3 = (char*)calloc(4, sizeof(char));

        instruction->funct7 = NULL;
        instruction->rd = NULL;
        break;

    case B_TYPE:
        instruction->imm = (char*)calloc(14, sizeof(char));
        instruction->rs2 = (char*)calloc(6, sizeof(char));
        instruction->rs1 = (char*)calloc(6, sizeof(char));
        instruction->funct3 = (char*)calloc(4, sizeof(char));

        instruction->funct7 = NULL;
        instruction->rd = NULL;
        break;

    case U_TYPE:
        instruction->imm = (char*)calloc(21, sizeof(char));
        instruction->rd = (char*)calloc(6, sizeof(char));

        instruction->funct7 = NULL;
        instruction->funct3 = NULL;
        instruction->rs2 = NULL;
        instruction->rs1 = NULL;
        break;

    default:
        printf("Invalid format.\n");
        break;
    }
}

void intializeInstruction(Instruction* instruction, FileLine source) {
    if (!instruction || !source.tokens || source.numTokens < 1) {
        printf("Error: Invalid input parameters\n");
        instruction->format = INVALID_TYPE;
        return;
    }

    // Alocar e copiar o nome da instrução
    instruction->name = strdup(source.tokens[0]);
    if (!instruction->name) {
        printf("Error: Failed to allocate memory for instruction name\n");
        instruction->format = INVALID_TYPE;
        return;
    }

    // Determinar o formato da instrução
    if (strcmp(instruction->name, "add") == 0 || 
        strcmp(instruction->name, "sub") == 0 ||
        strcmp(instruction->name, "xor") == 0 ||
        strcmp(instruction->name, "or") == 0 ||
        strcmp(instruction->name, "and") == 0 ||
        strcmp(instruction->name, "sll") == 0 ||
        strcmp(instruction->name, "srl") == 0 ||
        strcmp(instruction->name, "sra") == 0 ||
        strcmp(instruction->name, "slt") == 0 ||
        strcmp(instruction->name, "sltu") == 0 ||
        strcmp(instruction->name, "mul") == 0) {
        instruction->format = R_TYPE;
    }
    else if (strcmp(instruction->name, "addi") == 0 ||
             strcmp(instruction->name, "andi") == 0 ||
             strcmp(instruction->name, "ori") == 0 ||
             strcmp(instruction->name, "xori") == 0 ||
             strcmp(instruction->name, "slli") == 0 ||
             strcmp(instruction->name, "srli") == 0 ||
             strcmp(instruction->name, "srai") == 0 ||
             strcmp(instruction->name, "slti") == 0 ||
             strcmp(instruction->name, "sltiu") == 0 ||
             strcmp(instruction->name, "lb") == 0 ||
             strcmp(instruction->name, "lh") == 0 ||
             strcmp(instruction->name, "lw") == 0 ||
             strcmp(instruction->name, "lbu") == 0 ||
             strcmp(instruction->name, "lhu") == 0 ||
             strcmp(instruction->name, "jalr") == 0) {
        instruction->format = I_TYPE;
    }
    else if (strcmp(instruction->name, "sb") == 0 ||
             strcmp(instruction->name, "sh") == 0 ||
             strcmp(instruction->name, "sw") == 0) {
        instruction->format = S_TYPE;
    }
    else if (strcmp(instruction->name, "beq") == 0 ||
             strcmp(instruction->name, "bne") == 0 ||
             strcmp(instruction->name, "blt") == 0 ||
             strcmp(instruction->name, "bge") == 0 ||
             strcmp(instruction->name, "bltu") == 0 ||
             strcmp(instruction->name, "bgeu") == 0) {
        instruction->format = B_TYPE;
    }
    else if (strcmp(instruction->name, "lui") == 0 ||
             strcmp(instruction->name, "auipc") == 0) {
        instruction->format = U_TYPE;
    }
    else if (strcmp(instruction->name, "jal") == 0) {
        instruction->format = U_TYPE; // Jal usa formato U/J
    }
    else {
        printf("Error: Unknown instruction '%s'\n", instruction->name);
        instruction->format = INVALID_TYPE;
        free(instruction->name);
        instruction->name = NULL;
        return;
    }

    // Preencher os campos da instrução
    fillInstructionFields(instruction, source);
}
//nota mental: add, sub, xor, or, and, sll, srl, sra, slt, sltu
/*funct3: (add == sub) = 000 & (srl == sra) = 101
* xor = 100
* or = 110
* and = 111
* sll = 001
* slt = 010
* sltu = 011
*/
void fillInstructionFields(Instruction* instruction, FileLine source) {
    if (!instruction || instruction->format == INVALID_TYPE || !source.tokens) {
        printf("Error: Invalid instruction or source\n");
        return;
    }

    instructionMalloc(instruction);
    if (!instruction->opcode) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    switch (instruction->format) {
        case R_TYPE: {
            if (source.numTokens < 4) {
                printf("Error: Not enough tokens for R-type instruction\n");
                return;
            }

            strcpy(instruction->opcode, "0110011");
            intToBinary(extractRegistrator(source.tokens[1]), instruction->rd, 5);
            intToBinary(extractRegistrator(source.tokens[2]), instruction->rs1, 5);
            intToBinary(extractRegistrator(source.tokens[3]), instruction->rs2, 5);

            // Definir funct3 e funct7 baseados na instrução
            if (strcmp(instruction->name, "add") == 0 || strcmp(instruction->name, "sub") == 0) {
                strcpy(instruction->funct3, "000");
                strcpy(instruction->funct7, strcmp(instruction->name, "sub") == 0 ? "0100000" : "0000000");
            }
            else if (strcmp(instruction->name, "sll") == 0) {
                strcpy(instruction->funct3, "001");
                strcpy(instruction->funct7, "0000000");
            }
            else if (strcmp(instruction->name, "slt") == 0) {
                strcpy(instruction->funct3, "010");
                strcpy(instruction->funct7, "0000000");
            }
            else if (strcmp(instruction->name, "sltu") == 0) {
                strcpy(instruction->funct3, "011");
                strcpy(instruction->funct7, "0000000");
            }
            else if (strcmp(instruction->name, "xor") == 0) {
                strcpy(instruction->funct3, "100");
                strcpy(instruction->funct7, "0000000");
            }
            else if (strcmp(instruction->name, "srl") == 0 || strcmp(instruction->name, "sra") == 0) {
                strcpy(instruction->funct3, "101");
                strcpy(instruction->funct7, strcmp(instruction->name, "sra") == 0 ? "0100000" : "0000000");
            }
            else if (strcmp(instruction->name, "or") == 0) {
                strcpy(instruction->funct3, "110");
                strcpy(instruction->funct7, "0000000");
            }
            else if (strcmp(instruction->name, "and") == 0) {
                strcpy(instruction->funct3, "111");
                strcpy(instruction->funct7, "0000000");
            }
            else if (strcmp(instruction->name, "mul") == 0) {
                strcpy(instruction->funct3, "000");
                strcpy(instruction->funct7, "0000001");
            }
            break;
        }

        case I_TYPE: {
            if (source.numTokens < 3) {
                printf("Error: Not enough tokens for I-type instruction\n");
                return;
            }

            // Tratamento especial para jalr
            if (strcmp(instruction->name, "jalr") == 0) {
                strcpy(instruction->opcode, "1100111");
                strcpy(instruction->funct3, "000");
                
                if (source.numTokens >= 2) {
                    intToBinary(extractRegistrator(source.tokens[1]), instruction->rd, 5);
                }
                if (source.numTokens >= 3) {
                    intToBinary(extractRegistrator(source.tokens[2]), instruction->rs1, 5);
                }
                if (source.numTokens >= 4) {
                    intToBinary(atoi(source.tokens[3]), instruction->imm, 12);
                }
            }
            else {
                strcpy(instruction->opcode, "0010011");
                
                intToBinary(extractRegistrator(source.tokens[1]), instruction->rd, 5);
                intToBinary(extractRegistrator(source.tokens[2]), instruction->rs1, 5);

                // Definir funct3 baseado na instrução
                if (strcmp(instruction->name, "addi") == 0) strcpy(instruction->funct3, "000");
                else if (strcmp(instruction->name, "slti") == 0) strcpy(instruction->funct3, "010");
                else if (strcmp(instruction->name, "sltiu") == 0) strcpy(instruction->funct3, "011");
                else if (strcmp(instruction->name, "xori") == 0) strcpy(instruction->funct3, "100");
                else if (strcmp(instruction->name, "ori") == 0) strcpy(instruction->funct3, "110");
                else if (strcmp(instruction->name, "andi") == 0) strcpy(instruction->funct3, "111");
                else if (strcmp(instruction->name, "slli") == 0) strcpy(instruction->funct3, "001");
                else if (strcmp(instruction->name, "srli") == 0 || strcmp(instruction->name, "srai") == 0) {
                    strcpy(instruction->funct3, "101");
                    strcpy(instruction->funct7, strcmp(instruction->name, "srai") == 0 ? "0100000" : "0000000");
                }

                // Tratar immediate value
                if (source.numTokens >= 4) {
                    int immediate = atoi(source.tokens[3]);
                    intToBinary(immediate, instruction->imm, 12);
                }
            }
            break;
        }

        case S_TYPE: {
            if (source.numTokens < 3) {
                printf("Error: Not enough tokens for S-type instruction\n");
                return;
            }

            strcpy(instruction->opcode, "0100011");
            intToBinary(extractRegistrator(source.tokens[1]), instruction->rs2, 5);

            // Extrair rs1 e offset do formato "offset(rs1)"
            char* offset_part = strtok(source.tokens[2], "(");
            char* rs1_part = strtok(NULL, ")");
            
            if (offset_part && rs1_part) {
                intToBinary(extractRegistrator(rs1_part), instruction->rs1, 5);
                intToBinary(atoi(offset_part), instruction->imm, 12);
            }

            // Definir funct3 baseado na instrução
            if (strcmp(instruction->name, "sb") == 0) strcpy(instruction->funct3, "000");
            else if (strcmp(instruction->name, "sh") == 0) strcpy(instruction->funct3, "001");
            else if (strcmp(instruction->name, "sw") == 0) strcpy(instruction->funct3, "010");
            break;
        }

        case B_TYPE: {
            if (source.numTokens < 4) {
                printf("Error: Not enough tokens for B-type instruction\n");
                return;
            }

            strcpy(instruction->opcode, "1100011");
            intToBinary(extractRegistrator(source.tokens[1]), instruction->rs1, 5);
            intToBinary(extractRegistrator(source.tokens[2]), instruction->rs2, 5);

            // Definir funct3 baseado na instrução
            if (strcmp(instruction->name, "beq") == 0) strcpy(instruction->funct3, "000");
            else if (strcmp(instruction->name, "bne") == 0) strcpy(instruction->funct3, "001");
            else if (strcmp(instruction->name, "blt") == 0) strcpy(instruction->funct3, "100");
            else if (strcmp(instruction->name, "bge") == 0) strcpy(instruction->funct3, "101");
            else if (strcmp(instruction->name, "bltu") == 0) strcpy(instruction->funct3, "110");
            else if (strcmp(instruction->name, "bgeu") == 0) strcpy(instruction->funct3, "111");

            // Converter offset para binário (13 bits)
            int offset = atoi(source.tokens[3]);
            intToBinary(offset, instruction->imm, 13);
            break;
        }

        case U_TYPE: {
            if (source.numTokens < 3) {
                printf("Error: Not enough tokens for U-type instruction\n");
                return;
            }

            if (strcmp(instruction->name, "lui") == 0) {
                strcpy(instruction->opcode, "0110111");
            }
            else if (strcmp(instruction->name, "auipc") == 0) {
                strcpy(instruction->opcode, "0010111");
            }
            else if (strcmp(instruction->name, "jal") == 0) {
                strcpy(instruction->opcode, "1101111");
            }

            intToBinary(extractRegistrator(source.tokens[1]), instruction->rd, 5);
            
            // Tratar immediate value (20 bits)
            int immediate = atoi(source.tokens[2]);
            intToBinary(immediate, instruction->imm, 20);
            break;
        }

        default:
            printf("Error: Invalid instruction format\n");
            break;
    }
}

int extractRegistrator(char* token){
    char* p = token;
    while(*p && !isdigit(*p)) p++;

    return atoi(p);
}

void intToBinary(int number, char* stringTarget, int bits){
    if (!stringTarget || bits <= 0) return;

    for (int i = 0; i < bits; i++) {
        stringTarget[i] = '0';
    }
    stringTarget[bits] = '\0';

    unsigned int value;
    if (number < 0) {
        value = (~(-number) + 1) & ((1 << bits) - 1);
    } else {
        value = number;
    }

    for (int i = bits - 1; i >= 0; i--) {
        if (value & 1) {
            stringTarget[i] = '1';
        }
        value >>= 1;
    }
}


void buildBinaryInstruction(FileLine* fileLine, Instruction* instruction){
    if (!fileLine || !instruction) return;
    
    if (fileLine->binary) {
        free(fileLine->binary);
        fileLine->binary = NULL;
    }

    char binaryStr[33] = {0}; // 32 bits + null terminator
    
    switch (instruction->format) {
        case R_TYPE:
            snprintf(binaryStr, sizeof(binaryStr), "%s%s%s%s%s%s",
                    instruction->funct7 ? instruction->funct7 : "0000000",
                    instruction->rs2 ? instruction->rs2 : "00000",
                    instruction->rs1 ? instruction->rs1 : "00000",
                    instruction->funct3 ? instruction->funct3 : "000",
                    instruction->rd ? instruction->rd : "00000",
                    instruction->opcode ? instruction->opcode : "0000000");
            break;
            
        case I_TYPE:
            snprintf(binaryStr, sizeof(binaryStr), "%s%s%s%s%s",
                    instruction->imm ? instruction->imm : "000000000000",
                    instruction->rs1 ? instruction->rs1 : "00000",
                    instruction->funct3 ? instruction->funct3 : "000",
                    instruction->rd ? instruction->rd : "00000",
                    instruction->opcode ? instruction->opcode : "0000000");
            break;
            
        case S_TYPE:
            if (instruction->imm && strlen(instruction->imm) >= 12) {
                char imm_high[8] = {0};
                char imm_low[6] = {0};
                strncpy(imm_high, instruction->imm, 7);  // bits 11-5
                strncpy(imm_low, instruction->imm + 7, 5); // bits 4-0
                
                snprintf(binaryStr, sizeof(binaryStr), "%s%s%s%s%s%s",
                        imm_high,
                        instruction->rs2 ? instruction->rs2 : "00000",
                        instruction->rs1 ? instruction->rs1 : "00000",
                        instruction->funct3 ? instruction->funct3 : "000",
                        imm_low,
                        instruction->opcode ? instruction->opcode : "0000000");
            }
            break;
            
        case B_TYPE:
            if (instruction->imm && strlen(instruction->imm) >= 13) {
                char b_imm[14] = {0};
                b_imm[0] = instruction->imm[0];  // bit 12
                strncpy(b_imm + 1, instruction->imm + 1, 6);  // bits 10-5
                strncpy(b_imm + 7, instruction->imm + 7, 4);  // bits 4-1
                b_imm[11] = instruction->imm[11]; // bit 11
                
                snprintf(binaryStr, sizeof(binaryStr), "%c%s%s%s%s%s%c%s",
                        b_imm[0],   // bit 12
                        b_imm + 1,  // bits 10-5
                        instruction->rs2 ? instruction->rs2 : "00000",
                        instruction->rs1 ? instruction->rs1 : "00000",
                        instruction->funct3 ? instruction->funct3 : "000",
                        b_imm + 7,  // bits 4-1
                        b_imm[11],  // bit 11
                        instruction->opcode ? instruction->opcode : "0000000");
            }
            break;
            
        case U_TYPE:
            snprintf(binaryStr, sizeof(binaryStr), "%s%s%s",
                    instruction->imm ? instruction->imm : "00000000000000000000",
                    instruction->rd ? instruction->rd : "00000",
                    instruction->opcode ? instruction->opcode : "0000000");
            break;
            
        default:
            return;
    }
    
    fileLine->binary = strdup(binaryStr);
}