#include "allocator.h"
#include "header-helpers.h"

/* ==== Основные методы Аллокатора Памяти ==== */

/**
 * Поиск места в памяти для блока размером @size
 * @return: Указатель на место в памяти под блок с хэдером или @NULL
*/
int *getFreeSpace(int size, int ignoreHeader) {
	int *start = globalMem;
	
	do {
		if (size > globalMem + MEMORY_SIZE - start) {
			return NULL;
		}
		
		int currSize = 0;
		int currHeader = 0;

		int *j = start;
		while (j < globalMem + MEMORY_SIZE) {
			currHeader = *(start + currSize / 4);
			if (currHeader != ignoreHeader && getBlockHeadByte(currHeader) == 1) {
				// в getBlockSize передаём значение текущего проверяемого хэдера.
				// увеличиваем сдвиг на размер блока и ищем дальше.
				// +1 из-за того, что блок начинается после хэдера (+1 слово -> 4 байта)
				start = j + getBlockSize(currHeader) + 1;
				break;
			}

			currSize += 4; // добавляем 4 байта и выходим на новое DWORD слово
			if (currSize >= size) { // если дошли 
				return start;
			}

			j++;
		}
	} while (start < globalMem + MEMORY_SIZE); // двигаемся пока не выйдем за границы выделенной памяти

	return NULL;
}

findNextBlock(void *afterCurrBlockAddr, int deltaSize) {
	int *start = afterCurrBlockAddr;
	
	do {
		if (deltaSize > globalMem + MEMORY_SIZE - start) {
			return globalMem + MEMORY_SIZE - start;
		}
		
		int currSize = 0;

		int *j = start;
		while (j < globalMem + MEMORY_SIZE) {
			if (getBlockHeadByte(*(start + currSize / 4)) == 1) {
				return currSize; // если упёрлись в хэдер, то возвращаем сколько нашли свободных байт
			}

			currSize += 4; // добавляем 4 байта и выходим на новое DWORD слово
			if (currSize >= deltaSize) { // если дошли 
				return currSize;
			}

			j++;
		}
	} while (start < globalMem + MEMORY_SIZE); // двигаемся пока не выйдем за границы выделенной памяти
}

/**
 * Поиск места в памяти для блока размером @size и, если место найдено, 
 * инициализация хэдера и возврат указателя на место под данные.
 * @return: Указатель на место в памяти под данные блока (без хэдера) или @NULL
*/
void *memAlloc(size_t size) {
	// Найти свободное место учитывая + 1 *sizeof(int) для хэдера
	int *poiterToNew = getFreeSpace(size + 1 * sizeof(int), 0);

	if (!poiterToNew) {
		return NULL;
	}

	int header = initHeader(true, poiterToNew + 1, size);
	*poiterToNew = header;

	return poiterToNew + 1;
}

/**
 * под данные блока или @NULL.
 * Алгоритм поиска места в памяти для блока размером @size по уже выделенному адресу:
 * 1. Поиск в уже выделенном памяти и переопределение блока
 * 2. Перезапись частично, если новый блок < старого
 * 3. Выделение нового блока в дампе, если новый блок > старого и очищение старого блока
 * @return: Новый или переопредедённый старый указатель на место в памяти 
 */
void *memRealloc(void *addr, size_t size) {
	if ((int *)addr < globalMem + 1 || (int *)addr > globalMem + MEMORY_SIZE - 4) {
		return NULL;
	}

  // ищем свободное место в уже выделенной памяти (в байтах)
	int header = *((int*)addr - 1);
	int sizeOfBlock = getBlockSize(header) * 4; // умножаем на значение DWORD слова
	int sizeOfBusyBlock = sizeOfBlock - getBlockFreeSize(header);

  // перезаписываем блок полностью, если размеры нового и старого блоков совпадают
	if (size == sizeOfBusyBlock) {
		return addr;
	}

  // перезаписываем блок частично, если хватает памяти в выделенном блоке
	if (size <= sizeOfBlock) {
		int newHeader = initHeader(true, (int*)addr, size);
		int *headPointer = (int*)addr - 1;
		*headPointer = newHeader;
		return addr;
	}

	// если выделенной ранее области памяти не хватает:

	// этап 1:
	// * пытаемся посчитать доступное место справа от блока до следующего блока
	// * если хватило места, то выделяем память
	// * перезаписываем в тот же хедер новые значения размера и адреса блока 
	sizeOfBlock += findNextBlock(addr + sizeOfBlock, size - sizeOfBlock);
	if (size <= sizeOfBlock) {
		memFree(addr);
		void *newAddress = addr;
		int newHeader = initHeader(true, (int*)newAddress, size);
		int *headPointer = (int*)newAddress - 1;
		*headPointer = newHeader;
		return newAddress;
	}

	// этап 2:
	// * проверяем, есть ли место в памяти слева от текущего блока (с начала дампа),
	// * игнорируем хэдер текущего блока при поиска места
	// * если место слева есть - вернётся адрес для перезаписи блока
	// * если места слева нет - вернётся адрес для создание нового блока справа от него
	// * возвращается либо значение СЛЕВА от текущего хэдера, либо значение СПРАВА
	int *newBlockPointer = getFreeSpace(size + 1 * sizeof(int), header);
	if (newBlockPointer == NULL) {
		return NULL;
	}
	memFree(addr);
	int newBlockHeader = initHeader(true, newBlockPointer + 1, size);
	*newBlockPointer = newBlockHeader;

	return newBlockPointer;
}

/** 
 * Обращается к адресу хэдера блока и освобождает его
*/
void memFree(void *addr) {
	if ((int *)addr < globalMem || (int *)addr > globalMem + MEMORY_SIZE - 4) {
		return;
	}

	int *headPtr = (int *)addr - 1;

	*headPtr = 0;
}

/** 
 * Инициализирует хэдер блока по заданному адресу
 * и возвращает int значение самого хэдера
*/
int initHeader(bool busy, int *address, size_t size) {
	if (size < 1
		|| size > MEMORY_SIZE - 1
		|| (int)address - (int)globalMem < 4
		|| (int)address - (int)globalMem + 4 > MEMORY_SIZE
		|| ((int)address - (int)globalMem) % 4 != 0
	) {
		return -1;
	}

	// 1 в первом бите указывает на то, что это хэдер
	int header = 1;

	// установка флага занят/свободен
	shiftLeft(&header, 1);
	header += busy;

	// установка сдвига от начала дампа "локального порядкого адрес блока"
	shiftLeft(&header, 10);
	header += ((int)address - (int)globalMem) / 4;

	// установка размера блока (size/4, где 4 - размер DWORD)
	shiftLeft(&header, 10);
	int free = (size % 4 == 0) ? 0 : 4 - size % 4;
	header += size / 4 + (free == 0 ? 0 : 1);

	// установка свободных байт в этом блоке
	shiftLeft(&header, 3);
	header += free;
	
	// вывод информации о блоке
	printBlockInfo(
		size,
		header,
		address - 1,
		address
	);

	return header;
}
