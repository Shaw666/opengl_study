#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "opengl_player.h"

HWND CreateMyWindow(uint32_t width, uint32_t height)
{
    WNDCLASS window;
    memset(&window, 0, sizeof(window));
    window.lpfnWndProc = (WNDPROC)DefWindowProc;
    window.hInstance = GetModuleHandle(NULL);
    window.hCursor = LoadCursor(NULL, IDC_ARROW);
    window.lpszClassName = "yuv_player";
    if (!RegisterClass(&window))
    {
        return nullptr;
    }
    DWORD dwStyle = NULL;
    HWND wnd = CreateWindowEx(NULL, window.lpszClassName, "YUV Player", dwStyle,
                              100, 100, width, height, nullptr, nullptr,
                              GetModuleHandle(NULL), nullptr);
    ShowWindow(wnd, SW_SHOWDEFAULT);
    UpdateWindow(wnd);
    return wnd;
}

int main(void)
{
    std::string filename = "test_1920x1080420p.yuv";
    uint32_t width = 1920;
    uint32_t height = 1080;

    std::shared_ptr<OpenGLPlayer> yuv_player = std::make_shared<OpenGLPlayer>();

    HWND window = CreateMyWindow(width, height);
    yuv_player->SetWindow(window);
    uint8_t *data = new uint8_t[width * height * 3 / 2];
    std::ifstream fin(filename, std::ios::in | std::ios::binary);
    while (1)
    {
        if (fin.eof())
            fin.seekg(fin.beg);
        fin.read((char *)data, width * height * 3 / 2);
        yuv_player->RenderFrame(data, width, height);
        std::this_thread::sleep_for(std::chrono::milliseconds(66));
    }
    delete[] data;
    fin.close();
    getchar();
    std::cout << "start" << std::endl;
    getchar();
    return 0;
}