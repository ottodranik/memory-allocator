#pragma once

#include "allocator.h"

void shiftLeft(int *head, int n);

int getBlockFreeSize(int num);

int getBlockSize(int num);

int getBlockAddress(int num);

int getBlockBusyState(int num);

int getBlockHeadByte(int num);

char *doubleWordToBinary(int x);
