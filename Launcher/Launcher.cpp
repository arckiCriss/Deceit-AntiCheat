#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <shellapi.h>
#include <Nspapi.h>
#pragma comment(lib, "ntdll.lib")
#include <winternl.h>

void CreateStopService(void)
{
    SC_HANDLE h_manager = NULL;
    SC_HANDLE h_service = NULL;

    h_manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    h_service = CreateServiceW(h_manager, L"Baseline", L"Baseline", SERVICE_ALL_ACCESS, SERVICE_FILE_SYSTEM_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, L"C:\\Program Files (x86)\\Baseline\\DeceitAntiCheat.sys", NULL, NULL, NULL, NULL, NULL);
    h_service = OpenService(h_manager, L"Baseline", SERVICE_ALL_ACCESS);
    StartServiceA(h_service, 0, NULL);
	while (1)
	{
		HWND sdnd = FindWindowA(0, "Deceit");
		while (!sdnd)
		{
			SERVICE_CONTROL_STATUS_REASON_PARAMSA service_control_status;
			DWORD dwerror = NULL;
			ZeroMemory(&service_control_status, sizeof(SERVICE_CONTROL_STATUS_REASON_PARAMSA));
			service_control_status.dwReason = SERVICE_STOP_REASON_FLAG_PLANNED | SERVICE_STOP_REASON_MAJOR_APPLICATION | SERVICE_STOP_REASON_MINOR_NONE;;
			ControlServiceExA(h_service, SERVICE_CONTROL_STOP, SERVICE_CONTROL_STATUS_REASON_INFO, &service_control_status);
			DeleteService(h_service);
			exit(1);
		}
		Sleep(5);
	}
    CloseServiceHandle(h_service);
    CloseServiceHandle(h_manager);
}

void FindWindow()
{
	HWND Fhwnd = FindWindowA(0, "Deceit");

	bool Wfound = false;

	while (!Fhwnd)
	{
		std::cout << "I can't Find Deceit" << std::endl;
		system("cls");
		HWND ChangeFound = FindWindowA(0, "Deceit");
		if (ChangeFound)
			break;
	}
}

void testsignDetect()
{
	SYSTEM_CODEINTEGRITY_INFORMATION SysCodeInform = { 0 };
	ULONG SysCodeInformLenght = 0;
	SysCodeInform.Length = sizeof(SysCodeInform);
	if (NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x67, &SysCodeInform, sizeof(SysCodeInform), &SysCodeInformLenght) >= 0 && SysCodeInformLenght == sizeof(SysCodeInform))
	{
		BOOL TestsignEnabled = !!(SysCodeInform.CodeIntegrityOptions & 0x2);
		if (TestsignEnabled)
		{
			//Draw error shit
			MessageBox(0, L"Deceit can't work with TestMode windows, Restart your computer!", L"", 0);

		}
		else
		{
			ShellExecute(NULL, L"open", L"steam://rungameid/466240", NULL, NULL, SW_SHOWDEFAULT);
			FindWindow();
			CreateStopService();
		}
	}
}

int main()
{
	auto myConsole = GetConsoleWindow();
	ShowWindow(myConsole, 0);
	testsignDetect();
}
