#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "asm.h"

#include "../common.h"
#include "onegin.h"


void AsmInitLabels(Assembler* assembler) {
    for (int i = 0; i < CNT_LABELS; ++i) {
        assembler->labels[i] = -1;
    }
}

assembler_status AsmCtor(Assembler* assembler, const char* name_commands_file) {
    assert(assembler);
    assert(name_commands_file);

    assembler->about_text.text_name = name_commands_file;
    assembler->byte_code_data.size  = 0;

    AsmInitLabels(assembler);

    CHECK_AND_RETURN_ERRORS_ASM(OneginReadFile(assembler),      free(assembler->byte_code_data.data));

    CHECK_AND_RETURN_ERRORS_ASM(DivisionIntoCommands(assembler));

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, FIRST_COMPILE));

    return ASM_SUCCESS;
}


assembler_status AsmVerify(const Assembler* assembler, int number_of_compile) {
    if (assembler == NULL) {
        return ASM_NULL_POINTER_ON_STRUCT; 
    }

    if (number_of_compile == SECOND_COMPILE && assembler->byte_code_data.data == NULL) {
        return ASM_NULL_POINTER_ON_DATA;
    }

    if (assembler->about_text.text_name == NULL) {
        return ASM_NULL_POINTER_ON_NAME_OF_FILE;
    }

    return ASM_SUCCESS;
}

status_cmp FindCommand(Assembler* assembler, char* string, About_commands* current_command) {
    assert(assembler);
    assert(string);
    assert(current_command);

    for(int i = 0; i < MAX_CNT_COMMANDS; ++i) {
        if(!strcmp(about_commands[i].command_name, (const char*)string)) {
            *current_command = about_commands[i];
            return EQUAL;
        }
    }

    return DIFFERENT;
}

void FillCommand(Assembler* assembler, About_commands* current_command, int number_of_compile) {
    assert(assembler);
    assert(current_command);

    if (number_of_compile == SECOND_COMPILE) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = current_command->command_code;
    }

    assembler->byte_code_data.size++;
}

assembler_status GetFillArgNum(Assembler* assembler, About_commands* current_command, char* string, int number_of_compile) {
    assert(string);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    type_t number = 0;

    if (sscanf(string, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
        if (number_of_compile == SECOND_COMPILE) {
            assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
            current_command->argument = number;
        }

        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_NUMBER;
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    return ASM_SUCCESS;
}

status_cmp CheckRegister(Assembler* assembler, char* string, int type_argument) {
    assert(string);

    for (int i = 0; i < CNT_REGISTERS; ++i) {
        if (!strncmp(assembler->name_registers[i], string, LEN_NAME_REGISTER)) {
            if (type_argument == REG_ARGUMENT && strlen(string) == LEN_NAME_REGISTER)
                return EQUAL;

            if (type_argument == RAM_REG_ARGUMENT && strlen(string) == LEN_NAME_REGISTER + 1 //+1 because nam has ']' at the end
                                                  && string[LEN_NAME_REGISTER] == ']') 
                return EQUAL;
        }
    }

    return DIFFERENT;
}

assembler_status GetFillArgReg(Assembler* assembler, About_commands* current_command, char* string, int number_of_compile, int type_argument) {
    assert(string);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    if (string[0] == '[') string++;

    if (!CheckRegister(assembler, string, type_argument)) {
        return ASM_EXPECTS_REGISTER;
    }

    if (number_of_compile == SECOND_COMPILE) {
        type_t code_reg = string[1] - 'A';
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_reg;
        current_command->argument = code_reg;
    }

    assembler->byte_code_data.size++;

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    return ASM_SUCCESS;
}

assembler_status GetFillArgJump(Assembler* assembler, About_commands* current_command, char* string, int number_of_compile) {
    assert(string);

    if (GetFillArgNum(assembler, current_command, string, number_of_compile) == ASM_SUCCESS) {
        return ASM_SUCCESS;
    }

    if (*string != ':') {   
        return ASM_EXPECTS_JUMP_ARG;
    }

    int number = 0;                                                                                                       
    if (sscanf(string, "%*c%d", &number) != 1) {
        return ASM_EXPECTS_JUMP_ARG;
    }

    if (0 <= number && number <= 9) {
        if (number_of_compile == SECOND_COMPILE) {
            if (assembler->labels[number] == -1) return ASM_NOT_FOUND_LABEL;

            assembler->byte_code_data.data[assembler->byte_code_data.size] = assembler->labels[number];
            current_command->argument = assembler->labels[number];
        }

        assembler->byte_code_data.size++;

        return ASM_SUCCESS;
    }

    return ASM_EXPECTS_JUMP_ARG;
}

assembler_status CheckLabel(Assembler* assembler, char* string) {
    assert(string);

    if (*string == ':') {                                                       
        int number = 0;                                                         
                                                                                
        if (sscanf(string, "%*c%d", &number) != 1) {                            
            return ASM_NOT_FOUND_LABEL;                                          
        }       
        
        assembler->labels[number] = (type_t)assembler->byte_code_data.size;     
        
        return ASM_SUCCESS;
    }

   return ASM_NOT_FOUND_LABEL;
}

void FillListingFile(char* pointer_on_command, About_commands* current_command, FILE* listing_file, int type_argument) {
    assert(pointer_on_command);
    assert(current_command);
    assert(listing_file);

    fprintf(listing_file, "%p ", pointer_on_command);
    fprintf(listing_file, "%-8s ", current_command->command_name);
    fprintf(listing_file, "%-5ld ", current_command->command_code);

    if (type_argument != NO_ARGUMENT) {
        fprintf(listing_file, TYPE_T_PRINTF_SPECIFIER, current_command->argument);
    }

    fprintf(listing_file, "\n");
}

assembler_status PrintfByteCode(Assembler* assembler, int number_of_compile) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    fprintf(stderr, "Compile number %d:\n", number_of_compile);

    if (number_of_compile == SECOND_COMPILE) {
        fprintf(stderr, "Byte code:\n");

        for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
            fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "Labels:\n");
    for (int i = 0; i < CNT_LABELS; ++i) {
        fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->labels[i]);
    }
    fprintf(stderr, "\n\n");

    return ASM_SUCCESS;

    // getchar(); // TODO print message
}

#define OPEN_LISTING_FILE(file)                                  \
        if (number_of_compile == SECOND_COMPILE) {               \
            file = fopen("listing_file.txt", "w");               \
                                                                 \
            if (file == NULL) {                                  \
                CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR);     \
            }                                                    \
        }

#define CLOSE_LISTING_FILE(file)                                                     \
        if (number_of_compile == SECOND_COMPILE) {                                    \
            if (fclose(file) == EOF) {                                        \
                CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,    perror("Error is:")); \
            }                                                                         \
        }

assembler_status Assemblirovanie(Assembler* assembler, int number_of_compile) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    FILE* listing_file = NULL;
    OPEN_LISTING_FILE(listing_file);

    bool find_cmd_hlt = false;

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

        if (strlen(assembler->about_text.pointer_on_text[i]) == 0) {
            continue;
        }

        if (CheckLabel(assembler, assembler->about_text.pointer_on_text[i]) == ASM_SUCCESS) continue;

        About_commands current_command = {};
        if (!FindCommand(assembler, assembler->about_text.pointer_on_text[i], &current_command)) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_UNKNOWN_COMAND);
        }
        
        FillCommand(assembler, &current_command, number_of_compile);

        if (current_command.command_code == CMD_HLT) find_cmd_hlt = true;

        if (current_command.code_of_type_argument == NUM_ARGUMENT)   
            CHECK_AND_RETURN_ERRORS_ASM(GetFillArgNum(assembler, &current_command, assembler->about_text.pointer_on_text[++i], number_of_compile));
            
        if (current_command.code_of_type_argument == REG_ARGUMENT ||
            current_command.code_of_type_argument == RAM_REG_ARGUMENT)   
            CHECK_AND_RETURN_ERRORS_ASM(GetFillArgReg(assembler, &current_command, assembler->about_text.pointer_on_text[++i], 
                                                        number_of_compile, current_command.code_of_type_argument));

        if (current_command.code_of_type_argument == LABEL_ARGUMENT)
            CHECK_AND_RETURN_ERRORS_ASM(GetFillArgJump(assembler, &current_command, assembler->about_text.pointer_on_text[++i], number_of_compile));

        if (number_of_compile == SECOND_COMPILE) {
            FillListingFile(assembler->about_text.pointer_on_text[i], &current_command, listing_file, current_command.code_of_type_argument);
        }

        CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));
    }

    if (!find_cmd_hlt) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_EXPECTS_HLT);
    }
    
    PrintfByteCode(assembler, number_of_compile);

    CLOSE_LISTING_FILE(listing_file);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    return ASM_SUCCESS;
}

assembler_status CreatByteCodeData(Assembler* assembler) {
    assert(assembler);

    assembler->byte_code_data.data = (type_t*)calloc(assembler->byte_code_data.size, sizeof(type_t));

    if (assembler->byte_code_data.data == NULL){
        CHECK_AND_RETURN_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY);
    }

    assembler->byte_code_data.size = 0;

    return ASM_SUCCESS;
}

assembler_status CreateExeFile(const Assembler* assembler, const char* name_byte_code_file) {
    assert(name_byte_code_file);
    assert(assembler->about_text.pointer_on_text);
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, SECOND_COMPILE));

    FILE* text = fopen(name_byte_code_file, "w");
    if (text == NULL) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR,       perror("Error is"));
    }

    fprintf(text, "%zu\n", assembler->byte_code_data.size);

    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(text, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }

    if (fclose(text) == EOF) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,      perror("Error is"));
    }

    return ASM_SUCCESS;
}

assembler_status AsmDtor(Assembler* assembler) {
    assembler_status code_error = AsmVerify(assembler, SECOND_COMPILE);

    free(assembler->byte_code_data.data);
    free(assembler->about_text.text);
    free(assembler->about_text.pointer_on_text);

    return code_error;
}