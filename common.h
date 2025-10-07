#ifndef COMMON_H_
#define COMMON_H_


enum code_comand {
    CMD_PUSH  = 1,
    CMD_POP   = 2,
    CMD_ADD   = 3,
    CMD_SUB   = 4,
    CMD_DIV   = 5,
    CMD_MUL   = 6,
    CMD_SQRT  = 7,
    CMD_POW   = 8,
    CMD_IN    = 9,
    CMD_OUT   = 10,
    CMD_HLT   = 11,
    CMD_JMP   = 12,
    CMD_JB    = 13, // <
    CMD_JBE   = 14, // <=
    CMD_JA    = 15, // >
    CMD_JAE   = 16, // >=
    CMD_JE    = 17, // ==
    CMD_JNE   = 18, // !=
    CMD_POPR  = 42,
    CMD_PUSHR = 33
};

#endif // COMMON_H_