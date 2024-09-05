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
    printf("%-16s %d '", name, constant);
    printValue(chunk->constants.value[constant]);
    printf("'\n");
    return offset + (int)len;
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

    default:
        printf("Unknow code %d\n", instruction);
        return offset + 1;
    }
}