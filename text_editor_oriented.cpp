#include<windows.h>
#include<d2d1.h>
#include<iostream>
#include<stdlib.h>
#include<string>
#include<strings.h>
#include"file_stack.h"
using namespace std;
#define VIEW_HELP 1
#define FILE_MENU_OPEN 2
#define FILE_MENU_EXIT 3
#define SAVE_FILE 4
#define UNDO 5
#define REDO 6
#define HELP 7
#define TD_TEXT 8
#pragma comment(lib, "d2d1")

template<class DERIVED_TYPE>
// basewindow class is an abstract base class, from which specific window classes are derived
// Some new typekey: "P" means "Pointer", "W" means "Wide", "STR" means "string","C" "const", "L" mean "long":
// so, PWSTR : pointer of wide string = unsigned short* = wchar*_t
// LPWSTR: wchar*_t
// LPSTR: long pointer of string = char*
// PSTR: pointer of string = char*
// WCHAR: wide char = wchar_t
// LPCSTR: long pointer of const string: const char* 
// PCSTR: pointer of const string: const char*
class basewindow
{
    
public:
    
    basewindow(): m_hwnd(NULL){}
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
    // Now we register a window class:
    BOOL Create(
        PSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
        int nWidth = 700,int nHeight = 500,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    )
    {
        // Register a new window class:
        WNDCLASS wc = { };
        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        wc.hCursor = LoadCursor(NULL,IDC_ARROW);
        // Register the window class with OS
        RegisterClass(&wc);
        m_hwnd = CreateWindowEx(
            0,ClassName(),lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,
            GetModuleHandle(NULL),
            this
        );
        return (m_hwnd ? true:false);
    }
    HWND Window() const { return m_hwnd; }
    protected:
      //virtual void AddControls() = 0;
      virtual void AddMenus() = 0;
      virtual PCSTR ClassName() const = 0;
      virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM) = 0;
      HWND m_hwnd;
};

// Now let's create a simple class derived from baseclass:
class MainWindow : public basewindow<MainWindow>   // the derived_type now is MainWindow
{
    // We decalre some parameter for out application
    //hMenu for the menu
    HMENU hMenu;
    
    HWND hEdit;  // hEdit for the edit control (where we work with documents)
    stack file_undo; // file_undo is a stack where we contain conditions(string) of our work for undo
    stack file_redo; // file_redo is a stack where we contain conditions(string) of our work for redo
    int size = 0;    // we keep track size of the document
    char filename[100]; // carry address of the file when we open it
    bool saved = false;  // we keep track of the condition of the documents: saved or not saved
    string old_text;   // we keep track of the previous document after we text something more(for undo and redo)
    string content_old_file; // When we open a file, we need to save the old content to compare for saving then
    bool redo_parameter; // if users click undo, it will be set true
    bool undo_parameter;// if users click redo, it will be set true
    bool existed_file = false;
    public: 
         void initing(); // we init 2 stacks: file_undo, file_redo
         void AddControls(); // where we create our edit control
         void file_open(); // function to open a file
         void file_save(); // function to save a file
         void undo();      // function to undo 
         void redo();      // function do redo
         void about();     // when user click About TDText button, it shows a meassage box
         void AddMenus();  // where we create our menu
         PCSTR ClassName() const { return "My Class";}
         LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam); 

};
void MainWindow::initing()
{
    init(&file_undo);
    init(&file_redo);
}
LRESULT MainWindow :: HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: // When create the window, we add menu and the edit control to the window
         AddMenus();
         AddControls();
         break;
    case WM_PAINT: // this is an procedure for every window to repaint the window after user's interaction
        {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd,&ps);
        FillRect(hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW+1));
        EndPaint(m_hwnd,&ps);
        }
        return 0;
    case WM_SIZE:  // if user resize the window, we need to resize the size of the edit control
        {
            RECT rect;
            int width = 680,height = 440;
            if(GetWindowRect(m_hwnd, &rect))
            {
                 width = rect.right - rect.left;
                 height = rect.bottom - rect.top;
            }
            SetWindowPos(hEdit,NULL,0,0,width-20,height-60,SWP_SHOWWINDOW);
            // by resize the edit control, we resize the scroll bars
        }
        return 0;
    // Every time user click a button on the menu, system will send a meassage in WM_COMMANd type, so we need
    // to switch the parameters wParam to get that meassge
    case WM_COMMAND: 
    {
        switch (wParam)
        {
        case FILE_MENU_EXIT:  //user choose 'Exit' in 'File' menu
            if (MessageBoxA(m_hwnd,"Realy Quit?","My Application",MB_YESNO| MB_ICONEXCLAMATION) == IDYES)
         {
            DestroyWindow(m_hwnd);
         }
            return 0;

        case FILE_MENU_OPEN: // user choose 'Open File' in 'File' Menu
            file_open();
            break;
        
        case SAVE_FILE:  // if user click 'Save' in 'File' menu
            {   // Fisrt we get the content of the edit control
                size = GetWindowTextLength(hEdit);
                char text[size+1];
                GetWindowTextA(hEdit,text,size+1);
                // Check if there is nothing on the screen
                if (strcmp(text,"")==0)
                {
                    int val = MessageBoxA(m_hwnd,"You did not enter anything",NULL,MB_OKCANCEL|MB_ICONEXCLAMATION);
                    switch (val)
                    {
                    case IDOK:
                        DestroyWindow(m_hwnd);
                        return 0;
                    
                    case IDCANCEL:
                        break;
                    }
                }
                // else we check if user hasn't text anything more
                else{
                    string data(text);
                    if(content_old_file == text)
                    {
                        break;
                    }
                    // else we save the file
                    file_save();
                }
            }
            break;
        
        case UNDO: // if user click undo
            undo();
            break;
        case REDO:  // if user click redo
            redo();
            break;
        case TD_TEXT: // If user choose 'About TDText" in 'Help" menu
            {
             int val = MessageBox(m_hwnd,TEXT("Desinger: Vu Xuan Thinh 20203596 & Pham Ba Do 20203362\nContact Info: caocaodua1@gmail.com"),TEXT("TDText"),MB_OK|MB_ICONQUESTION);
             if(val == IDOK) break;;
            }
             break;
        case VIEW_HELP:  // If user choose "View Help" in "Help" menu
            {   
             int val = MessageBox(m_hwnd,TEXT("Create new file: When open TDText, you are in a new file, just text and when you want to save file:' File->Save' and name your file with tail '.txt' then click 'Save'\n\n How to open file: 'File->Open File', choose file and click 'Open'\n\n Note: If you click 'Exit' in' File', it will quit TDText without saving your file, so be careful!"),TEXT("Instructor"),MB_OK|MB_ICONQUESTION);
             if(val == IDOK) break;
            }
            break;
        }
        switch (HIWORD(wParam))
        {
        case EN_UPDATE:
        // if there is no character on the screen, we set old_text = "", add "" to the stack of file_undo
        // so that after we edit, when undo we can come back to the first condition: empty
            {
            saved = false;
            if (size == 0)
            {
            push(&file_undo,"");
            old_text = "";
            }
            // Now we need to keep track of the last text, if user deletes a character we will have the previous text
            // to push to the stack file_undo
            size = GetWindowTextLength(hEdit);
            char *data = new char[size+1];
            GetWindowTextA(hEdit,data,size+1);
            data[size] = '\0';
            if (old_text.length() > size)
                {
                    string mydata(data);
                    push(&file_undo,old_text);
                }
            
            if (old_text.length() < size)
            {
                if (data[size -1] == '\n')
                {
                    push(&file_undo, string(data));
                }
                // if user text more after undo/redo, we need to push old_text into undo
                if (redo_parameter == true)
                {
                    push(&file_undo,old_text);
                    redo_parameter = false;
                }
                if (undo_parameter == true)
                {
                    push(&file_undo,old_text);
                    undo_parameter = false;
                }
                // if user texts any character, we clear the stack file_redo because we need new file_redo when user undos
                clear(&file_redo);
            }
            // update the old_text
            old_text = string(data);
            delete[] data;
            }
            break;
        }
    }
    break;
    case WM_CLOSE: // if user click exit icon (X)
        {   
            size = GetWindowTextLength(hEdit);
            char data[size+1];
            GetWindowTextA(hEdit,data,size+1);
            string mydata(data);
            if (mydata != content_old_file && saved == false) // check if users hasn't save their works
            {
                int ans = MessageBoxA(m_hwnd,"File not saved. Really Quit?","My Application",MB_YESNO| MB_ICONEXCLAMATION);
                if (ans == IDNO)
                {
                    file_save();
                }
            }
            DestroyWindow(m_hwnd);
        }
        return 0;
    case WM_DESTROY:
         PostQuitMessage(0);
         return 0;
    // just do the default procedure
    default: return DefWindowProcA(m_hwnd,uMsg,wParam,lParam);
    }
}

void MainWindow::file_open()
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME)); // Set all elements of ofn to be zero
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = filename; //this will carry position of the file user selected
    ofn.lpstrFile[0] = '\0'; //There is no default file
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "All files\0*.*\0Source\0*.CPP\0Text Files\0*.TXT\0"; // the filter to choose file with specific tail
    ofn.nFilterIndex = 1; //Meanning we choose 'All files' to be showed when we open this dialog 
    // To use this function and open the dialog, we need to link ''libcomdlg32.a'' into our project
    // In DevC++, we create project "Windows Application" (for WIN32 API), add needed file to project
    // in common, we have the link: C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32/libcomdlg32.a"
    GetOpenFileName(&ofn);
    // Display the file:
    if (ofn.lpstrFile[0] != '\0') {
    FILE *file;
    file = fopen(ofn.lpstrFile,"rb");
    existed_file = true;
    fseek(file,0,SEEK_END);
    int nsize = ftell(file);
    rewind(file);
    char *data = new char(nsize +1);
    fread(data,nsize,1,file);
    data[nsize] = '\0';
    SetWindowTextA(hEdit,data);
    size = GetWindowTextLength(hEdit);
    string str(data);
    content_old_file = str;
    push(&file_undo,str);
    old_text = str;
    undo_parameter = true;
    delete[] data;
    fclose(file);}
}
void MainWindow::file_save()
{
    OPENFILENAME ofn;
    // if we aren't in a existed file, we need to create new file:
    if (existed_file == false){
    ZeroMemory(&ofn,sizeof(OPENFILENAME)); // Set all elements of ofn to be zero
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = filename; //this will carry position all the file user selected
    ofn.lpstrFile[0] = '\0'; //There is no default file
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "All files\0*.*\0Source\0*.CPP\0Text Files\0*.TXT\0"; // the filter to choose file with specific tail
    ofn.nFilterIndex = 1; //Meanning we choose 'All files' to be showed when we open this dialog 
    // To use this function and open the dialog, we need to link ''libcomdlg32.a'' into our project
    // In DevC++, we create project "Windows Application" (for WIN32 API), add needed file to project
    // in common, we have the link: C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib32/libcomdlg32.a"
    GetSaveFileName(&ofn);}
    FILE *file;
    file = fopen(filename,"w");
    int nsize = GetWindowTextLength(hEdit);
    char *data = new char(nsize+1);
    GetWindowTextA(hEdit,data,nsize+1);
    data[nsize] = '\0';
    string mydata(data);
    fwrite(data,size+1,1,file);
    saved = true;
    delete[] data;
    fclose(file);
}
void MainWindow::AddMenus()   // when we click the button, WM_COMMAND will be sent
{
    // hMenu will be the main menu
    hMenu = CreateMenu(); // CreateMenu is a function of win32++
    // in hMenu, we will create two submenus: hFileMenu for 'File' button, 'hHelp' for 'Help' button
    HMENU hFileMenu = CreateMenu();
    HMENU hHelp = CreateMenu();
    //AppendMenuA(hFileMenu,MF_STRING,FILE_MENU_NEW,"New"); Not Need right now
    AppendMenuA(hFileMenu,MF_STRING,SAVE_FILE,"Save");
    AppendMenuA(hFileMenu,MF_STRING,FILE_MENU_OPEN,"Open File");
    AppendMenu(hFileMenu,MF_SEPARATOR,NULL,NULL); // Draws a horizontal dividing line
    AppendMenuA(hFileMenu,MF_STRING,FILE_MENU_EXIT,"Exit");
    AppendMenuA(hMenu,MF_POPUP,(UINT_PTR)hFileMenu,"File"); 
    AppendMenuA(hMenu,MF_STRING,UNDO,"Undo");
    AppendMenuA(hMenu,MF_STRING,REDO,"Redo");
    AppendMenuA(hMenu,MF_POPUP,(UINT_PTR)hHelp,"Help");
    AppendMenuA(hHelp,MF_STRING,VIEW_HELP,"View Help");
    AppendMenuA(hHelp,MF_STRING,NULL,"Send Feedback(Not Supported)");
    AppendMenuA(hHelp,MF_STRING,TD_TEXT,"About TDText");
    SetMenu(m_hwnd,hMenu);  
}
void MainWindow::AddControls()  // Let's add an edit control where we can text and get the text
{
    //We use CW_USEDEFAILT -10 for width and height so that the edit control can change size base on the window
    // '-10' to avoid the edit has the same or bigger size with the window which will raise error
   hEdit = CreateWindowExA(0,"Edit","",WS_VISIBLE|WS_CHILD|ES_MULTILINE|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_AUTOHSCROLL
   ,0,0,680,440,m_hwnd,NULL,NULL,NULL);
    //WS_VISIBLE: make the edit control initially visible
    // WS_CHILD: The window is a child window. A window with this style cannot have a menu bar.
    //WS_BORDER: The window has a thin-line border.
    //ES_MULTILINE: Designates a multiline edit control. Default is single line edit control
    // ES_AUTOVSCROLL:Automatically scrolls text up one page when the user presses the ENTER key on the last line.
    //ES_AUTOHSCROLL: Automatically scrolls text to the right by 10 characters when the user types a character at the end of the line.
    // When the user presses the ENTER key, the control scrolls all text back to position zero
    //WS_HSCROLL: The window has a horizontal scroll bar.
}
void MainWindow::undo()
{
    // Check if the stack 'file_undo'' is empty
    if(isempty(file_undo) == false)
    {
        // we set undo_parameter to be true so that if user text anything after undo, we know to push the old_text into stack file_undo
       undo_parameter = true;
       size = GetWindowTextLength(hEdit);
       char *redo_data = new char[size + 1];
       GetWindowTextA(hEdit,redo_data,size+1);
       redo_data[size] = '\0';
       string a(redo_data);
       // Everytime we undo, we neen to push the old_text to file_redo so that we can re do what we've just undo
       push(&file_redo,a);
       string data = pop(&file_undo);
       old_text = data;
       int len = data.length();
       char *mydata = new char[len+1];
       strcpy(mydata,data.c_str()); // this is how we conver 'string' into 'char', because SetWindowText don't use 'string' type
       // we use dynamic memmories so we need to set last element '\0' to finish the string
       mydata[len] = '\0';
       SetWindowTextA(hEdit,mydata);
       delete[] mydata;
       delete[] redo_data;
    }
}
void MainWindow::redo()
{
    if(isempty(file_redo) == false)
    {
        redo_parameter = true;
        size = GetWindowTextLength(hEdit);
        char *undo_data = new char[size + 1];
        GetWindowTextA(hEdit,undo_data,size+1);
        // we use dynamic memmories so we need to set last element '\0' to finish the string
        undo_data[size] = '\0';
        string a(undo_data);
        // Everytime we redo, we need to push the old text to 'file_undo' so that we can undo what we've just redo
        push(&file_undo,a);
        string data = pop(&file_redo);
        old_text = data;
        int len = data.length();
        char *mydata = new char(len+1);
        // this is how we conver 'string' into 'char', becasue SetWindowText don't use 'string' type
        strcpy(mydata,data.c_str());
        // we use dynamic memmories so we need to set last element '\0' to finish the string
        mydata[len] = '\0';
        // display the redo text
        SetWindowTextA(hEdit,mydata);
        // update size of the text becasue "SetWindowText()" doesn't send EN_UPDATE measage(means we can update size in case : EN_UPDATE)
        size = GetWindowTextLength(hEdit);
        delete[] mydata;
        delete[] undo_data;
    }
}
// Now we create the window:
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdline, INT nCmdShow)
{
    // Now it's time to create the window
    MainWindow win;
    win.initing();
    if (!win.Create("TDText",WS_OVERLAPPEDWINDOW))
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
