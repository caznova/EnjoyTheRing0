#pragma once

#include "ProcessesUtils.h"
#include "NativeFunctions.h"
#include "MemoryAccessController.h"

typedef enum _SHELL_STATUS {
	SHELL_SUCCESS,
	SHELL_CODE_BUFFER_ERROR,
	SHELL_INPUT_BUFFER_ERROR,
	SHELL_OUTPUT_BUFFER_ERROR,
	SHELL_INVALID_CODE_ADDRESS,
	SHELL_SAVING_FPU_STATE_ERROR,
	SHELL_INVALID_RETURN_ADDRESS,
	SHELL_RUNTIME_ERROR
} SHELL_STATUS, *PSHELL_STATUS;


#define UMA_DIRECT_ACCESS       0
#define UMA_ALLOC_KERNEL_MEMORY 1
#define UMA_MAP_USERMODE_MEMORY 2

typedef BYTE USERMODE_MEMORY_ACCESS;

typedef struct _UM_MEMORY_INFO {
	PVOID64 Address;
	ULONG   Size;
	ULONG   Protect;
	USERMODE_MEMORY_ACCESS AccessMethod;
} UM_MEMORY_INFO, *PUM_MEMORY_INFO;

SHELL_STATUS ExecuteShell(
	IN PVOID EntryPoint,
	IN PUM_MEMORY_INFO CodeBlock,
	IN OPTIONAL PUM_MEMORY_INFO InputData,
	IN OPTIONAL PUM_MEMORY_INFO OutputData,
	IN OPTIONAL PSIZE_T Result
);