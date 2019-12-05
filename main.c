// Главный файл программы MemoryAllocato

#include <stdlib.h>
#include "helpers.h"
#include "allocator.h"

int main() {
	// malloc() возвращает адрес на первый байт области памяти размером size байт,
	// кото­рая была выделена из кучи. Если памяти недостаточно, то возвращается нулевой указатель.
	globalMem = malloc(MEMORY_SIZE);
	if (globalMem == NULL) {
		return 0;
	}
	
	printStr("\na = memAlloc(10)");
	void *a = memAlloc(10); // 10 байт, которые будут выровнены по границе DWORD 
	memDump();
	printStr("");

	printStr("b = memAlloc(8)");
	void *b = memAlloc(8); // ровно 2 DWORD слова
	memDump();
	printStr("");

	printStr("c = memAlloc(15)");
	void *c = memAlloc(15); // 15 байт, которые будут выровнены по границе DWORD 
	memDump();
	printStr("");

	printStr("d = memAlloc(1)");
	void *d = memAlloc(1); // 1 байт, который будет выровнен по границе DWORD 
	memDump();
	printStr("");

	printStr("memFree(b)");
	memFree(b);
	memDump();
	printStr("");

	printStr("memRealloc(c, 5)");
	void *newC = memRealloc(c, 5);
	memDump();
	printStr("");

	printStr("memRealloc(a, 19)");
	void *newA = memRealloc(a, 19);
	memDump();
	printStr("");

	printStr("memRealloc(a, 25)");
	void *newA2 = memRealloc(newA, 25);
	memDump();
	printStr("");

	printStr("memRealloc(c, 41 )");
	void *newC2 = memRealloc(newC, 41); // 27 байт - проверка поиска свободных блоков ПЕРЕД искомым
	memDump();
	printStr("");

	free(globalMem);
  return 0;
}

