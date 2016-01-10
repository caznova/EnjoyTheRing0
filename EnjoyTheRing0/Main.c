#include <wdm.h>
#include "Main.h"
#include "FilesUtils.h"
#include "StringsUtils.h"

UNICODE_STRING DeviceName;
UNICODE_STRING DeviceLink;

// Загрузка драйвера в систему:
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) 
{
	PDEVICE_OBJECT DeviceObject;

	NTSTATUS Status = 0;

	DbgPrint("[ETR0]: Loading...");

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
		DbgPrint("[ETR0]: IoCreateDevice Error!");
		return Status;
	}

	// Создаём ссылку на устройство:
	RtlInitUnicodeString(&DeviceLink, DeviceLinkStr);
	Status = IoCreateSymbolicLink(&DeviceLink, &DeviceName);

	if (!NT_SUCCESS(Status)) {
		DbgPrint("[ETR0]: IoCreateSymbolicLink Error!");
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	DbgPrint("[ETR0]: Successfully loaded!");
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
		// *** Пример работы со строками, файлами и папками ***

		LPWSTR DirectoryPath    = L"C:\\[ETR0] Directory\\";
		LPWSTR OriginalFileName = L"KernelFile.txt";
		
		// Формируем полный путь к файлу:
		LPWSTR OriginalFilePath;
		ConcatenateStringsW(DirectoryPath, OriginalFileName, &OriginalFilePath);

		// Создаём рабочую папку:
		CreateDirectory(DirectoryPath);

		// Создаём файл и записываем в него строку:
		HANDLE hFile;
		LPSTR  Text = "Hello, Kernel-World!";
		CreateEmptyFile(&hFile, OriginalFilePath);
		WriteFile(hFile, Text, (ULONG)LengthA(Text), NULL, NULL);
		CloseFile(hFile);
			
		// Освобождаем память, занятую под путь к файлу:
		FreeString(OriginalFilePath);



		// *** Обработка IOCTL ***

		// Обрабатываем нужный IOCTL, если требуется:
		switch (ControlCode) {
			// ...
		}

	} __except (EXCEPTION_EXECUTE_HANDLER) {
		DbgPrint("[ETR0]: Exception catched!");
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
	DbgPrint("[ETR0]: DriverCreate event!");
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = 0;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Событие очистки ресурсов драйвера:
NTSTATUS DriverCleanup(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket)
{
	DbgPrint("[ETR0]: DriverCleanup event!");
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = 0;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Событие закрытия драйвера (закрытия устройства через CloseHandle):
NTSTATUS DriverClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket)
{
	DbgPrint("[ETR0]: DriverClose event!");
	IORequestPacket->IoStatus.Status = STATUS_SUCCESS;
	IORequestPacket->IoStatus.Information = 0;
	IoCompleteRequest(IORequestPacket, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// Выгрузка драйвера:
VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("[ETR0]: Unloading...");

	IoDeleteSymbolicLink(&DeviceLink);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("[ETR0]: Successfully unloaded!");
	return;
}