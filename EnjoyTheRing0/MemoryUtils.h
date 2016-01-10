#pragma once

PVOID GetMem(SIZE_T Bytes);

LPWSTR AllocWideString(SIZE_T MaxCharactersCount, BOOL AddNullTerminator, OUT OPTIONAL SIZE_T* AllocatedCharacters);
LPSTR  AllocAnsiString(SIZE_T MaxCharactersCount, BOOL AddNullTerminator, OUT OPTIONAL SIZE_T* AllocatedCharacters);

VOID FreeMem(PVOID Pointer);

VOID FillChar(PVOID Buffer, SIZE_T BufferSize, UCHAR Char);