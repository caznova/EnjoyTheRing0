#pragma once

#include <ntdef.h>
#include <windef.h>

// I/O:

VOID __fastcall StartBeeper();
VOID __fastcall StopBeeper();
VOID __fastcall SetBeeperRegime();
VOID __fastcall SetBeeperOut();
VOID __fastcall SetBeeperIn();
VOID __fastcall SetBeeperDivider(WORD Divider);
VOID __fastcall SetBeeperFrequency(WORD Frequency);

VOID __fastcall WriteIoPortByte (WORD PortNumber, BYTE  Data);
VOID __fastcall WriteIoPortWord (WORD PortNumber, WORD  Data);
VOID __fastcall WriteIoPortDword(WORD PortNumber, DWORD Data);

BYTE  __fastcall ReadIoPortByte (WORD PortNumber);
WORD  __fastcall ReadIoPortWord (WORD PortNumber);
DWORD __fastcall ReadIoPortDword(WORD PortNumber);

// Interrupts:

typedef struct _REGISTERS_STATE {
#ifdef _AMD64_
	DWORD64 RAX;
	DWORD64 RCX;
	DWORD64 RDX;
#else
	DWORD32 EAX;
	DWORD32 ECX;
	DWORD32 EDX;
#endif
} REGISTERS_STATE, *PREGISTERS_STATE;

VOID __fastcall _CLI();
VOID __fastcall _STI();
VOID __fastcall _HLT();
VOID __fastcall _INT(BYTE InterruptNumber, PREGISTERS_STATE RegistersState);

// MSR:

ULONGLONG __fastcall _RDPMC(ULONG Index);
ULONGLONG __fastcall _RDMSR(ULONG Index);
VOID      __fastcall _WRMSR(ULONG Index, PULONGLONG Value);

// SystemRegisters:

/*
  CR0 - содержимое контрольных битов
  CR2, CR3 - для страничной трансляции
  CR4 - биты, определяющие системные возможности
  CR8 - изменяет приоритет внешних прерываний
  Остальные CR не используются, при попытке обращения будет сгенерировано #UD (Undefined Opcode)

  DR0..DR3 - линейные адреса брейкпоинтов
  DR4, DR5 - связаны с DR6 и DR7, если CR4.DE = 0
  DR6 - статусный отладочный регистр
  DR7 - контрольный отладочный регистр
  Остальные DR зарезервированы, при обращении будет сгенерировано #UD
*/

#define READ_CR  (BYTE)0x20
#define WRITE_CR (BYTE)0x22
#define READ_DR  (BYTE)0x21
#define WRITE_DR (BYTE)0x23 
#define SYS_REG_OPERATION(Operation, RegisterNumber) ((WORD)(((BYTE)Operation << 8) | (BYTE)RegisterNumber))

#define READ_CR0 (WORD)SYS_REG_OPERATION(READ_CR, 0)
#define READ_CR2 (WORD)SYS_REG_OPERATION(READ_CR, 2)
#define READ_CR3 (WORD)SYS_REG_OPERATION(READ_CR, 3)
#define READ_CR4 (WORD)SYS_REG_OPERATION(READ_CR, 4)
#define READ_CR8 (WORD)SYS_REG_OPERATION(READ_CR, 8)

#define WRITE_CR0 (WORD)SYS_REG_OPERATION(WRITE_CR, 0)
#define WRITE_CR2 (WORD)SYS_REG_OPERATION(WRITE_CR, 2)
#define WRITE_CR3 (WORD)SYS_REG_OPERATION(WRITE_CR, 3)
#define WRITE_CR4 (WORD)SYS_REG_OPERATION(WRITE_CR, 4)
#define WRITE_CR8 (WORD)SYS_REG_OPERATION(WRITE_CR, 8)

#define READ_DR0 (WORD)SYS_REG_OPERATION(READ_DR, 0)
#define READ_DR1 (WORD)SYS_REG_OPERATION(READ_DR, 1)
#define READ_DR2 (WORD)SYS_REG_OPERATION(READ_DR, 2)
#define READ_DR3 (WORD)SYS_REG_OPERATION(READ_DR, 3)
#define READ_DR6 (WORD)SYS_REG_OPERATION(READ_DR, 6)
#define READ_DR7 (WORD)SYS_REG_OPERATION(READ_DR, 7)

#define WRITE_DR0 (WORD)SYS_REG_OPERATION(WRITE_DR, 0)
#define WRITE_DR1 (WORD)SYS_REG_OPERATION(WRITE_DR, 1)
#define WRITE_DR2 (WORD)SYS_REG_OPERATION(WRITE_DR, 2)
#define WRITE_DR3 (WORD)SYS_REG_OPERATION(WRITE_DR, 3)
#define WRITE_DR6 (WORD)SYS_REG_OPERATION(WRITE_DR, 6)
#define WRITE_DR7 (WORD)SYS_REG_OPERATION(WRITE_DR, 7)

VOID    __fastcall DisableWriteProtection();
VOID    __fastcall EnableWriteProtection();
BOOLEAN __fastcall IsSMEPPresent();
BOOLEAN __fastcall IsSMAPPresent();
VOID    __fastcall DisableSMEP();
VOID    __fastcall DisableSMAP();
VOID    __fastcall EnableSMEP();
VOID    __fastcall EnableSMAP();
SIZE_T  __fastcall OperateCrDrRegister(WORD Action, OPTIONAL SIZE_T OptionalData);

typedef struct _GDTR {
	WORD	Limit;
	DWORD64 Base;
} GDTR, *PGDTR;

typedef struct _IDTR {
	WORD	Limit;
	DWORD64 Base;
} IDTR, *PIDTR;

typedef struct _TR {
	WORD TSSDescriptorSegmentSelector;
} TR, *PTR;

#define SIDT 0x900A010F
#define SGDT 0x9002010F
#define STR  0x900A000F

#define LIDT 0x901A010F
#define LGDT 0x9012010F
#define LTR  0x901A000F

VOID __fastcall IdtGdtTrOperation(DWORD32 Operation, PVOID Data);