#include <stdio.h>
#include <string.h>

#include "object.h"
#include "vm.h"
#include "value.h"

static Obj *allocateObject(size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;

    object->next = vm.objects;
    vm.objects = object;
    return object;
}

#define ALLOCATE_OBJ(type, objectType) \
    ((type *)allocateObject(sizeof(type), objectType))

ObjString *allocateSting(const char *chars, int length, uint32_t hash)
{
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet(string, NIL_VAL, &vm.strings);
    return string;
}

uint32_t hashString(const char *key, int length)
{
    uint32_t hash = 2166136261U;
    for (int i = 0; i < length; i++)
    {
        hash ^= key[i];
        hash *= 16777619U;
    }
    return hash;
}

ObjString *copyString(const char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL)
        return interned;

    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateSting(heapChars, length, hash);
}

void printObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
    case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    }
}

ObjString *takeString(const char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL)
    {
        FREE_ARRAY(char, (char *)chars, length + 1);
        return interned;
    }

    return allocateSting(chars, length, hash);
}

void freeObject(Obj *object)
{
    switch (object->type)
    {
    case OBJ_STRING:
    {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY(char, (char *)string->chars, string->length);
        FREE(ObjString, string);
        break;
    }
    }
}

void freeObjects()
{
    Obj *object = vm.objects;
    while (object != NULL)
    {
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}