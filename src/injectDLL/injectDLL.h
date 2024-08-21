#pragma once
#include "windows.h"
#include "tchar.h"
#include <iostream>
#include <string>

namespace DLLInjector {

    bool Inject(char* pid, char* dll, char detail[256]);
    int SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege, char detail[256]);
    int InjectDll(DWORD dwPID, LPCTSTR szDllPath, char detail[256]);

    DWORD CharToDWORD(char* str);
    LPCTSTR CharToLPCTSTR(char* str);
    char* LPCTSTRToChar(LPCTSTR wide);
}
