#include "MemoryAccessController.h"

ULONG WriteProtectionDisablesCount = 0;
ULONG SmepSmapDisablesCount = 0;

// Указатель на функцию/эмулятор интерпроцессорного прерывания:
typedef VOID(__stdcall *_IpiCaller)(PVOID Function, PVOID Argument);
_IpiCaller IpiCaller = (_IpiCaller)NULL;

ULONG_PTR __stdcall GlobalDisableWPCallback(IN ULONG_PTR Argument) {
	UNREFERENCED_PARAMETER(Argument);
	DisableWriteProtection();
	return (ULONG_PTR)NULL;
}

ULONG_PTR __stdcall GlobalEnableWPCallback(IN ULONG_PTR Argument) {
	UNREFERENCED_PARAMETER(Argument);
	EnableWriteProtection();
	return (ULONG_PTR)NULL;
}

ULONG_PTR __stdcall GlobalDisableSmepSmapCallback(IN ULONG_PTR Argument) {
	UNREFERENCED_PARAMETER(Argument);
	if (IsSMEPPresent()) DisableSMEP();
	if (IsSMAPPresent()) DisableSMAP();
	return (ULONG_PTR)NULL;
}

ULONG_PTR __stdcall GlobalEnableSmepSmapCallback(IN ULONG_PTR Argument) {
	UNREFERENCED_PARAMETER(Argument);
	if (IsSMEPPresent()) EnableSMEP();
	if (IsSMAPPresent()) EnableSMAP();
	return (ULONG_PTR)NULL;
}



typedef VOID (__stdcall *_IpiProc)(IN PVOID Argument);

VOID __stdcall EmulateIpi(PVOID Function, PVOID Argument) {
	KAFFINITY FullProcessorsAffinity = (KAFFINITY)0;

	ULONG ProcessorsCount = KeNumberProcessors;
	for (ULONG i = 0; i < ProcessorsCount; i++) {
		KAFFINITY CurrentAffinity = (KAFFINITY)(1 << i);
		KeSetSystemAffinityThread(CurrentAffinity);
		
		((_IpiProc)Function)(Argument);
		FullProcessorsAffinity |= CurrentAffinity;
	}

	KeSetSystemAffinityThread(FullProcessorsAffinity);
}

VOID __fastcall CallIpi(PVOID Function, PVOID Argument) {
	if (IpiCaller) {
		IpiCaller(Function, Argument);
		return;
	}

	IpiCaller = (_IpiCaller)GetKernelProcAddress(L"KeIpiGenericCall");
	if (IpiCaller) {
		IpiCaller(Function, Argument);
		return;
	}

	IpiCaller = EmulateIpi;
	IpiCaller(Function, Argument);
}

VOID GlobalDisableWriteProtection() {
	CallIpi(&GlobalDisableWPCallback, NULL);
	WriteProtectionDisablesCount++;
}

VOID GlobalEnableWriteProtection() {
	if (WriteProtectionDisablesCount == 0) return;
	CallIpi(&GlobalEnableWPCallback, NULL);
	WriteProtectionDisablesCount--;
}

VOID GlobalDisableSmepSmap() {
	CallIpi(&GlobalDisableSmepSmapCallback, NULL);
	SmepSmapDisablesCount++;
}

VOID GlobalEnableSmepSmap() {
	if (SmepSmapDisablesCount == 0) return;
	CallIpi(&GlobalEnableSmepSmapCallback, NULL);
	SmepSmapDisablesCount--;
}