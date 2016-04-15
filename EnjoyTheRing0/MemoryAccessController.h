#pragma once

#include "IpiWrapper.h"
#include "NativeFunctions.h"

VOID GlobalDisableWriteProtection();
VOID GlobalEnableWriteProtection();
VOID GlobalDisableSmepSmap();
VOID GlobalEnableSmepSmap();
