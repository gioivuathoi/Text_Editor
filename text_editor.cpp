#include<windows.h>

HMENU hMenu;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddMenus(HWND);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "Sample Window Class";
    // Register a new window class:
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    // register the window class with operating system
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        0,  //Optional window style
        CLASS_NAME,  // window class
        "Learn To Program Windows", // window text
        WS_OVERLAPPEDWINDOW, //window style
        /* WS_OVERLAPPEDWINDOW is actually several flags combined with a bitwise OR.
        Together these flags give the window a title bar, a border, a system menu,
        and Minimize and Maximize buttons.
        This set of flags is the most common style for a top-level application window. */
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, //size and position (use default)
        NULL, //parent window
        NULL, //Menu
        hInstance, // HANDLE instance
        NULL  // Additional Application
    );
    if (hwnd == NULL){
        return 0;
    }
    ShowWindow(hwnd,nCmdShow);
    MSG msg = { };
    while (GetMessage(&msg,NULL,0,0)>0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
         AddMenus(hwnd);
         break;
    case WM_PAINT:   // The window needs to be updated (Paint to update)
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd,&ps);
            // All painting occurs here, betwwen BeginPaint and EndPaint
            FillRect(hdc,&ps.rcPaint, (HBRUSH)(COLOR_WINDOW +1));
            // ps.rcPaint: gives the coordinates of the rectangle to fill
            EndPaint(hwnd,&ps);
        }
        return 0;
    case WM_COMMAND:
        {
            switch (wParam)
            {
            case 1:
                MessageBeep(MB_OK);
                break;
        }}
        break;
    case WM_CLOSE:
        if (MessageBox(hwnd,"Really Quit?","My Application",MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hwnd);
        }
        // Else user cancel, do nothing
        return 0;
    case WM_DESTROY: PostQuitMessage(0);
         return 0;
    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void AddMenus(HWND hwnd)
{
    hMenu = CreateMenu();
    AppendMenu(hMenu,MF_STRING,1,"File"); // We name this button is '1'
    SetMenu(hwnd,hMenu);  
}