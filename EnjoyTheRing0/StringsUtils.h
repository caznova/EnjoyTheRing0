#pragma once

#include <ntstrsafe.h>
#include <windef.h>

#define MAX_CHARS NTSTRSAFE_MAX_CCH // Можно указывать в DestMaxCharacters

// DestMaxCharacters и MaxCharacters - размер буфера в СИМВОЛАХ с учётом символа для нуль-терминатора

NTSTATUS SafeStrCatA(LPSTR  Dest, SIZE_T DestMaxCharacters, LPSTR  ConcatenateWith);
NTSTATUS SafeStrCatW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR ConcatenateWith);

NTSTATUS SafeStrCpyA(LPSTR  Dest, SIZE_T DestMaxCharacters, LPSTR  Source);
NTSTATUS SafeStrCpyW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR Source);

NTSTATUS SafeStrLenA(LPSTR  String, SIZE_T MaxCharacters, PSIZE_T Length);
NTSTATUS SafeStrLenW(LPWSTR String, SIZE_T MaxCharacters, PSIZE_T Length);

// Длина строки в символах без нуль-терминатора:
SIZE_T LengthA(LPSTR  Str);
SIZE_T LengthW(LPWSTR Str);

VOID WideToAnsi(LPWSTR SrcWide, OUT LPSTR DestAnsi);

VOID DbgPrintW(LPWSTR DebugString);

// Выделение памяти и конкатенация строк. Память необходимо освобождать с помощью FreeString:
SIZE_T ConcatenateStringsA(LPSTR  SrcString, LPSTR  ConcatenateWith, OUT LPSTR*  ResultString);
SIZE_T ConcatenateStringsW(LPWSTR SrcString, LPWSTR ConcatenateWith, OUT LPWSTR* ResultString);

// Освобождение памяти, выделенной при ConcatenateStrings:
VOID FreeString(PVOID String);
