#include "ProcessesUtils.h"



PVOID GetKernelProcAddress(LPWSTR ProcedureName) {
	UNICODE_STRING SystemRoutineName;
	RtlInitUnicodeString(&SystemRoutineName, ProcedureName);
	return MmGetSystemRoutineAddress(&SystemRoutineName);
}



PETHREAD GetThreadObjectByThreadId(HANDLE ThreadID) {
	PETHREAD Thread;
	return NT_SUCCESS(PsLookupThreadByThreadId(ThreadID, &Thread)) ? Thread : NULL;
}

PEPROCESS GetProcessObjectByProcessId(HANDLE ProcessId) {
	PEPROCESS Process;
	return NT_SUCCESS(PsLookupProcessByProcessId(ProcessId, &Process)) ? Process : NULL;
}

PEPROCESS GetProcessObjectbyThreadObject(PETHREAD Thread) { return IoThreadToProcess(Thread); }

HANDLE    GetCurrentProcessId()     { return PsGetCurrentProcessId(); }
HANDLE    GetCurrentProcessHandle() { return ZwCurrentProcess(); }
PEPROCESS GetCurrentProcessObject() { return PsGetCurrentProcess(); }

HANDLE   GetCurrentThreadId()     { return PsGetCurrentThreadId(); }
HANDLE   GetCurrentThreadHandle() { return ZwCurrentThread(); }
PETHREAD GetCurrentThreadObject() { return PsGetCurrentThread(); }


BOOL SwitchToSpecifiedProcessAddressSpace(HANDLE ProcessId, OUT PKAPC_STATE ApcState) {
	if (ApcState == NULL) return FALSE;
	PEPROCESS Process = GetProcessObjectByProcessId(ProcessId);
	BOOL Status = Process != NULL;
	if (Status) KeStackAttachProcess(Process, ApcState);
	DereferenceObject(Process);
	return Status;
}

BOOL DetachFromSpecifiedProcessAddressSpace(IN PKAPC_STATE ApcState) {
	if (ApcState == NULL) return FALSE;
	KeUnstackDetachProcess(ApcState);
	return TRUE;
}



NTSTATUS OpenProcess(HANDLE ProcessId, PHANDLE hProcess) {
	CLIENT_ID ClientId;
	ClientId.UniqueProcess = ProcessId;
	ClientId.UniqueThread  = 0;

	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	return ZwOpenProcess(hProcess, PROCESS_ALL_ACCESS, &ObjectAttributes, &ClientId);
}

NTSTATUS TerminateProcess(HANDLE hProcess, NTSTATUS ExitStatus) {
	return ZwTerminateProcess(hProcess, ExitStatus);
}



NTSTATUS CreateSystemThread(OUT PHANDLE hThread, PVOID ThreadProc, PVOID Arguments) {
	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	return PsCreateSystemThread(hThread, GENERIC_ALL, &ObjectAttributes, NULL, NULL, ThreadProc, Arguments);
}

NTSTATUS ExitSystemThread(NTSTATUS ExitStatus) {
	return PsTerminateSystemThread(ExitStatus);
}



NTSTATUS VirtualAlloc(HANDLE hProcess, SIZE_T Size, OUT PVOID *VirtualAddress) {
	PVOID BaseAddress = NULL;
	NTSTATUS Status = ZwAllocateVirtualMemory(hProcess, &BaseAddress, 0, &Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	*VirtualAddress = BaseAddress;
	return Status;
};

NTSTATUS VirtualFree(HANDLE hProcess, PVOID VirtualAddress) {
	SIZE_T RegionSize = 0;
	return ZwFreeVirtualMemory(hProcess, &VirtualAddress, &RegionSize, MEM_RELEASE);
}


typedef enum _PROCESS_MEMORY_OPERATION {
	MemoryRead,
	MemoryWrite,
} PROCESS_MEMORY_OPERATION;

BOOL OperateProcessMemory(HANDLE ProcessId, PVOID VirtualAddress, PVOID Buffer, SIZE_T BufferSize, PROCESS_MEMORY_OPERATION MemoryOperation) {
	KAPC_STATE ApcState;
	BOOL Status = SwitchToSpecifiedProcessAddressSpace(ProcessId, &ApcState);
	if (Status) {
		PHYSICAL_ADDRESS PhysicalAddress = GetPhysicalAddress(VirtualAddress);
		PVOID MappedMemory = MapPhysicalMemoryWithProtect(PhysicalAddress, BufferSize, PAGE_READWRITE);
		HANDLE SecureHandle;
		SecureVirtualMemory(MappedMemory, BufferSize, PAGE_READWRITE, &SecureHandle);
		switch (MemoryOperation) {
			case MemoryRead:
				RtlCopyMemory(Buffer, MappedMemory, BufferSize);
				break;
			case MemoryWrite:
				RtlCopyMemory(MappedMemory, Buffer, BufferSize);
				break;
		}
		UnsecureVirtualMemory(SecureHandle);
		UnmapPhysicalMemory(MappedMemory, BufferSize);
		DetachFromSpecifiedProcessAddressSpace(&ApcState);
	}
	return Status;
}

BOOL ReadProcessMemory(HANDLE ProcessId, PVOID VirtualAddress, PVOID Buffer, SIZE_T BufferSize) {
	return OperateProcessMemory(ProcessId, VirtualAddress, Buffer, BufferSize, MemoryRead);
}

BOOL WriteProcessMemory(HANDLE ProcessId, PVOID VirtualAddress, PVOID Buffer, SIZE_T BufferSize) {
	return OperateProcessMemory(ProcessId, VirtualAddress, Buffer, BufferSize, MemoryWrite);
}



NTSTATUS CloseProcess(HANDLE hProcess)   { return ZwClose(hProcess);    }
NTSTATUS CloseThread (HANDLE hThread)    { return ZwClose(hThread);     }
VOID     DereferenceObject(PVOID Object) { ObDereferenceObject(Object); }



NTSTATUS RegisterHandlesOperationsNotifier(IN PHANDLES_NOTIFY_STRUCT HandlesNotifyStruct, OUT PVOID *RegistrationHandle) {
	// Определяем тип операции и задаём сами каллбэки:
	OB_OPERATION_REGISTRATION OperationRegistration;
	OperationRegistration.ObjectType = HandlesNotifyStruct->ObjectType; // PsProcessType / PsThreadType / ExDesktopObjectType
	OperationRegistration.Operations = HandlesNotifyStruct->Operations; // OB_OPERATION_HANDLE_CREATE || OB_OPERATION_HANDLE_DUPLICATE 
	OperationRegistration.PostOperation = HandlesNotifyStruct->PostOperation;
	OperationRegistration.PreOperation  = HandlesNotifyStruct->PreOperation;

	// Определяем "высоту" фильтра в стеке фильтров:
	UNICODE_STRING Altitude;
	RtlInitUnicodeString(&Altitude, L"389020"); // It's a magic!

	// Заполняем структуру регистрации каллбэка:
	OB_CALLBACK_REGISTRATION CallbackRegistration;
	CallbackRegistration.Altitude                   = Altitude;
	CallbackRegistration.OperationRegistration      = &OperationRegistration;
	CallbackRegistration.OperationRegistrationCount = 1;
	CallbackRegistration.RegistrationContext        = HandlesNotifyStruct->RegistrationContext; // Параметр, который будет передан в каллбэк
	CallbackRegistration.Version                    = OB_FLT_REGISTRATION_VERSION;

	// Регистрируем каллбэк:
	return ObRegisterCallbacks(&CallbackRegistration, RegistrationHandle);
}

VOID UnregisterHandlesOperationsNotifier(PVOID RegistrationHandle) {
	ObUnRegisterCallbacks(RegistrationHandle);
}