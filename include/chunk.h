#ifndef _clox_chunk_h
#define _clox_chunk_h

#include "common.h"
#include "value.h"

#define MAX_LONG_CONSTANT 0xffffff

typedef enum
{
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_TRUE,
    OP_FALSE,
    OP_NIL,
    OP_ADD,
    OP_SUBSTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_RETURN,
} OpCode;

typedef struct
{
    int count;
    int capacity;
    int *sum;
    int *line;
} Line;

typedef struct
{
    int count;
    int capacity;
    uint8_t *code;
    Line lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);
int getLine(Chunk *chunk, int offset);
void writeConstant(Chunk *chunk, Value value, int line);

#endif