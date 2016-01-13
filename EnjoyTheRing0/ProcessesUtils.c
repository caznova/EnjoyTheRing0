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


BOOL SwitchToSpecifiedProcessAddressSpace(HANDLE ProcessId, OUT PRKAPC_STATE ApcState) {
	if (ApcState == NULL) return FALSE;
	PEPROCESS Process = GetProcessObjectByProcessId(ProcessId);
	BOOL Status = Process != NULL;
	if (Status) KeStackAttachProcess(Process, ApcState);
	DereferenceObject(Process);
	return Status;
}

BOOL DetachFromSpecifiedProcessAddressSpace(IN PRKAPC_STATE ApcState) {
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



NTSTATUS CloseProcess(HANDLE hProcess)   { return ZwClose(hProcess);    }
NTSTATUS CloseThread (HANDLE hThread)    { return ZwClose(hThread);     }
VOID     DereferenceObject(PVOID Object) { ObDereferenceObject(Object); }