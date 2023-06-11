#include <assert.h>
#include <tchar.h>

#include "WeChatHookCore.h"

#define WE_CHAT_PROCESS_NAME            L"WeChat.exe"
#define WE_CHAT_VERSION_STRING          L"3.9.5.81"

// Offset macro
#define USER_NAME_OFFSET                0x03ACB6D8
#define PHONE_NUMBER_OFFSET             0x03ACB6F8
#define NICK_NAME_OFFSET                0x03ACB7B8

WeChatHookCore::WeChatHookCore()
{
    GetProcessId();
    GetProcessHandle();
    GetWeChatWinModuleBase();
}

WeChatHookCore::~WeChatHookCore()
{
    CloseHandle(m_handle);
}

void WeChatHookCore::GetProcessId(void)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        assert(0);
    }

    PROCESSENTRY32 processEntry;
    ZeroMemory(&processEntry, sizeof(PROCESSENTRY32));
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &processEntry))
    {
        assert(0);
    }

    do
    {
        if (lstrcmpi(processEntry.szExeFile, WE_CHAT_PROCESS_NAME) == 0)
        {
            m_processId = processEntry.th32ProcessID;
            CloseHandle(hSnapshot);

            return;
        }
    } while (Process32Next(hSnapshot, &processEntry));

    assert(0);
}

void WeChatHookCore::GetProcessHandle(void)
{
    m_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processId);
    assert(m_handle);
}

void WeChatHookCore::GetWeChatWinModuleBase(void)
{
    HANDLE handle = NULL;

    handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_processId);
    if (!handle)
    {
        assert(0);
    }

    MODULEENTRY32 moduleEntry;
    ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(handle, &moduleEntry))
    {
        CloseHandle(handle);
        assert(0);
    }

    do
    {
        if (_tcscmp(moduleEntry.szModule, L"WeChatWin.dll") == 0)
        {
            m_weChatWinModuleBase = (uint64_t)moduleEntry.hModule;
            break;
        }
    } while (Module32Next(handle, &moduleEntry));

    CloseHandle(handle);

    assert(m_weChatWinModuleBase);
}

bool WeChatHookCore::GetPhoneNumber(std::vector<int>& phone)
{
    char phoneNumber[12];
    memset(phoneNumber, 0, sizeof(phoneNumber));

    bool ret = ReadProcessMemory(m_handle, (PBYTE*)(m_weChatWinModuleBase + PHONE_NUMBER_OFFSET), &phoneNumber, sizeof(phoneNumber) - 1, 0);

    for (int i = 0; i < 11; i++)
    {
        phone.push_back(phoneNumber[i] - '0');
    }

    return ret;
}
