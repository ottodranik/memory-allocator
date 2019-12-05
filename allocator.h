#pragma once
#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 256 // выделение памяти для программы (в байтах)

/* http://www.c-cpp.ru/books/bitovye-operatory
 * Диапазон unsigned int: [0, +4 294 967 295]
 * 1111 1111 1111 1111 = 2 байта = 65 535
 * 1111 1111 1111 1111 1111 1111 1111 1111 = 4 байта = Двойное слово = 4 294 967 295
 */
#define WORD 2
#define DWORD 4
#define BITS_IN_BYTE 8
#define BITS_IN_DWORD pow(2, DWORD * BITS_IN_BYTE)

/** NOTE!
 * https://stackoverflow.com/questions/739727/c-why-isnt-size-t-a-c-keyword
 * sizeof is a C keyword. It returns the size in a type named size_t. 
 * However, size_t is not a keyword, but is defined primarily in stddef.h 
 * and probably other C standard header files too. 
 */
// typedef int size_t;

typedef enum { false, true } bool;

int *globalMem;

void *memAlloc(size_t size);

void *memRealloc(void *addr, size_t size);

void memFree(void *addr);

int getBlockHeadByte(int num);

int getBlockBusy(int num);

int getBlockAddress(int num);

int getBlockSize(int num);

int getBlockFreeSize(int num);

char *doubleWordToBinary(int x);
