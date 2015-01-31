#include "stdafx.h"
#include "appwnd.h"

//
// CApplicationWindow class
//

class CApplicationWindow : public CAppWindow
{
public:
    // Windows messages handlers
    BEGIN_MSG_MAP(CApplicationWindow)
      MESSAGE_HANDLER(WM_KEYDOWN, _OnKeyDown);
      CHAIN_MSG_MAP(CAppWindow)
    END_MSG_MAP()

protected:
    virtual void OnFinalMessage(HWND)
    {        
        // post WM_QUIT on destroy window
        // required if used GetMessage function in message pump
        ::PostQuitMessage(0);
    }

private:
    // Windows messages handlers
    LRESULT _OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        // destroy window if ESCAPE pressed
        if ( wParam == VK_ESCAPE )
        {
            DestroyWindow();
        }

        return 0;
    }
};

//
// Entry point
//

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
    RECT r = { 0, 0, 640, 480 };

    // create process main window
    // use popup style to avoid border/title
    CApplicationWindow wnd;
    HWND hWnd = wnd.Create(NULL, r, NULL, WS_POPUP);

    if ( hWnd != NULL )
    {
        // center window relatively screen
        wnd.CenterWindow();

        // show window
        ::ShowWindow(hWnd, SW_SHOW);

        // message pump
        // (ends when window destroyed)
        while ( ::IsWindow(hWnd) )
        {
            MSG msg;
            if ( ::PeekMessage(&msg, 0, 0, 0, PM_REMOVE) )
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            else
            {
                // refresh on idle
                wnd.Invalidate();
            }
        }

        hWnd = NULL;
    }

    return 0;
}
