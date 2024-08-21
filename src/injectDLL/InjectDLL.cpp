#include "injectDLL.h"

DWORD DLLInjector::CharToDWORD(char* str) {
    DWORD dw;
    sscanf_s(str, "%ul", &dw);
    return dw;
}

LPCTSTR DLLInjector::CharToLPCTSTR(char* str) {
    int num1 = MultiByteToWideChar(0, 0, str, -1, NULL, 0);
    wchar_t* wide = new wchar_t[num1];
    MultiByteToWideChar(0, 0, str, -1, wide, num1);
    return wide;
}

char* DLLInjector::LPCTSTRToChar(LPCTSTR wide) {
    int num2 = WideCharToMultiByte(CP_OEMCP, NULL, wide, -1, NULL, 0, NULL, FALSE);
    char* pchar = new char[num2];
    WideCharToMultiByte(CP_OEMCP, NULL, wide, -1, pchar, num2, NULL, FALSE);
    return pchar;
}

bool DLLInjector::Inject(char* pid, char* dll, char detail[256]) {
    std::cout << "[DEBUG] Prepareing SetPrivilege" << std::endl;
    int result = 0;
    result = SetPrivilege(SE_DEBUG_NAME, TRUE, detail);
    std::cout << "[DEBUG] SetPrivilege status: " << result << std::endl;
    if (result != 0)
        return false;

    std::cout << "[DEBUG] Prepareing InjectDll" << std::endl;
    std::cout << "[DEBUG] PID DWORD: " << CharToDWORD(pid) << std::endl;
    std::cout << "[DEBUG] DLL LPCTSTR: " << CharToLPCTSTR(dll) << std::endl;
    result = InjectDll((DWORD)CharToDWORD(pid), (LPCTSTR)CharToLPCTSTR(dll), detail);
    if (result != 0) {
        char* temp = detail;
        sprintf_s(detail, 255, "%s\nInjectDll failed!!!", temp);
        return false;
    }
    sprintf_s(detail, 255, "InjectDll success!!!");
    return true;
}

int DLLInjector::SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege, char detail[256]) {
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &hToken))
    {
        sprintf_s(detail, 255, "OpenProcessToken error");
        std::cout << "[DEBUG] OpenProcessToken error: " << GetLastError() << std::endl;
        return 1;
    }

    if (!LookupPrivilegeValue(NULL,           // lookup privilege on local system
        lpszPrivilege,  // privilege to lookup 
        &luid))        // receives LUID of privilege
    {
        sprintf_s(detail, 255, "LookupPrivilegeValue error");
        std::cout << "[DEBUG] LookupPrivilegeValue error: " << GetLastError() << std::endl;
        return 2;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.
    if (!AdjustTokenPrivileges(hToken,
        FALSE,
        &tp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL))
    {
        sprintf_s(detail, 255, "AdjustTokenPrivileges error");
        std::cout << "[DEBUG] AdjustTokenPrivileges error: " << GetLastError() << std::endl;
        return 3;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        sprintf_s(detail, 255, "The token does not have the specified privilege.");
        std::cout << "[DEBUG] The token does not have the specified privilege." << std::endl;
        return 4;
    }

    return 0;
}

int DLLInjector::InjectDll(DWORD dwPID, LPCTSTR szDllPath, char detail[256]) {
    HANDLE hProcess = NULL, hThread = NULL;
    HMODULE hMod = NULL;
    LPVOID pRemoteBuf = NULL;
    DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
    LPTHREAD_START_ROUTINE pThreadProc;

    // #1. 使用 dwPID 获取目标进程(notepad.exe)句柄（PROCESS_ALL_ACCESS权限），然后就可以用 hProcess 控制进程.
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {        //OpenProcess调用API，借助PID获取目标权限
        sprintf_s(detail, 255, "OpenProcess(%d) failed!!!", dwPID);
        std::cout << "[DEBUG] OpenProcess(" << dwPID << ") failed!!!" << std::endl;
        return 5;
    }

    // #2. 在目标进程(notepad.exe) 内存中分配 szDllName 大小的内存，返回 pRemoteBuf 作为该缓冲区的地址.
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

    // #3. 将 myhack.dll 路径写入刚刚分配的缓冲区.
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

    // #4. 获取 LoadLibraryW() API 地址，kernel32.dll在每个进程中的加载地址相同（这个特性就是我们要利用的）.
    hMod = GetModuleHandle(L"kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");

    // #5. 在 notepad.exe 中运行线程
    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return 0;
}
