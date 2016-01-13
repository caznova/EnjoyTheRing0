#pragma once

#include <ntddk.h>
#include <windef.h>

// Выделение и освобождение виртуальной NonPaged-памяти из пула:
PVOID GetMem(SIZE_T Bytes);
VOID  FreeMem(PVOID Pointer);

// Выделение памяти под ANSI- и Wide-строки:
LPWSTR AllocWideString(SIZE_T MaxCharactersCount, BOOL AddNullTerminator, OUT OPTIONAL SIZE_T* AllocatedCharacters);
LPSTR  AllocAnsiString(SIZE_T MaxCharactersCount, BOOL AddNullTerminator, OUT OPTIONAL SIZE_T* AllocatedCharacters);

// Заполнение буфера символом:
VOID FillChar(PVOID Buffer, SIZE_T BufferSize, UCHAR Char);

// Выделение непрерывных участков физической памяти:
PVOID AllocPhysicalMemory(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes);
PVOID AllocCacheablePhysicalMemory(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, MEMORY_CACHING_TYPE CachingType);
VOID  FreePhysicalMemory(PVOID BaseAddress);

/* Работа с физической памятью:
     GetPhysicalAddress           - получить физический адрес виртуальной памяти в контексте текущего процесса
	 MapPhysicalMemory            - отобразить физическую память на виртуальную с настройкой кэширования
	 MapPhysicalMemoryWithProtect - отобразить физическую память на виртуальную с настройкой прав доступа
	 UnmapPhysicalMemory          - размапить физическую память
*/
PHYSICAL_ADDRESS GetPhysicalAddress          (PVOID BaseVirtualAddress);
PVOID            MapPhysicalMemory           (PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, MEMORY_CACHING_TYPE CachingType);
PVOID            MapPhysicalMemoryWithProtect(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, ULONG Protect);
VOID             UnmapPhysicalMemory         (PVOID BaseVirtualAddress, SIZE_T NumberOfBytes);

// "Защитить" память от урезания прав доступа или освобождения (ProbeMode - самый "ограничительный" приемлемый режим):
VOID SecureVirtualMemory(PVOID VirtualAddress, SIZE_T NumberOfBytes, ULONG ProbeMode, OUT PHANDLE SecureHandle);
VOID UnsecureVirtualMemory(HANDLE SecureHandle);

// Проверка на валидность адреса:
BOOLEAN IsAddressValid(PVOID VirtualAddress);

// Проверка на возможность чтения\записи юзермодной памяти и "выровненности" адреса:
BOOL IsUsermodeMemoryReadable (PVOID Address, SIZE_T NumberOfBytes, ULONG RequiredAlignment);
BOOL IsUsermodeMemoryWriteable(PVOID Address, SIZE_T NumberOfBytes, ULONG RequiredAlignment);

NTSTATUS CopyMemory(PVOID Destination, MM_COPY_ADDRESS Source, SIZE_T NumberOfBytes, ULONG Flags, OUT PSIZE_T BytesCopied);