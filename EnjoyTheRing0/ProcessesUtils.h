#pragma once

#include <ntifs.h>
#include <windef.h>

PVOID GetKernelProcAddress(LPWSTR ProcedureName);

PETHREAD  GetThreadObjectByThreadId(HANDLE ThreadID);
PEPROCESS GetProcessObjectByProcessId(HANDLE ProcessId);

PEPROCESS GetProcessObjectbyThreadObject(PETHREAD Thread);

HANDLE    GetCurrentProcessId();
HANDLE    GetCurrentProcessHandle();
PEPROCESS GetCurrentProcessObject();

HANDLE   GetCurrentThreadId();
HANDLE   GetCurrentThreadHandle();
PETHREAD GetCurrentThreadObject();

BOOL SwitchToSpecifiedProcessAddressSpace(HANDLE ProcessId, OUT PRKAPC_STATE ApcState);
BOOL DetachFromSpecifiedProcessAddressSpace(IN PRKAPC_STATE ApcState);

NTSTATUS OpenProcess(HANDLE ProcessId, PHANDLE hProcess);
NTSTATUS TerminateProcess(HANDLE hProcess, NTSTATUS ExitStatus);

NTSTATUS CreateSystemThread(OUT PHANDLE hThread, PVOID ThreadProc, PVOID Arguments);
NTSTATUS ExitSystemThread(NTSTATUS ExitStatus);

NTSTATUS VirtualAlloc(HANDLE hProcess, SIZE_T Size, OUT PVOID *VirtualAddress);
NTSTATUS VirtualFree(HANDLE hProcess, PVOID VirtualAddress);

NTSTATUS CloseProcess(HANDLE hProcess);
NTSTATUS CloseThread(HANDLE hThread);
VOID     DereferenceObject(PVOID Object);
