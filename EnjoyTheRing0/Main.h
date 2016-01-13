/*
	Книга по программированию драйверов: 
	 - В.П.Солдатов "Программирование драйверов Windows: http://drp.su/ru/driver_dev/
*/

#pragma once

#include <wdm.h>
#include <windef.h>

PCWSTR DeviceNameStr = L"\\Device\\EnjoyTheRing0";
PCWSTR DeviceLinkStr = L"\\??\\EnjoyTheRing0";

NTSTATUS DriverEntry  (IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
VOID     DriverUnload (IN PDRIVER_OBJECT DriverObject);
NTSTATUS DriverCreate (IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket);
NTSTATUS DriverCleanup(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket);
NTSTATUS DriverClose  (IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket);
NTSTATUS DriverControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP IORequestPacket);