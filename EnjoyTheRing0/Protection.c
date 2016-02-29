#include "ProcessesUtils.h"
#include "Protection.h"

PVOID  ProtectionCallbackHandle = NULL;

VOID ReduceRights(PACCESS_MASK pAccessMask) {
	*pAccessMask &= ~PROCESS_VM_READ;
	*pAccessMask &= ~PROCESS_VM_WRITE;
	*pAccessMask &= ~PROCESS_VM_OPERATION;
	*pAccessMask &= ~PROCESS_QUERY_INFORMATION;
	*pAccessMask &= ~PROCESS_QUERY_LIMITED_INFORMATION;
	*pAccessMask &= ~PROCESS_TERMINATE;
	*pAccessMask &= ~PROCESS_CREATE_THREAD;
}

OB_PREOP_CALLBACK_STATUS PreOpenCallback(IN PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);

	if (OperationInformation->ObjectType != *PsProcessType) return OB_PREOP_SUCCESS;

	HANDLE CurrentProcessId = PsGetProcessId((PEPROCESS)OperationInformation->Object);

	if (!IsProcessProtected(CurrentProcessId)) return OB_PREOP_SUCCESS;

	ReduceRights(&OperationInformation->Parameters->CreateHandleInformation.DesiredAccess);
	ReduceRights(&OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess);

	return OB_PREOP_SUCCESS;
}

VOID PostOpenCallback(IN PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
	return;
}

NTSTATUS RegisterProtection() {
	if (ProtectionCallbackHandle != NULL) return STATUS_SUCCESS;

	HANDLES_NOTIFY_STRUCT NotifyStruct;
	NotifyStruct.ObjectType = PsProcessType;
	NotifyStruct.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	NotifyStruct.PostOperation = PostOpenCallback;
	NotifyStruct.PreOperation = PreOpenCallback;
	NotifyStruct.RegistrationContext = NULL;
	return RegisterHandlesOperationsNotifier(&NotifyStruct, &ProtectionCallbackHandle);
}

VOID UnregisterProtection() {
	if (ProtectionCallbackHandle != NULL) UnregisterHandlesOperationsNotifier(ProtectionCallbackHandle);
	ProtectionCallbackHandle = NULL;
}



// Односвязный список защищаемых процессов:
typedef struct _PROTECTED_PROCESS {
	HANDLE ParentProcessID; // ID процесса, установившего защиту
	HANDLE ProcessID;       // Защищаемый ID
	PVOID  NextEntry;       // Адрес следующего элемента
} PROTECTED_PROCESS, *PPROTECTED_PROCESS;

// Указатель на первый элемент в списке (если NULL - список пуст):
PPROTECTED_PROCESS ProtectedProcess;

VOID __inline FillProtectionStruct(PPROTECTED_PROCESS ProtectionStruct, HANDLE ParentProcessId, HANDLE ProcessId) {
	ProtectionStruct->ParentProcessID = ParentProcessId;
	ProtectionStruct->ProcessID       = ProcessId;
	ProtectionStruct->NextEntry       = NULL;
}

VOID AddProtectedProcess(HANDLE ParentProcessId, HANDLE ProcessId) {
	PPROTECTED_PROCESS CurrentProcess = ProtectedProcess;

	// Если список ещё пуст:
	if (ProtectedProcess == NULL) {
		CurrentProcess = GetMem(sizeof(PROTECTED_PROCESS));
		FillProtectionStruct(CurrentProcess, ParentProcessId, ProcessId);
		ProtectedProcess = CurrentProcess;
		return;
	}

	// Ищем последний элемент в списке:
	while (CurrentProcess->NextEntry != NULL) {
		if (CurrentProcess->ProcessID == ProcessId) return;
		CurrentProcess = CurrentProcess->NextEntry;
	}

	if (CurrentProcess->ProcessID == ProcessId) return;

	CurrentProcess->NextEntry = GetMem(sizeof(PROTECTED_PROCESS));
	FillProtectionStruct(CurrentProcess->NextEntry, ParentProcessId, ProcessId);
}

VOID RemoveProtectedProcess(HANDLE ProcessId) {
	if (ProtectedProcess == NULL) return;

	PPROTECTED_PROCESS CurrentProcess = ProtectedProcess;

	// Если элемент только один:
	if (CurrentProcess->NextEntry == NULL) {
		if (CurrentProcess->ProcessID == ProcessId) {
			FreeMem(CurrentProcess);
			ProtectedProcess = NULL;
		}
		return;
	}

	// Если элементов несколько, но первый соответствует нужному процессу:
	if (CurrentProcess->ProcessID == ProcessId) {
		ProtectedProcess = CurrentProcess->NextEntry;
		FreeMem(CurrentProcess);
		return;
	}

	PPROTECTED_PROCESS NextProcess = CurrentProcess->NextEntry;

	// Выходим из цикла или когда следующий процесс - нужный, или когда мы на последнем в списке процессе:
	while (NextProcess->ProcessID != ProcessId) {
		CurrentProcess = NextProcess;
		NextProcess = CurrentProcess->NextEntry;

		// Если следующий элемент пустой - мы дошли до конца, но не нашли нужного:
		if (NextProcess == NULL) return;
	}

	// Удаляем элемент из списка:
	CurrentProcess->NextEntry = NextProcess->NextEntry;
	FreeMem(NextProcess);
}

VOID RemoveProcessesOfParent(HANDLE ParentProcessId) {
	if (ProtectedProcess == NULL) return;

	PPROTECTED_PROCESS CurrentProcess = ProtectedProcess;
	
	// Если первый процесс - нужный (требуется заменить адрес первого элемента):
	while (CurrentProcess->ParentProcessID == ParentProcessId) {
		ProtectedProcess = CurrentProcess->NextEntry;
		FreeMem(CurrentProcess);
		if (ProtectedProcess == NULL) return;
		CurrentProcess = ProtectedProcess;
	}

	// Проходимся по всем процессам:
	PPROTECTED_PROCESS NextProcess;
	while (CurrentProcess != NULL) {
		NextProcess = CurrentProcess->NextEntry;
		if (NextProcess == NULL) return;
		
		if (NextProcess->ParentProcessID == ParentProcessId) {
			CurrentProcess->NextEntry = NextProcess->NextEntry;
			FreeMem(NextProcess);
		}
		CurrentProcess = CurrentProcess->NextEntry;  
	}
}

BOOL IsProcessProtected(HANDLE ProcessId) {
	if (ProtectedProcess == NULL) return FALSE;

	PPROTECTED_PROCESS CurrentProcess = ProtectedProcess;
	while (CurrentProcess != NULL) {
		if (CurrentProcess->ProcessID == ProcessId) return TRUE;
		CurrentProcess = CurrentProcess->NextEntry;
	}
	return FALSE;
}

VOID ClearProtectedProcessesList() {
	if (ProtectedProcess == NULL) return;

	PPROTECTED_PROCESS CurrentProcess = ProtectedProcess;
	PPROTECTED_PROCESS NextProcess = NULL;

	while (CurrentProcess != NULL) {
		NextProcess = CurrentProcess->NextEntry;
		FreeMem(CurrentProcess);
		CurrentProcess = NextProcess;
	}

	ProtectedProcess = NULL;
}