#include "stdafx.h"
#include "geometry.h"

//
// CPointF class
//

CPointF::CPointF(float x, float y)
  : m_x(x), m_y(y)
{
}

//
// CRectF class
//

CRectF::CRectF(float left, float top, float right, float bottom)
  : m_left(0.0f), m_top(0.0f), m_right(0.0f), m_bottom(0.0f)
{
  Set(left, top, right, bottom);
}

//
// CVectorF class
//

CVectorF::CVectorF()
  : m_x(0.0f), m_y(0.0f), m_size(0.0f)
{
}

CVectorF:: CVectorF(const CPointF& p1, const CPointF& p2)
  : m_x(0.0f), m_y(0.0f), m_size(0.0f)
{
  Set(p2.GetX()-p1.GetX(), p2.GetY()-p1.GetY());
}

CVectorF::CVectorF(float x, float y)
  : m_x(0.0f), m_y(0.0f), m_size(0.0f)
{
  Set(x, y);
}

CVectorF operator + (const CVectorF& v1, const CVectorF& v2)
{
  float x = v1.m_x * v1.m_size + v2.m_x * v2.m_size;
  float y = v1.m_y * v1.m_size + v2.m_y * v2.m_size;
  return CVectorF(x, y);
}
