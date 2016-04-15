#include "MemoryAccessController.h"

ULONG WriteProtectionDisablesCount = 0;
ULONG SmepSmapDisablesCount = 0;

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