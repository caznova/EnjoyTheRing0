#include <ntstatus.h>
#include <ntstrsafe.h>
#include "StringsUtils.h"

VOID __forceinline ValidateMaxBufferSize(SIZE_T* pMaxBufferSize) {
	if (*pMaxBufferSize > NTSTRSAFE_MAX_CCH) *pMaxBufferSize = NTSTRSAFE_MAX_CCH;
}

NTSTATUS SafeStrCatA(LPSTR Dest, SIZE_T DestMaxCharacters, LPSTR ConcatenateWith) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	return RtlStringCchCatA(Dest, DestMaxCharacters, ConcatenateWith);
}

NTSTATUS SafeStrCatW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR ConcatenateWith) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	return RtlStringCchCatW(Dest, DestMaxCharacters, ConcatenateWith);
}

NTSTATUS SafeStrCpyA(LPSTR Dest, SIZE_T DestMaxCharacters, LPSTR Source) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	return RtlStringCchCopyA(Dest, DestMaxCharacters, Source);
}

NTSTATUS SafeStrCpyW(LPWSTR Dest, SIZE_T DestMaxCharacters, LPWSTR Source) {
	ValidateMaxBufferSize(&DestMaxCharacters);
	return RtlStringCchCopyW(Dest, DestMaxCharacters, Source);
}

NTSTATUS SafeStrLenA(LPSTR String, SIZE_T MaxCharacters, PSIZE_T Length) {
	ValidateMaxBufferSize(&MaxCharacters);
	return RtlStringCchLengthA(String, MaxCharacters, (size_t *)Length);
}

NTSTATUS SafeStrLenW(LPWSTR String, SIZE_T MaxCharacters, PSIZE_T Length) {
	ValidateMaxBufferSize(&MaxCharacters);
	return RtlStringCchLengthW(String, MaxCharacters, (size_t *)Length);
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