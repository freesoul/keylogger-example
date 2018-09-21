#include <windows.h>

bool test_being_scanned() {
	
	// AV usually avoids sleep
	int Tick = GetTickCount();
	Sleep(1000);
	int Tac = GetTickCount();
	if ((Tac - Tick) < 1000) 
		return true;
	
	// AV usually do not dedicate more than 1 core
	SYSTEM_INFO SysGuide;
	GetSystemInfo(&SysGuide);
	int CoreNum = SysGuide.dwNumberOfProcessors;
	if (CoreNum < 2) 
	  return true;

	// This dll should not exist
	HINSTANCE DLL = LoadLibrary(TEXT("this_dll_does_not_exist.dll"));
	if (DLL != NULL)
		return true;
	
	// Detect sandbox
	if(FindWindow(NULL, "main.exe")) 
		return true;
	
	// Prevent multiple instances
	CreateMutexA(0, FALSE, "Local\\$myprogram$"); 
    if(GetLastError() == ERROR_ALREADY_EXISTS) 
        return true; 

	
	return false;
}