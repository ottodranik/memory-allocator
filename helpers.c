#include "helpers.h"

// Перевод каретки в memDump
void newLineBreak(int *pointer) {
	if ((pointer - globalMem) % 16 == 0) {
		printf("\n");
	}
}

// Вывод текста на экран
void printStr(char str[]) {
	printf("%s\n\n", str);
}

// Debug-функция для просмотра указателей на блок, размера блока и хэдера
void printBlockInfo(size_t sizeToAllocate, int headerValue, int *blockHeaderAddress, int *blockStartAddress) {
	int blockEndAddress = ((int)sizeToAllocate % 4 == 0)
		? (int)blockStartAddress + sizeToAllocate
		: (int)blockStartAddress + (sizeToAllocate - sizeToAllocate % 4) + 4;
	printf("--------------------------------------------\n");
	printf("SIZE TO ALLOCATE: %d bytes\n", (int)sizeToAllocate);
	printf("SIZE ALLOCATED: %d bytes\n", getBlockSize(headerValue) * DWORD);
	printf("HEADER value = %d = %s\n", headerValue, doubleWordToBinary(headerValue));
	printf("HEADER starts at: %d\n", (int)blockHeaderAddress);
	printf("BLOCK starts at: %d\n", (int)blockStartAddress);
	printf("BLOCK ends at: %d\n", (int)blockEndAddress);
	printf("globalMem starts at: %d\n", (int)globalMem);
	printf("globalMem ends at: %d\n", (int)globalMem + MEMORY_SIZE);
	printf("--------------------------------------------\n\n");
}

void memDump() {
	int *pointer = globalMem;

	/* Указатель двигается на 4 байта за шаг цикла и проверяем выделена ли память
	 * если нет: выводим пусто FREE_BYTE
	 * если есть: выводим либо данные ALLOCED_BYTE, либо хэдер-данные HEADER_BYTE
	 */
	while (pointer < globalMem + MEMORY_SIZE / 4) { // рисуем 4 линии по 64 байта

		// Ищем хэдер. Если не хэдер, то выводим пустое слово
		if (getBlockHeadByte(*pointer) != 1) {
			printf("%c %c %c %c ", FREE_BYTE, FREE_BYTE, FREE_BYTE, FREE_BYTE);
			
			pointer++; /* Указатель перемещается на 4 байта, потому что указатель имеет
			            * свой собственный адрес и размер памяти в стеке (4 байта в x86),
									* поэтому инкримент для указателя на int = 4ёх байтное слово
						      * пример: pointer = 373298512, pointer++ = 373298516
									*/
			
			newLineBreak(pointer);

			continue;
		}

		int size = getBlockSize(*pointer); // получаем размер выделенного блока (в словах DWORD)
		int free = getBlockFreeSize(*pointer); // получаем пустое место в выделенном блоке (в байтах)

		// Выводим хэдер-данные и сдвигаем указатель у следующему слову
		printf("%c %c %c %c ", HEADER_BYTE, HEADER_BYTE, HEADER_BYTE, HEADER_BYTE);
		pointer++;

		newLineBreak(pointer);

		// Для блока размером size-1 показываем, что память выделена, и двигаем указатель.
		// (size-1 - потому что последнее слово выводим отдельно)
		for (int i = 0; i < size - 1; i++) {
			printf("%c %c %c %c ", ALLOCED_BYTE, ALLOCED_BYTE, ALLOCED_BYTE, ALLOCED_BYTE);
			pointer++;
			
			newLineBreak(pointer);
		}

		// Последнее слово может быть заполнено не полностью,
		// поэтому в начале выводим заполненную часть
		for (int i = 0; i < 4 - free; i++) {
			printf("%c ", ALLOCED_BYTE);
		}
		// а потом выводим пустую часть (если она есть)
		for (int i = 0; i < free; i++) {
			printf("%c ", EMPTY_BYTE);
		}

		pointer++;

		newLineBreak(pointer);
	}

	printf("\n\n");
}
