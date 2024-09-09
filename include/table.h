#ifndef _clox_table_h
#define _clox_table_h

#include "object.h"

#define TABLE_MAX_LOAD 0.75

typedef struct
{
    ObjString *key;
    Value value;
} Entry;

typedef struct
{
    int capacity;
    int count;
    Entry *entries;
} Table;

void initTable(Table *table);
void freeTable(Table *table);
bool tableSet(ObjString *key, Value value, Table *table);
void tableAddAll(Table *from, Table *to);
bool tableGet(Table *table, ObjString *key, Value *value);
bool tableDelete(Table *table, ObjString *key);
ObjString *tableFindString(Table *table, const char *chars, int length, uint32_t hash);

#endif