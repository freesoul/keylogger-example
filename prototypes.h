#ifndef __PROTOTYPES__
#define __PROTOTYPES__

#include <windows.h>

#pragma once
 
typedef HHOOK(WINAPI *PROTO_SetWindowsHookExA)(
	int       idHook,
	HOOKPROC  lpfn,
	HINSTANCE hmod,
	DWORD     dwThreadId
);

typedef HANDLE(WINAPI *PROTO_CreateThread)(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId
);

#endif 