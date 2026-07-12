#include <windows.h>

#define EXPORTABLE __declspec(dllexport)

typedef LONG WOW64_LOG_STATUS;

#define WOW64_LOG_SUCCESS ((WOW64_LOG_STATUS)0x00000000L)

EXPORTABLE WOW64_LOG_STATUS WINAPI Wow64LogInitialize(void)
{
    return WOW64_LOG_SUCCESS;
}

EXPORTABLE WOW64_LOG_STATUS WINAPI Wow64LogMessageArgList(
    UCHAR level,
    const char *format,
    void *args
)
{
    (void)level;
    (void)format;
    (void)args;

    return WOW64_LOG_SUCCESS;
}

EXPORTABLE WOW64_LOG_STATUS WINAPI Wow64LogSystemService(void *service_parameters)
{
    (void)service_parameters;

    return WOW64_LOG_SUCCESS;
}

EXPORTABLE WOW64_LOG_STATUS WINAPI Wow64LogTerminate(void)
{
    return WOW64_LOG_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    (void)instance;
    (void)reason;
    (void)reserved;

    /*
     * Terminate the host process as soon as the loader maps this DLL. This is
     * the whole point: satisfying WOW64's wow64log.dll dependency lets us stop
     * 32-bit process startup at the loader boundary.
     */
    ExitProcess(0);
}
