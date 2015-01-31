#pragma once

#include "balls.h"

//
// CAppWindow class
//
// implemented support of back buffering to avoid flashing

class CAppWindow : public CWindowImpl<CAppWindow>
{
public:
    // constructor/destructor
    CAppWindow();
    ~CAppWindow();

    // Windows messages handlers
    BEGIN_MSG_MAP(CAppWindow)
        MESSAGE_HANDLER(WM_CREATE, _OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, _OnDestroy)
        MESSAGE_HANDLER(WM_PAINT, _OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, _OnEraseBgnd)        
    END_MSG_MAP()

private:
    // Windows messages handlers
    LRESULT _OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT _OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT _OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT _OnEraseBgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);    

    // update back buffer
    inline void _UpdateBuffer();

    // copy back buffer to primary dc
    inline void _ShowBuffer();

    // draw buffer
    void _Draw(HDC hDC, const BITMAP& bmp);

private:
    HDC m_hDC; // captured primary DC for drawing
    HDC m_hMemDC; // memory DC of back buffer
    HBITMAP m_hMemBmp; // bitmap of back buffer
    BITMAP m_memBmp; // bitmap info of back buffer
    HBITMAP m_hOldMemBmp; // bitmap of memory DC of back buffer that was definied initially

    std::auto_ptr<CBallsSystem> m_ballsSystem;

    ULONG_PTR m_gdiplusToken; // gdiplus token

    DWORD m_dwTime; // timer
};
