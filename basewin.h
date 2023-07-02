#ifndef BASEWIN_H
#define BASEWIN_H
#include<windows.h>
template<class DERIVED_TYPE>
class Basewindow;

template<class DERIVED_TYPE>
class Basewindow 
{
    public:
      Basewindow();
      static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM , LPARAM);
        
      // Now we register a window class:
      BOOL Create(PSTR, DWORD,DWORD,int , int,int ,int ,HWND ,HMENU);
      HWND Window() const;
    protected:
      virtual PCSTR ClassName() const = 0;
      virtual LRESULT HandleMessage(UINT, WPARAM , LPARAM) = 0;
      HWND m_hwnd;
};
// basewindow class is an abstract base class, from which specific window classes are derived
// Some new typekey: "P" means "Pointer", "W" means "Wide", "STR" means "string","C" "const", "L" mean "long":
// so, PWSTR : pointer of wide string = unsigned short* = wchar*_t
// LPWSTR: wchar*_t
// LPSTR: long pointer of string = char*
// PSTR: pointer of string = char*
// WCHAR: wide char = wchar_t
// LPCSTR: long pointer of const string: const char* 
// PCSTR: pointer of const string: const char*
#endif