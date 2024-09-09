#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"
#include "object.h"

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
    switch (value.type)
    {
    case VAL_BOOL:
        printf("%s", AS_BOOL(value) ? "true" : "false");
        return;
    case VAL_NIL:
        printf("%s", "nil");
        return;
    case VAL_NUMBER:
        printf("%g", AS_NUMBER(value));
        return;
    case VAL_OBJ:
        printObject(value);
        return;
    }
}

bool valuesEqual(Value a, Value b)
{
    if (a.type != b.type)
        return false;
    switch (a.type)
    {
    case VAL_BOOL:
        return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
        return false;
    case VAL_NUMBER:
        return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_OBJ:
        return AS_OBJ(a) == AS_OBJ(b);
    default:
        PANIC("Unreachalbe code.");
        return false;
    }
}