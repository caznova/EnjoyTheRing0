#include "ProcessesUtils.h"

PVOID  RegistrationHandle = NULL;
HANDLE ProtectedProcessId = 0;

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(IN PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	if (OperationInformation->ObjectType != *PsProcessType) return OB_PREOP_SUCCESS;

	HANDLE CurrentProcessId = PsGetProcessId((PEPROCESS)OperationInformation->Object);

	if (CurrentProcessId != ProtectedProcessId) return OB_PREOP_SUCCESS;

	OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
	OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
	OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
	OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
	OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_QUERY_INFORMATION;
	OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_QUERY_LIMITED_INFORMATION;

	return OB_PREOP_SUCCESS;
}

VOID ObjectPostCallback(IN PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation) {
	return;
}

VOID SetupProtectedProcess(HANDLE ProcessId) {
	ProtectedProcessId = ProcessId;
}

NTSTATUS RegisterCallbacks() {
	HANDLES_NOTIFY_STRUCT NotifyStruct;
	NotifyStruct.ObjectType = PsProcessType;
	NotifyStruct.Operations = OB_OPERATION_HANDLE_CREATE;
	NotifyStruct.PostOperation = ObjectPostCallback;
	NotifyStruct.PreOperation  = ObjectPreCallback;
	NotifyStruct.RegistrationContext = NULL;
	return RegisterHandlesOperationsNotifier(&NotifyStruct, &RegistrationHandle);
}

VOID UnregisterCallbacks() {
	if (RegistrationHandle != NULL) UnregisterHandlesOperationsNotifier(RegistrationHandle);
}

