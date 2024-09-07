#include <stdio.h>

#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"

VM vm;

void resetStack()
{
    vm.stackTop = vm.stack;
}

void initVM()
{
    resetStack();
}

void freeVM()
{
}

#define READ_BYTE() (*vm.ip++)
Value readConstant(int len)
{
    int constant = 0;
    for (int i = 0; i < len; i++)
    {
        constant = (constant << 8) + READ_BYTE();
    }
    return vm.chunk->constants.value[constant];
}

InterpretResult run()
{
#define READ_CONSTANT() (vm.chunk->constants.value[READ_BYTE()])
#define BINARY_OP(op)         \
    do                        \
    {                         \
        double b = pop();     \
        *top() = *top() op b; \
    } while (0)

    for (;;)
    {
#ifdef DEBUG_TRACE_EXCUTION
        printf("          ");
        for (Value *i = vm.stack; i < vm.stackTop; i++)
        {
            printf("[");
            printValue(*i);
            printf("]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_CONSTANT_LONG:
        {
            Value constant = readConstant(3);
            push(constant);
            break;
        }
        case OP_ADD:
            BINARY_OP(+);
            break;
        case OP_SUBSTRACT:
            BINARY_OP(-);
            break;
        case OP_MULTIPLY:
            BINARY_OP(*);
            break;
        case OP_DIVIDE:
            BINARY_OP(/);
            break;
        case OP_NEGATE:
        {
            *top() = -(*top());
            break;
        }
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        }
    }
#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(const char *source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

void push(Value value)
{
    if (vm.stackTop > vm.stack + STACK_MAX)
    {
        PANIC("stack overflow");
    }
    *vm.stackTop = value;
    vm.stackTop++;
}

#define empty() (vm.stackTop == vm.stack)

Value pop()
{
    if (empty())
    {
        PANIC("try to pop from empty stack");
    }
    vm.stackTop--;
    return *vm.stackTop;
}

Value *top()
{
    if (empty())
    {
        PANIC("try to get top from an empty stack");
    }
    return vm.stackTop - 1;
}