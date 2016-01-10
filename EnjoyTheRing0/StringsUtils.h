#pragma once

#include <ntstrsafe.h>

#define MAX_CHARS NTSTRSAFE_MAX_CCH // Можно указывать в DestMaxCharacters

BOOL SafeStrCatA(LPSTR  Dest, SIZE_T DestMaxCharacters, LPSTR  ConcatenateWith);
BOOL SafeStrCatW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR ConcatenateWith);

BOOL SafeStrCpyA(LPSTR  Dest, SIZE_T DestMaxCharacters, LPSTR  Source);
BOOL SafeStrCpyW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR Source);

BOOL SafeStrLenA(LPSTR  Str, SIZE_T DestMaxCharacters, PSIZE_T Length);
BOOL SafeStrLenW(LPWSTR Str, SIZE_T DestMaxCharacters, PSIZE_T Length);

SIZE_T LengthA(LPSTR  Str);
SIZE_T LengthW(LPWSTR Str);

VOID WideToAnsi(LPWSTR SrcWide, LPSTR DestAnsi);

VOID DbgPrintW(LPWSTR DebugString);

// Выделение памяти и конкатенация строк. Память необходимо освобождать с помощью FreeString:
SIZE_T ConcatenateStringsA(LPSTR  SrcString, LPSTR  ConcatenateWith, OUT LPSTR*  ResultString);
SIZE_T ConcatenateStringsW(LPWSTR SrcString, LPWSTR ConcatenateWith, OUT LPWSTR* ResultString);

// Освобождение памяти, выделенной при ConcatenateStrings:
VOID FreeString(PVOID String);
