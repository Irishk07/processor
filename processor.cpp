#include <assert.h>
#include <stdlib.h>

#include "processor.h"

#include "check_errors.h"
#include "stack.h"


processor_status ProcCtor(Processor* processor, const char* file_name) {
    assert(processor);

    processor->byte_code_file = file_name;

    processor->byte_code_data.capacity = SIZE_BYTE_CODE;
    processor->byte_code_data.size     = 0;

    processor->byte_code_data.data = (type_t*)calloc(processor->byte_code_data.capacity, sizeof(type_t));

    if (processor->byte_code_data.data == NULL){
        CHECK_ERRORS_PROC(NOT_ENOUGH_MEMORIES);
    }

    CHECK_ERRORS_STACK(STACK_CREATE(processor->stack, DEFAULT_START_CAPACITY), free(processor->byte_code_data.data));

    //StackDump(&processor->stack, 0, DUMP_VAR_INFO);

    return PROC_SUCCESS;
}

processor_status ProcDtor(Processor* processor) {
    assert(processor);

    free(processor->byte_code_data.data);

    StackDtor(&processor->stack);

    // processor->byte_code_data = {};
    // *processor = {};

    return PROC_SUCCESS;
}