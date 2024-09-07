#ifndef _clox_compiler_h
#define _clox_compiler_h

#include "chunk.h"

bool compile(const char *source, Chunk *chunk);

#endif