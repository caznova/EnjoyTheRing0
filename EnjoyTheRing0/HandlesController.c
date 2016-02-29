#include "HandlesController.h"

ULONG HandlesCount = 0;

VOID IncHandlesCount() {
	HandlesCount++;
}

VOID DecHandlesCount() {
	if (HandlesCount > 0) HandlesCount--;
}

ULONG GetHandlesCount() {
	return HandlesCount;
}