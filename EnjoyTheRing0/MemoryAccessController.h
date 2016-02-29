#pragma once

#include "ProcessesUtils.h"
#include "NativeFunctions.h"

VOID GlobalDisableWriteProtection();
VOID GlobalEnableWriteProtection();
VOID GlobalDisableSmepSmap();
VOID GlobalEnableSmepSmap();
