#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "asm.h"

#include "../common.h"
#include "onegin.h"

// FIXME объединить с процессорным...как
About_commands about_commands [] = {
    {.command_name = "PUSH", .hash = hash_djb2((const char*)"PUSH"),  .command_code = CMD_PUSH,  .code_of_type_argument = NUM_ARGUMENT,     .argument = 0},
    {.command_name = "POP",  .hash = hash_djb2((const char*)"POP"),   .command_code = CMD_POP,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "ADD",  .hash = hash_djb2((const char*)"ADD"),   .command_code = CMD_ADD,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "SUB",  .hash = hash_djb2((const char*)"SUB"),   .command_code = CMD_SUB,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "DIV",  .hash = hash_djb2((const char*)"DIV"),   .command_code = CMD_DIV,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "MUL",  .hash = hash_djb2((const char*)"MUL"),   .command_code = CMD_MUL,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "SQRT", .hash = hash_djb2((const char*)"SQRT"),  .command_code = CMD_SQRT,  .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "POW",  .hash = hash_djb2((const char*)"POW"),   .command_code = CMD_POW,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "IN",   .hash = hash_djb2((const char*)"IN"),    .command_code = CMD_IN,    .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "OUT",  .hash = hash_djb2((const char*)"OUT"),   .command_code = CMD_OUT,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "HLT",  .hash = hash_djb2((const char*)"HLT"),   .command_code = CMD_HLT,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "JMP",  .hash = hash_djb2((const char*)"JMP"),   .command_code = CMD_JMP,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JB",   .hash = hash_djb2((const char*)"JB"),    .command_code = CMD_JB,    .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JBE",  .hash = hash_djb2((const char*)"JBE"),   .command_code = CMD_JBE,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JA",   .hash = hash_djb2((const char*)"JA"),    .command_code = CMD_JA,    .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JAE",  .hash = hash_djb2((const char*)"JAE"),   .command_code = CMD_JAE,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JE",   .hash = hash_djb2((const char*)"JE"),    .command_code = CMD_JE,    .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JNE",  .hash = hash_djb2((const char*)"JNE"),   .command_code = CMD_JNE,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "CALL", .hash = hash_djb2((const char*)"CALL"),  .command_code = CMD_CALL,  .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "RET",  .hash = hash_djb2((const char*)"RET"),   .command_code = CMD_RET,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "PUSHR",.hash = hash_djb2((const char*)"PUSHR"), .command_code = CMD_PUSHR, .code_of_type_argument = REG_ARGUMENT,     .argument = 0},
    {.command_name = "POPR", .hash = hash_djb2((const char*)"POPR"),  .command_code = CMD_POPR,  .code_of_type_argument = REG_ARGUMENT,     .argument = 0},
    {.command_name = "PUSHM",.hash = hash_djb2((const char*)"PUSHM"), .command_code = CMD_PUSHM, .code_of_type_argument = RAM_REG_ARGUMENT, .argument = 0},
    {.command_name = "POPM", .hash = hash_djb2((const char*)"POPM"),  .command_code = CMD_POPM,  .code_of_type_argument = RAM_REG_ARGUMENT, .argument = 0}
};

About_register about_register [] {
    {.register_name = "RAX",   .hash = hash_djb2((const char*)"RAX")},
    {.register_name = "[RAX]", .hash = hash_djb2((const char*)"[RAX]")},
    {.register_name = "RBX",   .hash = hash_djb2((const char*)"RBX")},
    {.register_name = "[RBX]", .hash = hash_djb2((const char*)"[RBX]")},
    {.register_name = "RCX",   .hash = hash_djb2((const char*)"RCX")},
    {.register_name = "[RCX]", .hash = hash_djb2((const char*)"[RCX]")},
    {.register_name = "RDX",   .hash = hash_djb2((const char*)"RDX")},
    {.register_name = "[RDX]", .hash = hash_djb2((const char*)"[RDX]")},
    {.register_name = "REX",   .hash = hash_djb2((const char*)"REX")},
    {.register_name = "[REX]", .hash = hash_djb2((const char*)"[REX]")},
    {.register_name = "RFX",   .hash = hash_djb2((const char*)"RFX")},
    {.register_name = "[RFX]", .hash = hash_djb2((const char*)"[RFX]")},
    {.register_name = "RGX",   .hash = hash_djb2((const char*)"RGX")},
    {.register_name = "[RGX]", .hash = hash_djb2((const char*)"[RGX]")},
    {.register_name = "RHX",   .hash = hash_djb2((const char*)"RHX")},
    {.register_name = "[RHX]", .hash = hash_djb2((const char*)"[RHX]")}
};


assembler_status AsmCtor(Assembler* assembler, const char* name_commands_file) {
    assert(assembler);
    assert(name_commands_file);

    assembler->about_text.text_name = name_commands_file;
    assembler->byte_code_data.size  = 0;

    AsmInitLabels(assembler);

    CHECK_AND_RETURN_ERRORS_ASM(OneginReadFile(assembler),      free(assembler->byte_code_data.data));

    CHECK_AND_RETURN_ERRORS_ASM(DivisionIntoCommands(assembler));

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    qsort(about_commands, sizeof(about_commands) / sizeof(about_commands[0]), sizeof(About_commands), &qsort_commands_comparator);
    qsort(about_register, sizeof(about_register) / sizeof(about_register[0]), sizeof(About_register), &qsort_register_comparator);

    return ASM_SUCCESS;
}

void AsmInitLabels(Assembler* assembler) {
    for (int i = 0; i < CNT_LABELS; ++i) {
        assembler->about_labels[i].hash  =  0;
        assembler->about_labels[i].index = -1;
    }
}

number_of_compile FirstOrSecondCompile(const Assembler* assembler) {
    if (assembler->byte_code_data.data == NULL) 
        return FIRST_COMPILE;

    return SECOND_COMPILE;
}

assembler_status AsmVerify(const Assembler* assembler) {
    if (assembler == NULL) {
        return ASM_NULL_POINTER_ON_STRUCT;
    }

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE && assembler->byte_code_data.data == NULL) {
        return ASM_NULL_POINTER_ON_DATA;
    }

    if (assembler->about_text.text_name == NULL) {
        return ASM_NULL_POINTER_ON_NAME_OF_FILE;
    }

    return ASM_SUCCESS;
}

unsigned long hash_djb2(const char *str) {
    assert(str);

    unsigned long hash = 5381;
    int c = 0;

    while ((c = *(str++)) != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    }

    return hash;
}

#define DO_QSORT_COMPARATOR(name, type)                                          \
                                                                                 \
int qsort_ ## name ## _comparator(const void* param1, const void* param2) {      \
    const struct type* struct1 = (const struct type*)param1;                     \
    const struct type* struct2 = (const struct type*)param2;                     \
                                                                                 \
    if (struct1->hash < struct2->hash) return -1;                                \
                                                                                 \
    if (struct1->hash > struct2->hash) return 1;                                 \
                                                                                 \
    return 0;                                                                    \
}

DO_QSORT_COMPARATOR(commands, About_commands)
DO_QSORT_COMPARATOR(register, About_register)
DO_QSORT_COMPARATOR(label,    About_labels)

#undef DO_QSORT_COMPARATOR

#define DO_BSEARCH_COMPARATOR(name, type)                                      \
                                                                               \
int bsearch_ ## name ## _comparator(const void* param1, const void* param2) {  \
    unsigned long search_hash = *(const unsigned long*)param1;                 \
    const struct type* struct1 = (const struct type*)param2;                   \
                                                                               \
    if (search_hash < struct1->hash) return -1;                                \
                                                                               \
    if (search_hash > struct1->hash) return 1;                                 \
                                                                               \
    return 0;                                                                  \
}

DO_BSEARCH_COMPARATOR(commands, About_commands)
DO_BSEARCH_COMPARATOR(register, About_register)

#undef DO_BSEARCH_COMPARATOR


assembler_status Assemblirovanie(Assembler* assembler) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    assembler->cnt_current_label = 0;

    FILE* listing_file = NULL;
    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        listing_file = fopen("listing_file.txt", "w");

        if (listing_file == NULL) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR);
        }
    }

    bool find_cmd_hlt = false;

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

        if (strlen(assembler->about_text.pointer_on_text[i]) == 0) {
            continue;
        }

        if (assembler->about_text.pointer_on_text[i][0] == ':') {
            if (FirstOrSecondCompile(assembler) == FIRST_COMPILE)
                InsertLabel(assembler, assembler->about_text.pointer_on_text[i]);
            continue;
        }

        About_commands current_command = {};
        if (!FindCommand(assembler, assembler->about_text.pointer_on_text[i], &current_command)) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_UNKNOWN_COMAND);
        }

        FillCommand(assembler, &current_command);

        if (current_command.command_code == CMD_HLT) find_cmd_hlt = true;

        if (current_command.code_of_type_argument != NO_ARGUMENT) {
            CHECK_AND_RETURN_ERRORS_ASM(PassArgs(assembler, &current_command, assembler->about_text.pointer_on_text[++i]));
        }

        if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
            FillListingFile(assembler->about_text.pointer_on_text[i], &current_command, listing_file, current_command.code_of_type_argument);
        }

        CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));
    }

    if (!find_cmd_hlt) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_EXPECTS_HLT);
    }

    PrintfByteCode(assembler);

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        if (fclose(listing_file) == EOF) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,    perror("Error is:"));
        }
    }

    if (FirstOrSecondCompile(assembler) == FIRST_COMPILE) {
        assembler->cnt_current_label = 0;

        CHECK_AND_RETURN_ERRORS_ASM(CheckDoubleLabels(assembler));
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

void InsertLabel(Assembler* assembler, char* string) {
    assert(string);

    assembler->about_labels[assembler->cnt_current_label].hash  = hash_djb2((char*)++string); // ++ because skip :
    assembler->about_labels[assembler->cnt_current_label].index = (type_t)assembler->byte_code_data.size;

    assembler->cnt_current_label++;
}

status_cmp FindCommand(Assembler* assembler, char* string, About_commands* current_command) {
    assert(assembler);
    assert(string);
    assert(current_command);

    unsigned long current_hash = hash_djb2((char*)string);

    About_commands* res = (About_commands*)bsearch(&current_hash, about_commands, sizeof(about_commands) / sizeof(about_commands[0]), 
                                                   sizeof(About_commands), &bsearch_commands_comparator);

    if (res == NULL) 
        return DIFFERENT;

    long int index = res - about_commands;

    if (strcmp(about_commands[index].command_name, string) == 0) {
        *current_command = about_commands[index];
        return EQUAL;
    }

    return DIFFERENT;
}

void FillCommand(Assembler* assembler, About_commands* current_command) {
    assert(assembler);
    assert(current_command);

    if (assembler->byte_code_data.data != NULL) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = current_command->command_code;
    }

    assembler->byte_code_data.size++;
}

assembler_status PassArgs(Assembler* assembler, About_commands* current_command, char* string) {
    if (current_command->code_of_type_argument == NUM_ARGUMENT) {
        if (GetFillArgNum(assembler, current_command, string) == ASM_SUCCESS)
            return ASM_SUCCESS;
    }

    if (current_command->code_of_type_argument == REG_ARGUMENT ||
        current_command->code_of_type_argument == RAM_REG_ARGUMENT) {
        if (GetFillArgReg(assembler, current_command, string, current_command->code_of_type_argument) == ASM_SUCCESS)
            return ASM_SUCCESS;
    }

    if (current_command->code_of_type_argument == LABEL_ARGUMENT) {
        if (GetFillArgJump(assembler, current_command, string) == ASM_SUCCESS)
            return ASM_SUCCESS;
    }

    return ASM_EXPECTS_ARG;
}

assembler_status GetFillArgNum(Assembler* assembler, About_commands* current_command, char* string) {
    assert(string);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    type_t number = 0;

    if (sscanf(string, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
        if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
            assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
            current_command->argument = number;
        }

        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_NUMBER;
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

assembler_status GetFillArgReg(Assembler* assembler, About_commands* current_command, char* string, int type_argument) {
    assert(string);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    if (!CheckRegister(string, type_argument)) {
        return ASM_EXPECTS_REGISTER;
    }

    if (string[0] == '[') 
        string++;

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        type_t code_reg = string[1] - 'A';
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_reg;
        current_command->argument = code_reg;
    }

    assembler->byte_code_data.size++;

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

status_cmp CheckRegister(char* string, int type_argument) {
    assert(string);

    unsigned long current_hash = hash_djb2((char*)string);

    About_register* res = (About_register*)bsearch(&current_hash, about_register, sizeof(about_register) / sizeof(about_register[0]), 
                                                     sizeof(About_register), &bsearch_register_comparator);

    if (res == NULL) 
        return DIFFERENT;

    long int index = res - about_register;

    if (strncmp(about_register[index].register_name, string, LEN_NAME_REGISTER) == 0) {
        if (type_argument == REG_ARGUMENT && strlen(string) == LEN_NAME_REGISTER)
            return EQUAL;

        if (type_argument == RAM_REG_ARGUMENT && strlen(string) == LEN_NAME_REGISTER + 2 // +2 because name has '[' at the begin and ']' at the end
                                                && string[0] == '['
                                                && string[LEN_NAME_REGISTER + 1] == ']')
            return EQUAL;
    }

    return DIFFERENT;
}

assembler_status GetFillArgJump(Assembler* assembler, About_commands* current_command, char* string) {
    assert(string);

    if (GetFillArgNum(assembler, current_command, string) == ASM_SUCCESS) {
        return ASM_SUCCESS;
    }

    if (*string != ':') {
        return ASM_EXPECTS_JUMP_ARG;
    }

    if (FirstOrSecondCompile(assembler) == FIRST_COMPILE) {
        assembler->byte_code_data.size++;

        return ASM_SUCCESS;
    }

    unsigned long current_hash = hash_djb2((char*)++string); // ++ because skip :

    for (size_t i = 0; i < sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]); ++i) {
        if (current_hash != assembler->about_labels[i].hash)
            continue;

        if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
            if (assembler->about_labels[i].index == -1)
                return ASM_NOT_FOUND_LABEL;

            assembler->byte_code_data.data[assembler->byte_code_data.size] = assembler->about_labels[i].index;
            current_command->argument = assembler->about_labels[i].index;
        }

        assembler->byte_code_data.size++;

        return ASM_SUCCESS;

    }

    return ASM_EXPECTS_JUMP_ARG;
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

assembler_status PrintfByteCode(Assembler* assembler) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    fprintf(stderr, "Compile number %d:\n", FirstOrSecondCompile(assembler));

    if (FirstOrSecondCompile(assembler) == SECOND_COMPILE) {
        fprintf(stderr, "Byte code:\n");

        for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
            fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "Labels:\n");
    for (size_t i = 0; i < sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]); ++i) {
        fprintf(stderr, "hash %lu, index " TYPE_T_PRINTF_SPECIFIER "\n", assembler->about_labels[i].hash, assembler->about_labels[i].index);
    }
    fprintf(stderr, "\n\n");

    return ASM_SUCCESS;
}

assembler_status CheckDoubleLabels(Assembler* assembler) {
    qsort(assembler->about_labels, sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]), 
          sizeof(About_labels), &qsort_label_comparator);

    for (size_t i = 0; i < sizeof(assembler->about_labels) / sizeof(assembler->about_labels[0]) - 1; ++i) {
        if (assembler->about_labels[i].hash == assembler->about_labels[i + 1].hash &&
            assembler->about_labels[i].hash != 0) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_DOUBLE_LABEL);
        }
    }

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
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

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
    assembler_status code_error = AsmVerify(assembler);

    free(assembler->byte_code_data.data);
    free(assembler->about_text.text);
    free(assembler->about_text.pointer_on_text);

    return code_error;
}
