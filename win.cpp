#include"basewin.h"

class MainWindow : public Basewindow<MainWindow>   // the derived_type now is MainWindow
{
    public: 
         PCSTR ClassName() const { return "Sample Window Class";}
         LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM); 
};
LRESULT MainWindow :: HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
        {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd,&ps);
        FillRect(hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW+1));
        EndPaint(m_hwnd,&ps);
        }
        return 0;
    case WM_CLOSE:
        if (MessageBox(m_hwnd,"Realy Quit?","My Application",MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(m_hwnd);
        }
        return 0;
    case WM_DESTROY:
         PostQuitMessage(0);
         return 0;
    default: return DefWindowProcA(m_hwnd,uMsg,wParam,lParam);
    }
    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdline, INT nCmdShow)
{
    MainWindow win;
    if (!win.Create("Learn to program windows",WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }
    ShowWindow(win.Window(), nCmdShow);
    // Now we run the message loop:
    MSG msg = { };
    while (GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}