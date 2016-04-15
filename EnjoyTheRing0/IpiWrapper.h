#pragma once

#include "ProcessesUtils.h"

// Вызов KeIpiGenericCall там, где доступно, и эмуляция - где недоступно:
VOID __fastcall CallIpi(PVOID Function, PVOID Argument);