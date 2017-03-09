#include "plugin.h"

//Thanks to Mattiwatti https://github.com/x64dbg/x64dbg/issues/1481#issuecomment-285231185
extern "C" __declspec(dllexport) void CBLOADDLL(CBTYPE cbType, PLUG_CB_LOADDLL* info)
{
    if(strcmp(info->modname, "user32.dll") == 0)
    {
        auto user32 = GetModuleHandleW(L"user32.dll");
        auto addr_GetForegroundWindow = duint(GetProcAddress(user32, "GetForegroundWindow"));
        if(addr_GetForegroundWindow)
            addr_GetForegroundWindow = addr_GetForegroundWindow - duint(user32) + duint(info->LoadDll->lpBaseOfDll);
        auto addr_GetActiveWindow = duint(GetProcAddress(user32, "GetActiveWindow"));
        if(addr_GetActiveWindow)
            addr_GetActiveWindow = addr_GetActiveWindow - duint(user32) + duint(info->LoadDll->lpBaseOfDll);
        _plugin_logprintf("[" PLUGIN_NAME "] GetForegroundWindow: %p, GetActiveWindow: %p\n", addr_GetForegroundWindow, addr_GetActiveWindow);
        if(addr_GetForegroundWindow && addr_GetActiveWindow)
        {
            unsigned char patch[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
            auto dw = int(addr_GetActiveWindow - addr_GetForegroundWindow - 5);
            memcpy(patch + 1, &dw, sizeof(dw));
            if(DbgMemWrite(addr_GetForegroundWindow, patch, sizeof(patch)))
                _plugin_logputs("[" PLUGIN_NAME "] GetForegroundWindow patched!");
            else
                _plugin_logputs("[" PLUGIN_NAME "] DbgMemWrite failed...");
        }
        else
            _plugin_logputs("[" PLUGIN_NAME "] Address not found...");
    }
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    return true;
}

//Deinitialize your plugin data here (clearing menus optional).
bool pluginStop()
{
    return true;
}

//Do GUI/Menu related things here.
void pluginSetup()
{
}
