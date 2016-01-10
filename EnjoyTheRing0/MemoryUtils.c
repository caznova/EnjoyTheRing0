#include <wdm.h>
#include <windef.h>

PVOID GetMem(SIZE_T Bytes) {
	if (Bytes == 0) return NULL;
	PVOID Memory = ExAllocatePool(NonPagedPool, Bytes);
	RtlSecureZeroMemory(Memory, Bytes);
	return Memory;
}

LPWSTR AllocWideString(SIZE_T MaxCharactersCount, BOOL AddNullTerminator, OUT OPTIONAL SIZE_T* AllocatedCharacters) {
	if (AddNullTerminator) MaxCharactersCount += 1;
	if (AllocatedCharacters != NULL) *AllocatedCharacters = MaxCharactersCount;
	return GetMem(MaxCharactersCount * sizeof(WCHAR));
}

LPSTR AllocAnsiString(SIZE_T MaxCharactersCount, BOOL AddNullTerminator, OUT OPTIONAL SIZE_T* AllocatedCharacters) {
	if (AddNullTerminator) MaxCharactersCount += 1;
	if (AllocatedCharacters != NULL) *AllocatedCharacters = MaxCharactersCount;
	return GetMem(MaxCharactersCount * sizeof(CHAR));
}

VOID FreeMem(PVOID Pointer) {
	ExFreePool(Pointer);
}

VOID FillChar(PVOID Buffer, SIZE_T BufferSize, UCHAR Char) {
	RtlFillMemory(Buffer, BufferSize, Char);
}