#ifndef _clox_value_h
#define _clox_value_h

#include "common.h"

typedef double Value;

typedef struct
{
    int capacity;
    int count;
    Value *value;
} ValueArray;

void initValueArray(ValueArray *array);
void freeValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void printValue(Value value);

#endif