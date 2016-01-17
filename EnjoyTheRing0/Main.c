// KernelAPI by HoShiMin - include 'em all!
#include "MemoryUtils.h"
#include "StringsUtils.h"
#include "FilesUtils.h"
#include "ProcessesUtils.h"
#include "RegistryUtils.h"

#include "Main.h"

UNICODE_STRING DeviceName;
UNICODE_STRING DeviceLink;

// Загрузка драйвера в систему:
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) 
{
	PDEVICE_OBJECT DeviceObject;

	NTSTATUS Status = 0;
	
	DbgPrint("[ETR0]: Loading...\r\n");

	// Назначаем события:
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE]  = DriverCreate;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DriverCleanup;
	DriverObject->MajorFunction[IRP_MJ_CLOSE]   = DriverClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverControl;

	// Создаём устройство, ассоциирующееся с драйвером:
	RtlInitUnicodeString(&DeviceName, DeviceNameStr);
	Status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

	if (!NT_SUCCESS(Status)) {
		DbgPrint("[ETR0]: IoCreateDevice Error!\r\n");
		return Status;
	}

	// Создаём ссылку на устройство:
	RtlInitUnicodeString(&DeviceLink, DeviceLinkStr);
	Status = IoCreateSymbolicLink(&DeviceLink, &DeviceName);

	if (!NT_SUCCESS(Status)) {
		DbgPrint("[ETR0]: IoCreateSymbolicLink Error!\r\n");
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	DbgPrint("[ETR0]: Successfully loaded!\r\n");
	return STATUS_SUCCESS;
}


// Событие обработки IOCTL:
NTSTATUS DriverControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket)
{
	// Получаем указатель на стек запросов и код (IOCTL) полученного запроса:
	PIO_STACK_LOCATION IRPStack = IoGetCurrentIrpStackLocation(IORequestPacket);
	ULONG ControlCode = IRPStack->Parameters.DeviceIoControl.IoControlCode;

	// Получаем адреса и размеры входного и выходного буферов:
	ULONG InputBufferSize  = IRPStack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG OutputBufferSize = IRPStack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID InputBuffer  = IRPStack->Parameters.DeviceIoControl.Type3InputBuffer;
	PVOID OutputBuffer = IORequestPacket->UserBuffer;

	DbgPrint(
		"[ETR0] IRP Info:\r\n"
		" - IOCTL : 0x%X\r\n"
		" - IN    : 0x%X in %d bytes\r\n"
		" - OUT   : 0x%X in %d bytes\r\n",
		ControlCode,
		InputBuffer, InputBufferSize,
		OutputBuffer, OutputBufferSize
	);

	// Определяем возвращаемое количество байт:
	ULONG ResponseLength = 0;

	// Обрабатываем IRP:
	__try {

		HANDLE hKey;
		NTSTATUS Status = CreateKey(HKEY_CURRENT_USER, L"Software\\EnjoyTheRing0\\", &hKey);

		if NT_SUCCESS(Status) {
			SetKeyDword(hKey, L"DWORD value", 0x11223344);
			SetKeyString(hKey, L"String value", L"PWNED!");

			LPWSTR StringValue = NULL;
			GetKeyStringWithAlloc(hKey, L"String value", &StringValue, NULL);
			DbgPrintW(StringValue);
			if (StringValue != NULL) FreeString(StringValue);
			
			CloseKey(hKey);
		}
		
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		DbgPrint("[ETR0]: Exception catched!\r\n");
	}

	// Завершение запроса:
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = ResponseLength;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

// Событие создания драйвера (открытия устройства через CreateFile):
NTSTATUS DriverCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket)
{
	DbgPrint("[ETR0]: DriverCreate event!\r\n");
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = 0;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Событие очистки ресурсов драйвера:
NTSTATUS DriverCleanup(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket)
{
	DbgPrint("[ETR0]: DriverCleanup event!\r\n");
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = 0;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Событие закрытия драйвера (закрытия устройства через CloseHandle):
NTSTATUS DriverClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket)
{
	DbgPrint("[ETR0]: DriverClose event!\r\n");
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = 0;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Выгрузка драйвера:
VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("[ETR0]: Unloading...\r\n");

	IoDeleteSymbolicLink(&DeviceLink);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("[ETR0]: Successfully unloaded!\r\n");
	return;
}