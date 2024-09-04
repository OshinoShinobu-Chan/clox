#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"

int main(int argc, char *argv[])
{
    Chunk chunk;
    initChunk(&chunk);

    for (int i = 0; i < (1 << 24) + 1; i++)
    {
        writeConstant(&chunk, i, i);
    }

    writeChunk(&chunk, OP_RETURN, 511);

    disassembleChunk(&chunk, "test chunk");

    freeChunk(&chunk);
    return 0;
}