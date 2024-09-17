#include <stdio.h>

#include "debug.h"

void disassembleChunk(Chunk *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;)
    {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char *name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char *name, Chunk *chunk, int offset, const uint8_t len)
{
    int constant = 0;
    if (len == 2)
    {
        constant = chunk->code[offset + 1];
    }
    else if (len == 4)
    {
        constant = *((uint32_t *)&chunk->code[offset]) >> 8;
    }
    else
    {
        PANIC("wrong constant instruction length");
    }
    printf("%-21s %d '", name, constant);
    printValue(chunk->constants.value[constant]);
    printf("'\n");
    return offset + (int)len;
}

static int jumpInstruction(const char *name, Chunk *chunk, int offset, int sign)
{
    int jump = (chunk->code[offset + 1] << 8) | chunk->code[offset + 2];
    printf("%-21s %d -> %d \n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

int disassembleInstruction(Chunk *chunk, int offset)
{
    printf("%04d ", offset);
    if (offset > 0 && getLine(chunk, offset) == getLine(chunk, offset - 1))
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", getLine(chunk, offset));
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction)
    {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset, 2);

    case OP_CONSTANT_LONG:
        return constantInstruction("OP_CONSTANT_LONG", chunk, offset, 4);

    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);

    case OP_SUBSTRACT:
        return simpleInstruction("OP_SUBSTRACT", offset);

    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);

    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);

    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);

    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);

    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);

    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);

    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);

    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);

    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);

    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);

    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);

    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);

    case OP_POP:
        return simpleInstruction("OP_POP", offset);

    case OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset, 2);

    case OP_DEFINE_GLOBAL_LONG:
        return constantInstruction("OP_DEFINE_GLOBAL_LONG", chunk, offset, 4);

    case OP_GET_GLOBAL:
        return constantInstruction("OP_GET_GLOBAL", chunk, offset, 2);

    case OP_GET_GLOBAL_LONG:
        return constantInstruction("OP_GET_GLOBAL_LONG", chunk, offset, 4);

    case OP_SET_GLOBAL:
        return constantInstruction("OP_SET_GLOBAL", chunk, offset, 2);

    case OP_SET_GLOBAL_LONG:
        return constantInstruction("OP_SET_GLOBAL_LONG", chunk, offset, 4);

    case OP_GET_LOCAL:
        return constantInstruction("OP_GET_LOCAL", chunk, offset, 2);

    case OP_GET_LOCAL_LONG:
        return constantInstruction("OP_GET_LOCAL_LONG", chunk, offset, 4);

    case OP_SET_LOCAL:
        return constantInstruction("OP_SET_LOCAL", chunk, offset, 2);

    case OP_SET_LOCAL_LONG:
        return constantInstruction("OP_SET_LOCAL_LONG", chunk, offset, 4);

    case OP_JUMP:
        return jumpInstruction("OP_JUMP", chunk, offset, 1);

    case OP_JUMP_BACK:
        return jumpInstruction("OP_JUMP_BACK", chunk, offset, -1);

    case OP_JUMP_IF_FALSE:
        return jumpInstruction("OP_JUMP_IF_FALSE", chunk, offset, 1);

    default:
        printf("Unknow code %d\n", instruction);
        return offset + 1;
    }
}