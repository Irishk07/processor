#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "processor.h"

#include "check_errors.h"
#include "../common.h"
#include "stack.h"
#include "variable_information.h"


int TextSize(const char *file_name) {
    assert(file_name);

    struct stat text_info = {};

    if (stat(file_name, &text_info) == -1) {
        perror("Error is");

        return -1;
    }

    return (int)text_info.st_size;
}

processor_status ProcReadFile(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_file);

    FILE *text = fopen(processor->byte_code_file, "r");
    if (text == NULL) {
        CHECK_ERRORS_PROC(PROC_OPEN_ERROR,       perror("Error is"));
    }

    int text_size = TextSize(processor->byte_code_file);
    if (text_size == -1) {
        CHECK_ERRORS_PROC(PROC_STAT_ERROR);
    }

    processor->byte_code_array = (char*)calloc((size_t)(text_size + 1), sizeof(char));
    if (processor->byte_code_array == NULL) {
        CHECK_ERRORS_PROC(PROC_NOT_ENOUGH_MEMORY,        perror("Error is"));
    }

    fread((char*)processor->byte_code_array, sizeof(char), (size_t)text_size, text);
    if (ferror(text) != 0) {
        CHECK_ERRORS_PROC(PROC_READ_ERROR,     fprintf(stderr, "Error is: problem with reading file"),
                                               free(processor->byte_code_array));
    }

    if (fclose(text) == EOF) {
        CHECK_ERRORS_PROC(PROC_CLOSE_ERROR,       perror("Error is:")); 
    }

    return PROC_SUCCESS;
}

processor_status ProcCtor(Processor* processor, const char* name_byte_code_file) {
    assert(processor);
    assert(name_byte_code_file);

    processor->byte_code_file = name_byte_code_file;
    processor->programm_cnt = 0;

    CHECK_ERRORS_STACK(STACK_CREATE(processor->stack, DEFAULT_START_CAPACITY));

    *processor->registers = {};

    CHECK_ERRORS_PROC(ProcReadFile(processor),      ProcDtor(processor));

    CHECK_ERRORS_PROC(SPU(processor),       ProcDtor(processor));

    return PROC_SUCCESS;
}

processor_status ProcFillPointersArray(Processor* processor, char** pointers_data, int cnt_commands) {
    assert(processor);
    assert(processor->byte_code_file);
    assert(processor->byte_code_array);

    char* temp_text = processor->byte_code_array;

    for (int i = 0; i < cnt_commands; ) {
        if (*temp_text == '\n' || *temp_text == ' ') {
            //*temp_text == '\0';

            pointers_data[i] = temp_text + 1;

            ++i;
        }

        temp_text++;
    }

    return PROC_SUCCESS;
}

processor_status SPU(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_file);

    int cnt_commands = 0;
    sscanf(processor->byte_code_array, "%d", &cnt_commands);

    char** pointers_data = (char**)calloc((size_t)(cnt_commands + 1), sizeof(char*));
    ProcFillPointersArray(processor, pointers_data, cnt_commands);

    type_t command = 0;

    while (true) {
        sscanf(pointers_data[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &command);

        switch (command) {
            case CMD_PUSH:
                DO_CASE(DO_PUSH(processor, pointers_data));
            case CMD_POP:
                DO_CASE(DO_POP(processor));
            case CMD_ADD:
                DO_CASE(DO_ADD(processor));
            case CMD_SUB:       
                DO_CASE(DO_SUB(processor));
            case CMD_DIV:
                DO_CASE(DO_DIV(processor));
            case CMD_MUL:
                DO_CASE(DO_MUL(processor));
            case CMD_SQRT:
                DO_CASE(DO_SQRT(processor));
            case CMD_POW:
                DO_CASE(DO_POW(processor));
            case CMD_IN:
                DO_CASE(DO_IN(processor));
            case CMD_POPR:
                DO_CASE(DO_POPR(processor, pointers_data));
            case CMD_PUSHR:
                DO_CASE(DO_PUSHR(processor, pointers_data));
            case CMD_JMP:
                DO_CASE(DO_JMP(processor, pointers_data));
            case CMD_OUT:
                DO_CASE(DO_OUT(processor));
            case CMD_HLT:
                break;
            default:
                return PROC_UNKNOWN_COMAND;
        }

        if (command == CMD_HLT) {
            break;
        }

        processor->programm_cnt++;
    }

    free(pointers_data);

    return PROC_SUCCESS;
}

type_error_t ProcVerify(Processor* processor) {
    if (processor == NULL) {
        return NULL_POINTER_ON_STRUCT_PROC;
    }

    type_error_t code_error = PROC_SUCCESS;

    if (processor->byte_code_file == NULL)               code_error |= NULL_POINTER_ON_FILE;
    if (processor->byte_code_array == NULL)              code_error |= NULL_POINTER_ON_BYTE_CODE;
    if (StackVerify(&processor->stack) != STACK_SUCCESS) code_error |= STACK_ERROR; // FIXME

    return code_error;
}

void ProcDump(Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name) {
    fprintf(stderr, "Called at %s() %s:%d:\n", function_name, file_name, line);

    fprintf(stderr, "    registers[%d]:\n", CNT_REGISTERS);

    fprintf(stderr, "      [%d] = SERVICE\n", 0);
    for (int i = 1; i < CNT_REGISTERS; ++i) {
        fprintf(stderr, "      [%d] = " TYPE_T_PRINTF_SPECIFIER " (R%cX)\n", i, processor->registers[i], i + 'A' - 1);
    }

    fprintf(stderr, "\n");
    StackDump(&processor->stack, code_error, DUMP_VAR_INFO);
}

processor_status ProcDtor(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_file);

    StackDtor(&processor->stack);

    free(processor->byte_code_array);

    *processor = {};

    return PROC_SUCCESS;
}


processor_status DO_PUSH(Processor* processor, char** pointers_data) {
    type_t num = 0;
    processor->programm_cnt++;

    sscanf(pointers_data[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num);
    CHECK_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status DO_POP(Processor* processor) {
    type_t deleted_value = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &deleted_value));

    return PROC_SUCCESS;
}

processor_status DO_ADD(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num + first_num));

    return PROC_SUCCESS;
}

processor_status DO_SUB(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num - first_num));

    return PROC_SUCCESS;
}

processor_status DO_DIV(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));

    if (first_num != 0) {
        CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num / first_num));
    }
    else {
        return PROC_DIVISION_BY_ZERO;
    }

    return PROC_SUCCESS;
}

processor_status DO_MUL(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num * first_num));

    return PROC_SUCCESS;
}

processor_status DO_SQRT(Processor* processor) {
    type_t num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &num));

    if (num >= 0) {
        CHECK_ERRORS_STACK(StackPush(&processor->stack, (type_t)sqrt(num)));
    }
    else {
        return PROC_SQRT_NEGATIVE_NUM;
    }

    return PROC_SUCCESS;
}

processor_status DO_POW(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_ERRORS_STACK(StackPush(&processor->stack, (type_t)pow(second_num, first_num)));

    return PROC_SUCCESS;
}

processor_status DO_IN(Processor* processor) {
    type_t num  = 0;

    scanf(TYPE_T_PRINTF_SPECIFIER, &num);
    CHECK_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status DO_POPR(Processor* processor, char** pointers_data) {
    type_t num = 0;
    type_t code_reg  = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &num));
    processor->programm_cnt++;
    sscanf(pointers_data[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg);
    processor->registers[code_reg] = num;

    return PROC_SUCCESS;
}

processor_status DO_PUSHR(Processor* processor, char** pointers_data) {
    type_t code_reg  = 0;

    processor->programm_cnt++;
    sscanf(pointers_data[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg);
    CHECK_ERRORS_STACK(StackPush(&processor->stack, processor->registers[code_reg]));

    return PROC_SUCCESS;
}

processor_status DO_OUT(Processor* processor) {
    type_t num = 0;

    CHECK_ERRORS_STACK(StackPop(&processor->stack, &num));
    fprintf(stderr, "Answer " TYPE_T_PRINTF_SPECIFIER "\n", num);

    return PROC_SUCCESS;
}

processor_status DO_JMP(Processor* processor, char** pointers_data) {
    type_t num = 0;
    processor->programm_cnt++;

    sscanf(pointers_data[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num);
    processor->programm_cnt = (size_t)(num - 1);

    getchar();

    return PROC_SUCCESS;
}

/*

why break?

while (bytes != 0) {
    switch(bytes % 8) {
        case 0:
            memcpy(8); bytes -= 8;
            break;
        case 7:
        case 6:
        case 5:
        case 4:
            memcpy(4); bytes -= 4;
            break;
        case 3:
        case 2:
            memcpy(2); bytes -= 2;
            break;
        case 1:
            memcpy(1); bytes -= 1;
            break;
        default:
            assert(0);
    }
}

*/