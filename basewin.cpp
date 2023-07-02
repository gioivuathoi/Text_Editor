#include "basewin.h"
#include<windows.h>

Basewindow :: Basewindow(): m_hwnd(NULL){}
template<class DERIVED_TYPE>
LRESULT CALLBACK Basewindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
 {
     DERIVED_TYPE *pThis = NULL;
        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT*)lParam;
            // CREATESTRCUT is a build-in struct of windows.h, search on google for it's content
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            // lpCreateParams : contains additional data which may be used to create the window
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)pThis);
            //store the pThis pointer in the instance data for the window.
            // Anytime you want to get the pointer back, call GetWindowLongPtr(), then cast into DERIVED_TYPE:
            // LONG_PTR ptr = GetWindowLongPtr(hwnd,GWLP_USERDATA);
            // DERIVED_TYPE *pThis = (DERIVED_TYPE*)ptr
            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
        }
        // Now we check if Pthis is not NULL, we handle the meassage:
        if(pThis)
        {
            return pThis->HandleMessage(uMsg,wParam,lParam);
        }
        // If pThis == NULL, it means there is no our window to work with, we just use the default function
        else{
            return DefWindowProc(hwnd,uMsg,wParam,lParam);
        }
 }


template<class DERIVED_TYPE>
BOOL Basewindow::Create(
    PSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
   ) 
{
   // Register a new window class:
        WNDCLASS wc = { };
        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        // Register the window class with OS
        RegisterClass(&wc);
        m_hwnd = CreateWindowEx(
            dwExStyle, ClassName(),lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,
            GetModuleHandle(NULL),
            this
        );
        return (m_hwnd ? true:false);
}
HWND Basewindow::Window()  { return m_hwnd; }

