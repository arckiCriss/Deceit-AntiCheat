#include <ntifs.h>

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\baseline");
UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(L"\\??\\baselinesrv");

PDEVICE_OBJECT DeviceObject = NULL;

#define PROCESS_TERMINATE         0x0001  
#define PROCESS_VM_OPERATION      0x0008  
#define PROCESS_VM_READ           0x0010  
#define PROCESS_VM_WRITE          0x0020

BOOLEAN toogle = FALSE;

UCHAR* PsGetProcessImageFileName(PEPROCESS proc);
PVOID handle;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
	LIST_ENTRY64    InLoadOrderLinks;
	LIST_ENTRY64    InMemoryOrderLinks;
	LIST_ENTRY64    InInitializationOrderLinks;
	PVOID            DllBase;
	PVOID            EntryPoint;
	ULONG            SizeOfImage;
	UNICODE_STRING    FullDllName;
	UNICODE_STRING     BaseDllName;
	ULONG            Flags;
	USHORT            LoadCount;
	USHORT            TlsIndex;
	PVOID            SectionPointer;
	ULONG            CheckSum;
	PVOID            LoadedImports;
	PVOID            EntryPointActivationContext;
	PVOID            PatchInformation;
	LIST_ENTRY64    ForwarderLinks;
	LIST_ENTRY64    ServiceTagLinks;
	LIST_ENTRY64    StaticLinks;
	PVOID            ContextInformation;
	ULONG64            OriginalBase;
	LARGE_INTEGER    LoadTime;
} LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;

VOID Unload(PDRIVER_OBJECT DriverObject)
{
	if (toogle)
		ObUnRegisterCallbacks(handle);
	DbgPrint("Deleted Callback \r\n");
	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(DeviceObject);
	DbgPrint("Driver Unload \r\n");
}

NTSTATUS ProcProtect();

OB_PREOP_CALLBACK_STATUS CallBack(PVOID regContext, POB_PRE_OPERATION_INFORMATION pOperInform);

char* GetProcName(HANDLE ProcID)
{
	NTSTATUS status;
	PEPROCESS Proc = NULL;
	status = PsLookupProcessByProcessId(ProcID, &Proc);
	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}
	ObDereferenceObject(Proc);
	return (char*)PsGetProcessImageFileName(Proc);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	DriverObject->DriverUnload = Unload;
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("creating device failed \r\n");
		return status;
	}

	status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Creating symbilic link failed \r\n");
		IoDeleteDevice(DeviceObject);
		return status;
	}

	DbgPrint("Driver load \r\n");

	PLDR_DATA_TABLE_ENTRY64 ldrDataTable;
	ldrDataTable = (PLDR_DATA_TABLE_ENTRY64)DriverObject->DriverSection;
	ldrDataTable->Flags |= 0x20;
	status = ProcProtect();

	if (NT_SUCCESS(status))
	{
		DbgPrint("Register callback function succeeded \r\n");
		toogle = TRUE;
	}
	else
		DbgPrint("registration callback was failed \r\n");
	return status;
}


NTSTATUS ProcProtect()
{
	OB_CALLBACK_REGISTRATION CallbackReg;
	OB_OPERATION_REGISTRATION OperationReg;

	memset(&CallbackReg, 0, sizeof(OperationReg));
	CallbackReg.Version = ObGetFilterVersion();
	CallbackReg.OperationRegistrationCount = 1;
	CallbackReg.RegistrationContext = NULL;
	RtlInitUnicodeString(&CallbackReg.Altitude, L"321000");
	memset(&OperationReg, 0, sizeof(OperationReg));
	OperationReg.ObjectType = PsProcessType;
	OperationReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	OperationReg.PreOperation = (POB_PRE_OPERATION_CALLBACK)(&CallBack);
	CallbackReg.OperationRegistration = &OperationReg;
	return ObRegisterCallbacks(&CallbackReg, &handle);
}

OB_PREOP_CALLBACK_STATUS CallBack(PVOID regContext, POB_PRE_OPERATION_INFORMATION pOperInform)
{
	HANDLE pID = PsGetProcessId((PEPROCESS)pOperInform->Object);
	char szProcName[16] = { 0 };
	UNREFERENCED_PARAMETER(regContext);
	strcpy(szProcName, GetProcName(pID));
	if (!_stricmp(szProcName, "Deceit.exe"))
	{
		if (pOperInform->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			if ((pOperInform->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_TERMINATE) == PROCESS_TERMINATE)
			{
				pOperInform->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
			}
			if ((pOperInform->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
			{
				pOperInform->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
			}
			if ((pOperInform->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)
			{
				pOperInform->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
			}
			if ((pOperInform->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
			{
				pOperInform->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
			}
		}
	}
	return OB_PREOP_SUCCESS;
}