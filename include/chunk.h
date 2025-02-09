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
    OP_PRINT,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_DEFINE_GLOBAL_LONG,
    OP_GET_GLOBAL,
    OP_GET_GLOBAL_LONG,
    OP_SET_GLOBAL,
    OP_SET_GLOBAL_LONG,
    OP_GET_LOCAL,
    OP_GET_LOCAL_LONG,
    OP_SET_LOCAL,
    OP_SET_LOCAL_LONG,
    OP_JUMP_IF_FALSE,
    OP_JUMP,
    OP_JUMP_BACK,
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
void writeConst(Chunk *chunk, int index, int line, uint8_t shortInstruction,
                uint8_t longInstruction, unsigned int longRange, int longLengths,
                const char *oerverflowMessage);

#define writeGlobal(chunk, global, line)                                     \
    writeConst(chunk, global, line, OP_DEFINE_GLOBAL, OP_DEFINE_GLOBAL_LONG, \
               MAX_LONG_CONSTANT, 3, "the max number of globals should not over 16777215.")

#define writeGetGlobal(chunk, global, line)                            \
    writeConst(chunk, global, line, OP_GET_GLOBAL, OP_GET_GLOBAL_LONG, \
               MAX_LONG_CONSTANT, 3, "Unreachable code.")

#define writeSetGlobal(chunk, global, line)                            \
    writeConst(chunk, global, line, OP_SET_GLOBAL, OP_SET_GLOBAL_LONG, \
               MAX_LONG_CONSTANT, 3, "Unreachable code.")

#endif