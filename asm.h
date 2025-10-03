#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "processor.h"


processor_status DataReSize(Processor* processor, size_t new_capacity);

status_cmp SetCommand(Processor* processor, const char* expecting_comand, char* comand, type_t code_expecting_comand);

processor_status Assemblirovanie(const char* stream, Processor* processor);

processor_status CreateExeFile(Processor* processor);


#endif //ASM_H_