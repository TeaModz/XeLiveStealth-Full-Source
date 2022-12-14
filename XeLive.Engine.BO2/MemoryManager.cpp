#include "MemoryManager.h"

MemoryManager::MemoryManager() { }

MemoryManager::MemoryManager(DWORD Buffer, DWORD Size) { SetupManager(Buffer, Size); }

MemoryManager::~MemoryManager() { }

VOID MemoryManager::SetupManager(DWORD Buffer, DWORD Size) {
	memory = Buffer;
	memorySize = Size;
}

BOOL MemoryManager::alloc(DWORD Size, DWORD *Out, BOOL zeroMemory) {
	for (DWORD i = 0; i < memorySize - Size; i++) {
		BOOL validMemoryRegion = TRUE;
		for (DWORD a = 0; a < allocated.size(); a++) {
			for (DWORD s = 0; s < Size; s++) {
				if (memory + i + s >= allocated[a].address && memory + i + s < allocated[a].address + allocated[a].size) {
					validMemoryRegion = FALSE;
					i += (allocated[a].size + s - 1);
					break;
				}
			}
		}
		if (validMemoryRegion) {
			if(zeroMemory)
				memset((PVOID)(memory + i), 0, Size);
			allocated.push_back(MemoryRegion(memory + i, Size));
			*Out = memory + i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL MemoryManager::alloc(DWORD Size, PVOID Out, BOOL zeroMemory) { return alloc(Size, (DWORD*)Out); }

BOOL MemoryManager::free(DWORD Address, BOOL clearMemory) {
	for (DWORD i = 0; i < allocated.size(); i++) {
		if (allocated[i].address == Address) {
			if(clearMemory)
				memset((PVOID)allocated[i].address, 0, allocated[i].size);
			std::vector<MemoryRegion>::iterator it = allocated.begin();
			std::advance(it, i);
			allocated.erase(it);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL MemoryManager::free(PVOID Address, BOOL clearMemory) { return free((DWORD)Address); }