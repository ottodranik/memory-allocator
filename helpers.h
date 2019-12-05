#pragma once
#include "allocator.h"

// Константы
#define HEADER_BYTE 'H' // символ хэдера
#define ALLOCED_BYTE 'A' // символ информации
#define EMPTY_BYTE '_' // символ пустого байта
#define FREE_BYTE '*' // символ свободной памяти

void memDump();

void printStr(char str[]);

void printBlockInfo(size_t sizeToAllocate, int headerValue, int *headerAddress, int *dataAddress);
