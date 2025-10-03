#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

#include "canary.h"
#include "processor.h"
#include "stack.h"
#include "string_functions.h"
#include <sys/stat.h>


processor_status DataReSize(Processor* processor, size_t new_capacity) {
    assert(processor);
    assert(processor->byte_code_data.data);

    type_t* temp_data = (type_t*)my_recalloc(processor->byte_code_data.data, new_capacity * sizeof(type_t), 
                                                                             processor->byte_code_data.capacity * sizeof(type_t));

    if (temp_data == NULL) {
        CHECK_ERRORS_PROC(NOT_ENOUGH_MEMORIES,     free(temp_data));
    }

    processor->byte_code_data.data = temp_data;

    return PROC_SUCCESS;
}

status_cmp SetCommand(Processor* processor, const char* expecting_comand, char* comand, type_t code_expecting_comand) {
    assert(processor);
    assert(processor->byte_code_data.data);

    if(!strcmp(expecting_comand, (const char*)comand)) {
        processor->byte_code_data.data[processor->byte_code_data.size] = code_expecting_comand; // TODO
        processor->byte_code_data.size++;

        return EQUAL;
    }

    return DIFFERENT;
}

processor_status Assemblirovanie(const char* stream, Processor* processor) {
    assert(processor);
    assert(processor->byte_code_data.data);

    FILE* text = fopen(stream, "r");
    if (text == NULL) {
        CHECK_ERRORS_PROC(OPEN_ERROR, perror("Error is"));
    }

    //char address = 0;
    char comand[5] = {}; // FIXME

    while (fscanf(text, "%s", comand) != EOF) {
        if (processor->byte_code_data.size == processor->byte_code_data.capacity) {
            size_t new_capacity = processor->byte_code_data.capacity * REALLOC_COEFF;

            CHECK_ERRORS_PROC(DataReSize(processor, new_capacity));

            processor->byte_code_data.capacity = new_capacity;
        }

        size_t old_size = processor->byte_code_data.size;

        if (SetCommand(processor, "PUSH", comand, PUSH)) {
            type_t number = 0;

            if (fscanf(text, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
                processor->byte_code_data.data[processor->byte_code_data.size] = number;
                processor->byte_code_data.size++;
            }
            else {
                CHECK_ERRORS_PROC(EXPECTS_NUMBER,       free(processor));
            }
        }

        SetCommand(processor, "ADD",  comand, ADD);
        SetCommand(processor, "SUB",  comand, SUB);
        SetCommand(processor, "DIV",  comand, DIV);
        SetCommand(processor, "MUL",  comand, MUL);
        SetCommand(processor, "SQRT", comand, SQRT);
        SetCommand(processor, "POW",  comand, POW);
        SetCommand(processor, "OUT",  comand, OUT);

        if (SetCommand(processor, "HLT", comand, HLT)) {
            break;
        }

        if (processor->byte_code_data.size == old_size) {
            CHECK_ERRORS_PROC(UNKNOWN_COMAND,        free(processor->byte_code_data.data));
        }        
    }

    if (processor->byte_code_data.data[processor->byte_code_data.size - 1] != HLT) {
        CHECK_ERRORS_PROC(EXPECTS_HLT,       free(processor->byte_code_data.data));
    }


    if (fclose(text) == EOF) {
        CHECK_ERRORS_PROC(CLOSE_ERROR,      perror("Error is"));
    }

    return PROC_SUCCESS;
}

processor_status CreateExeFile(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_data.data);

    FILE* text = fopen(processor->byte_code_file, "w");
    if (text == NULL) {
        CHECK_ERRORS_PROC(OPEN_ERROR,       perror("Error is"));
    }

    for (size_t i = 0; i < processor->byte_code_data.size; ++i) {
        fprintf(text, TYPE_T_PRINTF_SPECIFIER "\n", processor->byte_code_data.data[i]);
    }

    if (fclose(text) == EOF) {
        CHECK_ERRORS_PROC(CLOSE_ERROR,      perror("Error is"));
    }

    return PROC_SUCCESS;
}