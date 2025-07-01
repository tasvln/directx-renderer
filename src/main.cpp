#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    MessageBox(nullptr,
               L"DirectX App Initialized!",
               L"Welcome",
               MB_OK | MB_ICONINFORMATION);
    return 0;
}
