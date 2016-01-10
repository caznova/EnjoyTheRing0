#include <wdm.h>
#include <ntstrsafe.h>
#include <windef.h>
#include "MemoryUtils.h"
#include "StringsUtils.h"

VOID __inline ValidateMaxBufferSize(SIZE_T* pMaxBufferSize) {
	if (*pMaxBufferSize > NTSTRSAFE_MAX_CCH) *pMaxBufferSize = NTSTRSAFE_MAX_CCH;
}

BOOL SafeStrCatA(LPSTR Dest, SIZE_T DestMaxCharacters, LPSTR ConcatenateWith) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	NTSTATUS Status = RtlStringCchCatA(Dest, DestMaxCharacters, ConcatenateWith);
	return NT_SUCCESS(Status);
}

BOOL SafeStrCatW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR ConcatenateWith) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	NTSTATUS Status = RtlStringCchCatW(Dest, DestMaxCharacters, ConcatenateWith);
	return NT_SUCCESS(Status);
}

BOOL SafeStrCpyA(LPSTR Dest, SIZE_T DestMaxCharacters, LPSTR Source) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	NTSTATUS Status = RtlStringCchCopyA(Dest, DestMaxCharacters, Source);
	return NT_SUCCESS(Status);
}

BOOL SafeStrCpyW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR Source) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	NTSTATUS Status = RtlStringCchCopyW(Dest, DestMaxCharacters, Source);
	return NT_SUCCESS(Status);
}

BOOL SafeStrLenA(LPSTR Str, SIZE_T DestMaxCharacters, PSIZE_T Length) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	NTSTATUS Status = RtlStringCchLengthA(Str, DestMaxCharacters, Length);
	return NT_SUCCESS(Status);
}

BOOL SafeStrLenW(LPWSTR Str, SIZE_T DestMaxCharacters, PSIZE_T Length) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	NTSTATUS Status = RtlStringCchLengthW(Str, DestMaxCharacters, Length);
	return NT_SUCCESS(Status);
}

SIZE_T LengthA(LPSTR Str) {
	SIZE_T Length = 0;
	SafeStrLenA(Str, NTSTRSAFE_MAX_CCH, &Length);
	return Length;
}

SIZE_T LengthW(LPWSTR Str) {
	SIZE_T Length = 0;
	SafeStrLenW(Str, NTSTRSAFE_MAX_CCH, &Length);
	return Length;
}

VOID WideToAnsi(LPWSTR SrcWide, LPSTR DestAnsi) {
	SIZE_T WideLength = 0;
	SafeStrLenW(SrcWide, NTSTRSAFE_MAX_CCH, &WideLength);

	if (WideLength == 0) return;

	for (int i = 0; i < WideLength; i++) {
		*((PBYTE)DestAnsi + i) = *((PBYTE)(SrcWide) + (i * 2));
	}
	*((PBYTE)DestAnsi + WideLength) = 0; // Нуль-терминатор
}

VOID DbgPrintW(LPWSTR DebugString) {
	SIZE_T WideLength = 0;
	SafeStrLenW(DebugString, NTSTRSAFE_MAX_CCH, &WideLength);
	LPSTR AnsiString = AllocAnsiString(WideLength, TRUE, NULL);
	WideToAnsi(DebugString, AnsiString);
	DbgPrint(AnsiString);
	FreeMem(AnsiString);
}

SIZE_T ConcatenateStringsA(LPSTR SrcString, LPSTR ConcatenateWith, OUT LPSTR* ResultString) {
	SIZE_T SrcStringLength = LengthA(SrcString);
	SIZE_T ConcatenateWithLength = LengthA(ConcatenateWith);
	SIZE_T ResultLength = SrcStringLength + ConcatenateWithLength;
	*ResultString = AllocAnsiString(ResultLength, TRUE, &ResultLength);
	SafeStrCpyA(*ResultString, ResultLength, SrcString);
	SafeStrCatA(*ResultString, ResultLength, ConcatenateWith);
	return ResultLength - 1; // Вычитаем нуль-терминатор
}

SIZE_T ConcatenateStringsW(LPWSTR SrcString, LPWSTR ConcatenateWith, OUT LPWSTR* ResultString) {
	SIZE_T SrcStringLength = LengthW(SrcString);
	SIZE_T ConcatenateWithLength = LengthW(ConcatenateWith);
	SIZE_T ResultLength = SrcStringLength + ConcatenateWithLength;
	*ResultString = AllocWideString(ResultLength, TRUE, &ResultLength);
	SafeStrCpyW(*ResultString, ResultLength, SrcString);
	SafeStrCatW(*ResultString, ResultLength, ConcatenateWith);
	return ResultLength - 1; // Вычитаем нуль-терминатор
}

VOID FreeString(PVOID String) {
	FreeMem(String);
}