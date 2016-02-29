#pragma once

#include <ntdef.h>
#include <windef.h>

#define SET_PROTECTION    CTL_CODE(0x8000, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)
#define RESET_PROTECTION  CTL_CODE(0x8000, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)
#define RESET_ALL_BY_PID  CTL_CODE(0x8000, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)
#define RESET_ALL         CTL_CODE(0x8000, 0x803, METHOD_NEITHER, FILE_ANY_ACCESS)
#define BEACON            CTL_CODE(0x8000, 0x804, METHOD_NEITHER, FILE_ANY_ACCESS)


NTSTATUS RegisterProtection();
VOID     UnregisterProtection();

VOID AddProtectedProcess    (HANDLE ParentProcessId, HANDLE ProcessId);
VOID RemoveProtectedProcess (HANDLE ProcessId);
VOID RemoveProcessesOfParent(HANDLE ParentProcessId);

BOOL IsProcessProtected     (HANDLE ProcessId);

VOID ClearProtectedProcessesList();

