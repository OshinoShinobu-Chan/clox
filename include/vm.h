#ifndef _clox_vm_h
#define _clox_vm_h

#include "chunk.h"
#include "value.h"
#include "table.h"

#define STACK_MAX (1 << 16)

typedef struct
{
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
    Table globals;
    Table strings;
    Obj *objects;
} VM;

extern VM vm;

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(const char *source);
void push(Value value);
Value pop();
Value *top();
Value peek(int distance);

#endif