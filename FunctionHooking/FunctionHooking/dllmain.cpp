#include "pch.h"
#include <Windows.h>
#include <stdio.h>


int CustomSum(int x, int y) {
    printf("This is from my custom function!\n");
    return 1337;
}

VOID Hook(HANDLE targetFunc, HANDLE newFunc, DWORD dwLen) {
    DWORD offset = ((DWORD)newFunc - (DWORD)targetFunc - 5);
    DWORD oldProt = 0;
    BOOL check = VirtualProtect(targetFunc, dwLen, PAGE_EXECUTE_READWRITE, &oldProt);

    if (check) {
        memset(targetFunc, 0x90, dwLen);
        *(BYTE*)targetFunc = 0xE9;
        *(DWORD*)((DWORD_PTR)targetFunc + 1) = offset;
        VirtualProtect(targetFunc, dwLen, oldProt, NULL);
    }
}

VOID MainThread() {
    HMODULE targetDll = GetModuleHandleA("hookme.dll");
    HANDLE targetSum = GetProcAddress(targetDll, "Sum"); 
    DWORD bytesToWrite = 5;

    Hook(targetSum, CustomSum, bytesToWrite);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MainThread();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

