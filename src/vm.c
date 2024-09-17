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
    ObjString *a = AS_STRING(pop());

    int length = a->length + b->length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString *result = takeString(chars, length);
    push(OBJ_VAL(result));
}

static bool isFalsey(Value value)
{
    if (IS_NIL(value))
        return true;
    if (IS_BOOL(value))
        return !AS_BOOL(value);
    runtimeError("Condition can only be bool or nil.");
    return false;
}

void initVM()
{
    resetStack();
    vm.objects = NULL;
    initTable(&vm.strings);
    initTable(&vm.globals);
}

void freeVM()
{
    freeTable(&vm.strings);
    freeTable(&vm.globals);
    freeObjects();
}

#define READ_BYTE() (*vm.ip++)

static int readBytes(int len)
{
    int bytes = 0;
    for (int i = 0; i < len; i++)
    {
        bytes = (bytes << 8) + READ_BYTE();
    }
    return bytes;
}

Value readConstant(int len)
{
    int constant = 0;
    for (int i = 0; i < len; i++)
    {
        constant = (constant << 8) + READ_BYTE();
    }
    return vm.chunk->constants.value[constant];
}

Value readLocal(int len)
{
    int local = 0;
    for (int i = 0; i < len; i++)
    {
        local = (local << 8) + READ_BYTE();
    }
    return vm.stack[local];
}

InterpretResult run()
{
#define READ_CONSTANT() (vm.chunk->constants.value[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define READ_STRING_LONG(len) AS_STRING(readConstant(len))
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
        case OP_PRINT:
        {
            printValue(pop());
            printf("\n");
            break;
        }
        case OP_POP:
            pop();
            break;
        case OP_DEFINE_GLOBAL:
        {
            ObjString *name = READ_STRING();
            tableSet(name, peek(0), &vm.globals);
            pop();
            break;
        }
        case OP_DEFINE_GLOBAL_LONG:
        {
            ObjString *name = READ_STRING_LONG(3);
            tableSet(name, peek(0), &vm.globals);
            pop();
            break;
        }
        case OP_GET_GLOBAL:
        {
            ObjString *name = READ_STRING();
            Value value;
            if (!tableGet(&vm.globals, name, &value))
            {
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            push(value);
            break;
        }
        case OP_GET_GLOBAL_LONG:
        {
            ObjString *name = READ_STRING_LONG(3);
            Value value;
            if (!tableGet(&vm.globals, name, &value))
            {
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            push(value);
            break;
        }
        case OP_SET_GLOBAL:
        {
            ObjString *name = READ_STRING();
            Value value = peek(0);
            if (tableSet(name, value, &vm.globals))
            {
                tableDelete(&vm.globals, name);
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_SET_GLOBAL_LONG:
        {
            ObjString *name = READ_STRING_LONG(3);
            Value value = peek(0);
            if (tableSet(name, value, &vm.globals))
            {
                tableDelete(&vm.globals, name);
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_GET_LOCAL:
        {
            uint8_t slot = READ_BYTE();
            push(vm.stack[slot]);
            break;
        }
        case OP_GET_LOCAL_LONG:
        {
            int slot = readBytes(3);
            push(vm.stack[slot]);
            break;
        }
        case OP_SET_LOCAL:
        {
            uint8_t slot = READ_BYTE();
            vm.stack[slot] = peek(0);
            break;
        }
        case OP_SET_LOCAL_LONG:
        {
            int slot = readBytes(3);
            vm.stack[slot] = peek(0);
            break;
        }
        case OP_JUMP:
        {
            uint16_t offset = readBytes(2);
            vm.ip += offset;
            break;
        }
        case OP_JUMP_BACK:
        {
            uint16_t offset = readBytes(2);
            vm.ip -= offset;
            break;
        }
        case OP_JUMP_IF_FALSE:
        {
            uint16_t offset = readBytes(2);
            if (isFalsey(peek(0)))
                vm.ip += offset;
            break;
        }
        }
    }
#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
#undef READ_STRING
#undef READ_STRING_LONG
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