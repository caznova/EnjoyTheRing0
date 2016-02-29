#pragma once

#include "MemoryUtils.h"

// Константы операций для каллбэков:
#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)

// Маска IOPL для AllowIO/DisallowIO:
#define IOPL_ACCESS_MASK 0x3000 // 12й и 13й (начиная с нуля) биты в регистре EFLAGS

// Получить адрес функции из ntoskrnl.exe/hal.dll:
PVOID GetKernelProcAddress(LPWSTR ProcedureName);

// Получение объектов процессов и потоков по их ID:
PETHREAD  GetThreadObjectByThreadId  (HANDLE ThreadID);
PEPROCESS GetProcessObjectByProcessId(HANDLE ProcessId);

// Получение объекта процесса по объекту его потока:
PEPROCESS GetProcessObjectbyThreadObject(PETHREAD Thread);

// Информация о текущем процессе:
HANDLE    GetCurrentProcessId();
HANDLE    GetCurrentProcessHandle();
PEPROCESS GetCurrentProcessObject();

// Информация о текущем потоке:
HANDLE   GetCurrentThreadId();
HANDLE   GetCurrentThreadHandle();
PETHREAD GetCurrentThreadObject();

// Переключение в адресное пространство нужного процесса и обратно:
BOOL AttachToProcess(HANDLE ProcessId, OUT PKAPC_STATE ApcState);
BOOL DetachFromProcess(IN PKAPC_STATE ApcState);

// Манипуляция контекстом потока:
NTSTATUS GetThreadContext(IN PETHREAD Thread, IN OUT PCONTEXT Context, IN KPROCESSOR_MODE PreviousMode);
NTSTATUS SetThreadContext(IN PETHREAD Thread, IN PCONTEXT Context    , IN KPROCESSOR_MODE PreviousMode);

#ifdef _AMD64_
// Манипуляция контекстом:
PKTRAP_FRAME GetTrapFrame();
VOID AllowIO();
VOID DisallowIO();
#endif

// Заморозка/разморозка процесса:
NTSTATUS SuspendProcess(IN PEPROCESS Process);
NTSTATUS ResumeProcess(IN PEPROCESS Process);

// Открытие и завершение процесса:
NTSTATUS OpenProcess(HANDLE ProcessId, OUT PHANDLE hProcess);
NTSTATUS TerminateProcess(HANDLE hProcess, NTSTATUS ExitStatus);

// Создание системных потоков (нельзя передавать юзермодные функции):
NTSTATUS CreateSystemThread(OUT PHANDLE hThread, PKSTART_ROUTINE ThreadProc, PVOID Arguments);
NTSTATUS ExitSystemThread(NTSTATUS ExitStatus);

// Выделение и освобождение виртуальной памяти в контексте процесса:
NTSTATUS VirtualAlloc(HANDLE hProcess, SIZE_T Size, OUT PVOID *VirtualAddress);
NTSTATUS VirtualFree (HANDLE hProcess, PVOID VirtualAddress);

// Получить физический адрес памяти в определённом процессе:
PHYSICAL_ADDRESS GetPhysicalAddressInProcess(HANDLE ProcessId, PVOID BaseVirtualAddress);

// Отображение виртуальной памяти:
PVOID MapVirtualMemory(
	HANDLE ProcessId,
	PVOID VirtualAddress,
	OPTIONAL PVOID MapToVirtualAddress,
	ULONG Size,
	KPROCESSOR_MODE ProcessorMode,
	ULONG Protect,
	OUT PMDL* pMdl
);
VOID UnmapVirtualMemory(PMDL Mdl, PVOID MappedMemory);

// Работа с памятью процессов:
typedef enum _MEMORY_ACCESS_TYPE {
	MdlAccess,
	MdlWithPhysicalAccess,
	DirectPhysicalAccess
} MEMORY_ACCESS_TYPE;

BOOL ReadProcessMemory (HANDLE ProcessId, PVOID VirtualAddress, PVOID Buffer, ULONG BufferSize, BOOL IsUsermodeBuffer, MEMORY_ACCESS_TYPE AccessType);
BOOL WriteProcessMemory(HANDLE ProcessId, PVOID VirtualAddress, PVOID Buffer, ULONG BufferSize, BOOL IsUsermodeBuffer, MEMORY_ACCESS_TYPE AccessType);

// Закрытие хэндлов и уменьшение счётчика ссылок на объекты процессов и потоков:
NTSTATUS CloseProcess(HANDLE hProcess);
NTSTATUS CloseThread (HANDLE hThread);
VOID     DereferenceObject(PVOID Object);

/* Регистрация каллбэков-оповещений об операциях с хэндлами процессов и потоков:
	
	Для работы с каллбэками необходимо добавить флаг линкера /INTEGRITYCHECK 
	Каллбэки устанавливать в DriverEntry!

	Шаблоны каллбэков:
		
		OB_PREOP_CALLBACK_STATUS ObjectPreCallback(IN PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
			...
			return OB_PREOP_SUCCESS;
		}

		VOID ObjectPostCallback(IN PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation) {
			...
			return;
		}
*/

typedef struct _HANDLES_NOTIFY_STRUCT {
	POBJECT_TYPE *ObjectType; // PsProcessType / PsThreadType / ExDesktopObjectType
	OB_OPERATION Operations;  // OB_OPERATION_HANDLE_CREATE || OB_OPERATION_HANDLE_DUPLICATE
	POB_PRE_OPERATION_CALLBACK  PreOperation;
	POB_POST_OPERATION_CALLBACK PostOperation;
	PVOID RegistrationContext;
} HANDLES_NOTIFY_STRUCT, *PHANDLES_NOTIFY_STRUCT;

NTSTATUS RegisterHandlesOperationsNotifier(IN PHANDLES_NOTIFY_STRUCT HandlesNotifyStruct, OUT PVOID *RegistrationHandle);
VOID UnregisterHandlesOperationsNotifier(PVOID RegistrationHandle);