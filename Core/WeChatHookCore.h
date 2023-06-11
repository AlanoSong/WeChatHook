#pragma once

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>



class WeChatHookCore
{
public:
    WeChatHookCore();
    ~WeChatHookCore();

private:
    void GetProcessId(void);
    void GetProcessHandle(void);
    void GetWeChatWinModuleBase(void);

public:
    bool GetPhoneNumber(std::vector<int> &phone);

private:
    DWORD m_processId;
    HANDLE m_handle;
    uint64_t m_weChatWinModuleBase;
};