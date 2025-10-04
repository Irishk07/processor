#include "processor.h"

int main() {
    Processor processor = {};

    ProcCtor(&processor, "processor/byte_code.txt");

    ProcDtor(&processor);

    return 0;
}