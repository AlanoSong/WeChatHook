#include <iostream>
#include <vector>

#include "./Core/WeChatHookCore.h"

int main()
{
    WeChatHookCore hook;

    std::vector<int> phoneNumber;
    hook.GetPhoneNumber(phoneNumber);

    return 0;
}
