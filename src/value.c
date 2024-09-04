#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "value.h"

void initValueArray(ValueArray *array)
{
    array->capacity = 0;
    array->count = 0;
    array->value = NULL;
}

void freeValueArray(ValueArray *array)
{
    FREE_ARRAY(uint8_t, array->value, array->capacity);
    initValueArray(array);
}

void writeValueArray(ValueArray *array, Value value)
{
    if (array->capacity < array->count + 1)
    {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->value = GROW_ARRAY(Value, array->value, oldCapacity, array->capacity);
    }

    array->value[array->count] = value;
    array->count++;
}

void printValue(Value value)
{
    printf("%g", value);
}