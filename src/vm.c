#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"
#include "object.h"
#include "memory.h"

VM vm;

void resetStack()
{
    vm.stackTop = vm.stack;
}

static void runtimeError(const char *format, ...)
{
    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = getLine(vm.chunk, (int)instruction);
    fprintf(stderr, "[\x1b[32mline %d\x1b[0m] in scipt \"\x1b[31m", line);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\x1b[0m\"\n", stderr);

    resetStack();
}

static void concatenate()
{
    ObjString *b = AS_STRING(pop());
    ObjString *a = AS_STRING(*top());

    int length = a->length + b->length;
    char *newString = reallocate((void *)a->chars, a->length + 1, length + 1);
    memcpy(newString + a->length, b->chars, b->length);
    newString[length] = '\0';

    a->chars = newString;
    a->length = length;
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
#define BINARY_OP(valueType, op)                        \
    do                                                  \
    {                                                   \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) \
        {                                               \
            runtimeError("Operants must be number");    \
            return INTERPRET_RUNTIME_ERROR;             \
        }                                               \
        double b = AS_NUMBER(pop());                    \
        *top() = valueType(AS_NUMBER(*top()) op b);     \
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
            if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
            {
                concatenate();
            }
            else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
            {
                double b = AS_NUMBER(pop());
                *top() = NUMBER_VAL(AS_NUMBER(*top()) + b);
            }
            else
            {
                runtimeError("Operands of '+' must be two numbers or two strings");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        case OP_SUBSTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OP_NEGATE:
        {
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operant of '-' must be a number");
                return INTERPRET_RUNTIME_ERROR;
            }
            *top() = NUMBER_VAL(-(AS_NUMBER(*top())));
            break;
        }
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_NOT:
        {
            if (IS_BOOL(peek(0)))
            {
                *top() = BOOL_VAL(!(AS_BOOL(*top())));
            }
            else if (IS_NIL(peek(0)))
            {
                *top() = BOOL_VAL(true);
            }
            else
            {
                runtimeError("Operant of '!' must be a bool or nil");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_EQUAL:
        {
            Value b = pop();
            *top() = BOOL_VAL(valuesEqual(*top(), b));
            break;
        }
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
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
    ASSERT(vm.stackTop <= vm.stack + STACK_MAX, "stack overflow");
    *vm.stackTop = value;
    vm.stackTop++;
}

#define empty() (vm.stackTop == vm.stack)

Value pop()
{
    ASSERT(!empty(), "stack is empty, can't pop");
    vm.stackTop--;
    return *vm.stackTop;
}

Value *top()
{
    ASSERT(!empty(), "stack is empty, can't get top");
    return vm.stackTop - 1;
}

Value peek(int distance)
{
    ASSERT(vm.stackTop >= vm.stack + distance + 1, "there's no enough item in stack");
    return vm.stackTop[-1 - distance];
}