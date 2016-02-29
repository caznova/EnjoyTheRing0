#include "HandlesController.h"
#include "ProcessesUtils.h"
#include "NativeFunctions.h"
#include "ShellCode.h"
#include "IOCTLDispatcher.h"

NTSTATUS __fastcall DispatchIOCTL(IN PIOCTL_INFO RequestInfo, OUT PULONG ResponseLength) {
	NTSTATUS Status = STATUS_SUCCESS;

#define INPUT(Type)  ((Type)RequestInfo->InputBuffer) 
#define OUTPUT(Type) ((Type)RequestInfo->OutputBuffer)

#define SET_RESPONSE_LENGTH(Length) if (ResponseLength != NULL) {*ResponseLength = (Length);}

	switch (RequestInfo->ControlCode) {
	
// DriverFunctions:

	case GET_HANDLES_COUNT:
		*OUTPUT(PULONG) = GetHandlesCount();
		SET_RESPONSE_LENGTH(sizeof(ULONG));
		break;

// NativeFunctions:

	case START_BEEPER:
		__outbyte(0x61, __inbyte(0x61) | 3); 
		break;
	
	case STOP_BEEPER: 
		__outbyte(0x61, __inbyte(0x61) & 252);
		break;
	
	case SET_BEEPER_REGIME: 
		__outbyte(0x43, 0xB6);
		break;
	
	case SET_BEEPER_OUT: 
		__outbyte(0x61, __inbyte(0x61) | 2);
		break;
	
	case SET_BEEPER_IN: 
		__outbyte(0x61, __inbyte(0x61) & 253);
		break;

	case SET_BEEPER_DIVIDER:
		SetBeeperDivider(*INPUT(PWORD));
		break;

	case SET_BEEPER_FREQUENCY:
		SetBeeperFrequency(*INPUT(PWORD));
		break;

	case READ_IO_PORT_BYTE:
		*OUTPUT(PBYTE) = __inbyte(*INPUT(PWORD));
		SET_RESPONSE_LENGTH(sizeof(BYTE));
		break;

	case READ_IO_PORT_WORD:
		*OUTPUT(PWORD) = __inword(*INPUT(PWORD));
		SET_RESPONSE_LENGTH(sizeof(WORD));
		break;

	case READ_IO_PORT_DWORD:
		*OUTPUT(PDWORD32) = __indword(*INPUT(PWORD));
		SET_RESPONSE_LENGTH(sizeof(DWORD));
		break;

	case WRITE_IO_PORT_BYTE:
		__outbyte(
			INPUT(PWRITE_IO_PORT_BYTE_INPUT)->PortNumber,
			INPUT(PWRITE_IO_PORT_BYTE_INPUT)->Data
		);
		break;

	case WRITE_IO_PORT_WORD:
		__outword(
			INPUT(PWRITE_IO_PORT_WORD_INPUT)->PortNumber,
			INPUT(PWRITE_IO_PORT_WORD_INPUT)->Data
		);
		break;

	case WRITE_IO_PORT_DWORD:
		__outdword(
			INPUT(PWRITE_IO_PORT_DWORD_INPUT)->PortNumber,
			INPUT(PWRITE_IO_PORT_DWORD_INPUT)->Data
		);
		break;

	case RDPMC:
		*OUTPUT(PULONGLONG) = __readpmc(*INPUT(PULONG));
		SET_RESPONSE_LENGTH(sizeof(ULONGLONG));
		break;

	case RDMSR:
		*OUTPUT(PULONGLONG) = __readmsr(*INPUT(PULONG));
		SET_RESPONSE_LENGTH(sizeof(ULONGLONG));
		break;

	case WRMSR:
		__writemsr(INPUT(PWRMSR_INPUT)->Index, INPUT(PWRMSR_INPUT)->Data);
		break;

// MemoryUtils:

	case GET_PHYSICAL_ADDRESS:
		*OUTPUT(PPHYSICAL_ADDRESS) = GetPhysicalAddressInProcess(
			(HANDLE)INPUT(PGET_PHYSICAL_ADDRESS_INPUT)->ProcessID,
			(PVOID)INPUT(PGET_PHYSICAL_ADDRESS_INPUT)->VirtualAddress
		);
		SET_RESPONSE_LENGTH(sizeof(PHYSICAL_ADDRESS));
		break;

	case READ_PHYSICAL_MEMORY:
		*OUTPUT(PBOOL) = ReadPhysicalMemory(
			INPUT(PREAD_PHYSICAL_MEMORY_INPUT)->PhysicalAddress,
			(PVOID)INPUT(PREAD_PHYSICAL_MEMORY_INPUT)->Buffer,
			INPUT(PREAD_PHYSICAL_MEMORY_INPUT)->BufferSize
		);
		SET_RESPONSE_LENGTH(sizeof(BOOL));
		break;

	case WRITE_PHYSICAL_MEMORY:
		*OUTPUT(PBOOL) = WritePhysicalMemory(
			INPUT(PWRITE_PHYSICAL_MEMORY_INPUT)->PhysicalAddress,
			(PVOID)INPUT(PWRITE_PHYSICAL_MEMORY_INPUT)->Buffer,
			INPUT(PWRITE_PHYSICAL_MEMORY_INPUT)->BufferSize
		);
		SET_RESPONSE_LENGTH(sizeof(BOOL));
		break;

	case READ_DMI_MEMORY:
		ReadDmiMemory(OUTPUT(PVOID), DMI_SIZE);
		break;

// ProcessesUtils:

	case EXECUTE_SHELL_CODE:
		*OUTPUT(PSHELL_STATUS) = ExecuteShell(
			(PVOID)INPUT(PEXECUTE_SHELL_CODE_INPUT)->EntryPoint,
			(PVOID)INPUT(PEXECUTE_SHELL_CODE_INPUT)->CodeBlock,
			(PVOID)INPUT(PEXECUTE_SHELL_CODE_INPUT)->InputData,
			(PVOID)INPUT(PEXECUTE_SHELL_CODE_INPUT)->OutputData,
			(PVOID)INPUT(PEXECUTE_SHELL_CODE_INPUT)->Result
		);
		break;

	case MAP_VIRTUAL_MEMORY:
		OUTPUT(PMAP_VIRTUAL_MEMORY_OUTPUT)->MappedMemory = MapVirtualMemory(
			(HANDLE)INPUT(PMAP_VIRTUAL_MEMORY_INPUT)->ProcessId,
			INPUT(PMAP_VIRTUAL_MEMORY_INPUT)->VirtualAddress,
			INPUT(PMAP_VIRTUAL_MEMORY_INPUT)->MapToVirtualAddress,
			INPUT(PMAP_VIRTUAL_MEMORY_INPUT)->Size,
			UserMode,
			INPUT(PMAP_VIRTUAL_MEMORY_INPUT)->Protect,
			(PMDL*)&(OUTPUT(PMAP_VIRTUAL_MEMORY_OUTPUT)->Mdl)
		);
		break;

	case UNMAP_VIRTUAL_MEMORY:
		UnmapVirtualMemory(
			INPUT(PUNMAP_VIRTUAL_MEMORY_INPUT)->Mdl,
			INPUT(PUNMAP_VIRTUAL_MEMORY_INPUT)->MappedMemory
		);
		break;

	case READ_PROCESS_MEMORY:
		*OUTPUT(PBOOL) = ReadProcessMemory(
			(HANDLE)INPUT(PREAD_PROCESS_MEMORY_INPUT)->ProcessId,
			(PVOID)INPUT(PREAD_PROCESS_MEMORY_INPUT)->VirtualAddress,
			(PVOID)INPUT(PREAD_PROCESS_MEMORY_INPUT)->Buffer,
			INPUT(PREAD_PROCESS_MEMORY_INPUT)->BytesToRead,
			TRUE,
			(MEMORY_ACCESS_TYPE)INPUT(PREAD_PROCESS_MEMORY_INPUT)->AccessType
		);
		SET_RESPONSE_LENGTH(sizeof(BOOL));
		break;

	case WRITE_PROCESS_MEMORY:
		*OUTPUT(PBOOL) = WriteProcessMemory(
			(HANDLE)INPUT(PWRITE_PROCESS_MEMORY_INPUT)->ProcessId,
			(PVOID)INPUT(PWRITE_PROCESS_MEMORY_INPUT)->VirtualAddress,
			(PVOID)INPUT(PWRITE_PROCESS_MEMORY_INPUT)->Buffer,
			INPUT(PWRITE_PROCESS_MEMORY_INPUT)->BytesToWrite,
			TRUE,
			(MEMORY_ACCESS_TYPE)INPUT(PWRITE_PROCESS_MEMORY_INPUT)->AccessType
		);
		SET_RESPONSE_LENGTH(sizeof(BOOL));
		break;

#ifdef _AMD64_
	case ALLOW_IO:
		AllowIO();
		break;

	case DISALLOW_IO:
		DisallowIO();
		break;
#endif

	default: 
		Status = STATUS_NOT_IMPLEMENTED;
		break;
	}

	return Status;
}