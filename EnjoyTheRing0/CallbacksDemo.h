#pragma once

#include <ntdef.h>

VOID     SetupProtectedProcess(HANDLE ProcessId); // Вызывать в DriverEntry
NTSTATUS RegisterCallbacks();                     // ... в DriverControl
VOID     UnregisterCallbacks();                   // ... в DriverUnload