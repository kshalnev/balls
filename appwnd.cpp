#include "stdafx.h"
#include "appwnd.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

//
// Consts
//

static const float BALL_SIZE = 10.0;
static const size_t BALLS_COUNT = 25; // 12

//
// CAppWindow class
//

CAppWindow::CAppWindow() 
{
    m_dwTime = 0;
    m_gdiplusToken = 0;
    m_hDC = NULL;
    m_hMemDC = NULL;
    m_hMemBmp = NULL;
    m_hOldMemBmp = NULL;
    ZeroMemory(&m_memBmp, sizeof(m_memBmp));
}

CAppWindow::~CAppWindow() 
{
    // do window destroyed properly
    ASSERT(m_gdiplusToken == 0);
    ASSERT(m_hDC == NULL);
    ASSERT(m_hMemDC == NULL);
    ASSERT(m_hMemBmp == NULL);
    ASSERT(m_hOldMemBmp == NULL);
}

LRESULT CAppWindow::_OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // window client rect is
    RECT rect;
    GetClientRect(&rect);

    // capture primary dc
    HDC hDC = GetDC();

    // back buffer bitmap info is
    BITMAPINFO bi = { 0 };
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = rect.right;
    bi.bmiHeader.biHeight = -rect.bottom; // zero on top
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biSizeImage = 0;
    bi.bmiHeader.biXPelsPerMeter = 0;
    bi.bmiHeader.biYPelsPerMeter = 0;
    bi.bmiHeader.biClrUsed = 0;
    bi.bmiHeader.biClrImportant = 0;

    // create and initialize back buffer
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, NULL, NULL, 0);
    HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

    // get back buffer bitmap info
    BITMAP bmp;
    ZeroMemory(&bmp, sizeof(bmp));
    GetObject(hMemBmp, sizeof(bmp), &bmp);

    // init members
    m_hDC = hDC;
    m_hMemDC = hMemDC;
    m_hMemBmp = hMemBmp;
    m_hOldMemBmp = hOldMemBmp;
    m_memBmp = bmp;

    m_ballsSystem = std::auto_ptr<CBallsSystem>(
      new CBallsSystem(
        CRectF((float)rect.left, (float)rect.top, (float)rect.right, (float)rect.bottom), 
        BALLS_COUNT, BALL_SIZE));

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    m_dwTime = GetTickCount();

    return 0;
}

LRESULT CAppWindow::_OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
    m_gdiplusToken = 0;

    // delete gdi objects
    SelectObject(m_hMemDC, m_hOldMemBmp);
    DeleteObject(m_hMemBmp);
    DeleteDC(m_hMemDC);
    ReleaseDC(m_hDC);

    // reset members
    m_hDC = NULL;
    m_hMemDC = NULL;
    m_hMemBmp = NULL;
    m_hOldMemBmp = NULL;
    ZeroMemory(&m_memBmp, sizeof(m_memBmp));
    
    return 0;
}
LRESULT CAppWindow::_OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // update back buffer
    _UpdateBuffer();

    // copy back buffer to primary dc
    _ShowBuffer();
    
    return 0;
}

LRESULT CAppWindow::_OnEraseBgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // do nothing
    return 1;
}

void CAppWindow::_ShowBuffer()
{
    // copy back buffer into primary dc
    BitBlt(m_hDC, 0, 0, m_memBmp.bmWidth, m_memBmp.bmHeight, m_hMemDC, 0, 0, SRCCOPY);
}

void CAppWindow::_UpdateBuffer()
{
    // update frame in back buffer
    // back buffer specified by m_hMemDC, m_hMemBmp and m_memBmp
    _Draw(m_hMemDC, m_memBmp);
}

void CAppWindow::_Draw(HDC hDC, const BITMAP& bmp)
{
  // black screen
  ZeroMemory(bmp.bmBits, bmp.bmWidthBytes * bmp.bmHeight);
  
  // create graphics
  Gdiplus::Graphics graphics(hDC);

  // set smoothing antialiasing mode
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

  CBallsSystem::Balls::const_iterator i = m_ballsSystem->begin(), iend = m_ballsSystem->end();
  Gdiplus::Status status = Gdiplus::Ok;
  for ( ; i != iend; ++i ) 
  {
    const CBall* ball = *i;
    const CRectF& ballRect = ball->GetBoundBox();

    // create a brush object.
    Gdiplus::Color color;
    color.SetFromCOLORREF(ball->GetColor());
    Gdiplus::SolidBrush brush(color);

    // draw the ellipse.
    status = graphics.FillEllipse(&brush,
      ballRect.GetLeft(), ballRect.GetTop(), ballRect.GetWidth(), ballRect.GetHeight());   
  }

  DWORD dwTime = GetTickCount();

  m_ballsSystem->Update(dwTime - m_dwTime);

  m_dwTime = dwTime;
}
