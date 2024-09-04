#include <stdlib.h>

#include "chunk.h"
#include "memory.h"
void initLine(Line *line)
{
    line->count = 0;
    line->capacity = 0;
    line->line = NULL;
    line->sum = NULL;
}

void freeLine(Line *line)
{
    FREE_ARRAY(int, line->line, line->capacity);
    FREE_ARRAY(int, line->sum, line->capacity);
    initLine(line);
}

void writeLine(Line *line, int line_num)
{
    // if this bytecode is in the same line with last bytecode,
    // then we don't need to grow the line
    if (line->count != 0 && line->line[line->count - 1] == line_num)
    {
        line->sum[line->count - 1]++;
        return;
    }

    if (line->capacity < line->count + 1)
    {
        int oldCapacity = line->capacity;
        line->capacity = GROW_CAPACITY(oldCapacity);
        line->line = GROW_ARRAY(int, line->line, oldCapacity, line->capacity);
        line->sum = GROW_ARRAY(int, line->sum, oldCapacity, line->capacity);
    }

    line->line[line->count] = line_num;
    if (line->count == 0)
    {
        line->sum[0] = 0;
    }
    else
    {
        line->sum[line->count] = line->sum[line->count - 1] + 1;
    }
    line->count++;
}

int getLine(Chunk *chunk, int offset)
{
    for (int i = 0; i < chunk->lines.count; i++)
    {
        if (chunk->lines.sum[i] >= offset)
        {
            return chunk->lines.line[i];
        }
    }
    PANIC("unreachable code.");
    return -1;
}

void initChunk(Chunk *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initLine(&chunk->lines);
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeLine(&chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line)
{
    if (chunk->capacity < chunk->count + 1)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    writeLine(&chunk->lines, line);
    chunk->count++;
}

int addConstant(Chunk *chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void writeConstant(Chunk *chunk, Value value, int line)
{
    int constant = addConstant(chunk, value);
    if (constant <= (unsigned int)UINT8_MAX)
    {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, constant, line);
    }
    else if (constant <= (unsigned int)MAX_LONG_CONSTANT)
    {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        uint8_t *constant_ = (uint8_t *)&constant;
        for (int i = 0; i <= 2; i++)
        {
            writeChunk(chunk, constant_[i], line);
        }
    }
    else
    {
        PANIC("the max number of constant should not over 16777215");
    }
}