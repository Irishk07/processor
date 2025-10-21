#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "processor.h"

#include "check_errors.h"
#include "../common.h"
#include "onegin.h"
#include "stack.h"
#include "variable_information.h"

void InitAboutCommands(About_commands* about_commands) {
    assert(about_commands);

    about_commands[CMD_PUSH]  = {.command_name = "PUSH",  &do_push};
    about_commands[CMD_POP]   = {.command_name = "POP",   &do_pop};
    about_commands[CMD_ADD]   = {.command_name = "ADD",   &do_add};
    about_commands[CMD_SUB]   = {.command_name = "SUB",   &do_sub};
    about_commands[CMD_DIV]   = {.command_name = "DIV",   &do_div};    
    about_commands[CMD_MUL]   = {.command_name = "MUL",   &do_mul};
    about_commands[CMD_SQRT]  = {.command_name = "SQRT",  &do_sqrt};
    about_commands[CMD_POW]   = {.command_name = "POW",   &do_pow};
    about_commands[CMD_IN]    = {.command_name = "IN",    &do_in};
    about_commands[CMD_OUT]   = {.command_name = "OUT",   &do_out};
    about_commands[CMD_JMP]   = {.command_name = "JMP",   &do_jmp};
    about_commands[CMD_JB]    = {.command_name = "JB",    &do_jb};
    about_commands[CMD_JBE]   = {.command_name = "JBE",   &do_jbe};
    about_commands[CMD_JA]    = {.command_name = "JA",    &do_ja};
    about_commands[CMD_JAE]   = {.command_name = "JAE",   &do_jae};
    about_commands[CMD_JE]    = {.command_name = "JE",    &do_je};
    about_commands[CMD_JNE]   = {.command_name = "JNE",   &do_jne};
    about_commands[CMD_CALL]  = {.command_name = "CALL",  &do_call};
    about_commands[CMD_RET]   = {.command_name = "RET",   &do_ret};
    about_commands[CMD_PUSHR] = {.command_name = "PUSHR", &do_pushr};
    about_commands[CMD_POPR]  = {.command_name = "POPR",  &do_popr};
    about_commands[CMD_PUSHM] = {.command_name = "PUSHM", &do_pushm};
    about_commands[CMD_POPM]  = {.command_name = "POPM",  &do_popm};
}

processor_status ProcCtor(Processor* processor, const char* name_byte_code_file) {
    assert(processor);
    assert(name_byte_code_file);

    processor->about_text.text_name = name_byte_code_file;
    processor->programm_cnt = 0;
    processor->cnt_commands = 0;

    CHECK_AND_RETURN_ERRORS_STACK(STACK_CREATE(processor->stack, DEFAULT_START_CAPACITY));
    CHECK_AND_RETURN_ERRORS_STACK(STACK_CREATE(processor->return_stack, DEFAULT_START_CAPACITY));

    memset(processor->registers, 0, CNT_REGISTERS * sizeof(processor->registers[0]));
    memset(processor->ram, 0, SIZE_RAM * sizeof(processor->ram[0]));

    CHECK_AND_RETURN_ERRORS_PROC(OneginReadFile(processor));

    CHECK_AND_RETURN_ERRORS_PROC(DivisionIntoCommands(processor));

    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    return PROC_SUCCESS;
}

processor_status ProcVerify(const Processor* processor) {
    if (processor == NULL) {
        return PROC_NULL_POINTER_ON_STRUCT;
    }

    if (processor->about_text.pointer_on_text == NULL)   return PROC_NULL_POINTER_ON_DATA;

    if (processor->about_text.text_name == NULL)         return PROC_NULL_POINTER_ON_FILE;

    if (StackVerify(&processor->stack) != STACK_SUCCESS) return STACK_ERROR;

    return PROC_SUCCESS;
}

processor_status SPU(Processor* processor, About_commands* about_commands) {
    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    type_t command = 0;

    while (processor->programm_cnt != processor->cnt_commands) {
        if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &command) != 1) {
            CHECK_AND_RETURN_ERRORS_PROC(PROC_WRONG_BYTE_CODE);
        }

        if (command == CMD_HLT) {
            break;
        }

        
        about_commands[command].function(processor);

        processor->programm_cnt++;
    }

    if (processor->programm_cnt == processor->cnt_commands) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_EXPECTS_HLT);
    }

    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    return PROC_SUCCESS;
}

void ProcDump(const Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name) {
    fprintf(stderr, "Called at %s() %s:%d:\n", function_name, file_name, line);

    fprintf(stderr, "Count commands is: %zu\n", processor->cnt_commands);
    fprintf(stderr, "Now command is: %zu\n", processor->programm_cnt + 1);

    fprintf(stderr, "    registers[%d]:\n", CNT_REGISTERS);
    for (int i = 0; i < CNT_REGISTERS; ++i) {
        fprintf(stderr, "      [%d] = " TYPE_T_PRINTF_SPECIFIER " (R%cX)\n", i, processor->registers[i], i + 'A');
    }

    fprintf(stderr, "    ram[%d]:\n", SIZE_RAM);
    for (int i = 0; i < SIZE_RAM; ++i) {
        fprintf(stderr, "      [%d] = " "%d\n", i, processor->ram[i]);
    }

    fprintf(stderr, "\n\n");
    fprintf(stderr, "About stack:\n");
    StackDump(&processor->stack, code_error, DUMP_VAR_INFO);
}

processor_status ProcDtor(Processor* processor) {
    assert(processor);

    processor_status code_error = ProcVerify(processor);

    StackDtor(&processor->stack);
    StackDtor(&processor->return_stack);
    
    free(processor->about_text.text);
    free(processor->about_text.pointer_on_text);

    *processor = {};

    return code_error;
}


processor_status do_push(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t num = 0;
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num) != 1) {
        return PROC_EXPECTS_ARG;
    }
        
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status do_pop(Processor* processor) {
    assert(processor);
    
    type_t deleted_value = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &deleted_value));

    return PROC_SUCCESS;
}

processor_status do_add(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num + first_num));

    return PROC_SUCCESS;
}

processor_status do_sub(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num - first_num));

    return PROC_SUCCESS;
}

processor_status do_div(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));

    if (first_num != 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num / first_num));
    }
    else {
        return PROC_DIVISION_BY_ZERO;
    }

    return PROC_SUCCESS;
}

processor_status do_mul(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num * first_num));

    return PROC_SUCCESS;
}

processor_status do_sqrt(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    if (num >= 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)sqrt(num)));
    }
    else {
        return PROC_SQRT_NEGATIVE_NUM;
    }

    return PROC_SUCCESS;
}

processor_status do_pow(Processor* processor) {
    assert(processor);

    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)pow(second_num, first_num)));

    return PROC_SUCCESS;
}

processor_status do_in(Processor* processor) {
    assert(processor);
    
    type_t num  = 0;

    printf("Enter tne number please:\n");

    if (scanf(TYPE_T_PRINTF_SPECIFIER, &num) != 1) {
        return PROC_EXPECTS_ARG;
    }

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status do_popr(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t num = 0;
    type_t code_reg  = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg) != 1) {
        return PROC_EXPECTS_ARG;
    }

    if (code_reg < 0 || code_reg >= CNT_REGISTERS) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_INVALID_REGISTER);
    }

    processor->registers[code_reg] = num;

    return PROC_SUCCESS;
}

processor_status do_pushr(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t code_reg  = 0;
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg) != 1) {
        return PROC_EXPECTS_ARG;
    }

    if (code_reg < 0 || code_reg >= CNT_REGISTERS) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_INVALID_REGISTER);
    }
    
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, processor->registers[code_reg]));

    return PROC_SUCCESS;
}

processor_status do_out(Processor* processor) {
    assert(processor);

    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    fprintf(stderr, "Answer " TYPE_T_PRINTF_SPECIFIER "\n", num);

    return PROC_SUCCESS;
}

processor_status do_jmp(Processor* processor) {
    assert(processor);
    assert(processor->about_text.pointer_on_text);

    type_t num = 0;
    processor->programm_cnt++;

    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num) != 1) {
        return PROC_EXPECTS_ARG;
    }

    processor->programm_cnt = (size_t)(num - 1);

    // getchar();

    return PROC_SUCCESS;
}

processor_status do_jb(Processor* processor) {
    type_t first_num  = 0;                                                   
    type_t second_num = 0;       
                                                
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); 

    if (second_num < first_num) {                                         
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     
    }                                                                        
    else {
        processor->programm_cnt++;
    }    
    
    return PROC_SUCCESS;
}

processor_status do_jbe(Processor* processor) {
    type_t first_num  = 0;                                                   
    type_t second_num = 0;       
                                                
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); 

    if (second_num <= first_num) {                                         
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     
    }                                                                        
    else {
        processor->programm_cnt++;
    }    
    
    return PROC_SUCCESS;
}

processor_status do_ja(Processor* processor) {
    type_t first_num  = 0;                                                   
    type_t second_num = 0;       
                                                
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); 

    if (second_num > first_num) {                                         
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     
    }                                                                        
    else {
        processor->programm_cnt++;
    }    
    
    return PROC_SUCCESS;
}

processor_status do_jae(Processor* processor) {
    type_t first_num  = 0;                                                   
    type_t second_num = 0;       
                                                
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); 

    if (second_num >= first_num) {                                         
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     
    }                                                                        
    else {
        processor->programm_cnt++;
    }    
    
    return PROC_SUCCESS;
}

processor_status do_je(Processor* processor) {
    type_t first_num  = 0;                                                   
    type_t second_num = 0;       
                                                
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); 

    if (second_num == first_num) {                                         
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     
    }                                                                        
    else {
        processor->programm_cnt++;
    }    
    
    return PROC_SUCCESS;
}

processor_status do_jne(Processor* processor) {
    type_t first_num  = 0;                                                   
    type_t second_num = 0;       
                                                
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); 

    if (second_num != first_num) {                                         
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     
    }                                                                        
    else {
        processor->programm_cnt++;
    }    
    
    return PROC_SUCCESS;
}

processor_status do_call(Processor* processor) {
    assert(processor);

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->return_stack, (type_t)processor->programm_cnt + 2));

    CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));

    return PROC_SUCCESS;
}

processor_status do_ret(Processor* processor) {
    assert(processor);

    type_t last_value = 0;
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->return_stack, &last_value));

    processor->programm_cnt = (size_t)last_value - 1; // -1 because after this function in spu processor->programm_cnt++

    return PROC_SUCCESS;
}

processor_status do_pushm(Processor* processor) {
    assert(processor);

    processor->programm_cnt++;

    type_t reg = 0;
    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &reg) != 1) {
        return PROC_EXPECTS_ARG;
    }

    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, processor->ram[processor->registers[reg]]));

    return PROC_SUCCESS;
}

processor_status do_popm(Processor* processor) {
    assert(processor);

    type_t num = 0;
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    processor->programm_cnt++;

    type_t reg = 0;
    if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &reg) != 1) {
        return PROC_EXPECTS_ARG;
    }

    processor->ram[processor->registers[reg]] = (int)num;

    draw_ram(processor);

    return PROC_SUCCESS;
}

void draw_ram(Processor* processor) {
    assert(processor);

    for (int i = 0; i < SIZE_RAM; ++i) {
        if (i % 10 == 0) 
            printf ("\n");

        if (processor->ram[i] == 1)
            printf("%c ", '*');
        else
            printf("%c ", '.');
    }

    printf("\n");
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