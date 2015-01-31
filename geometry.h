#pragma once

//
// CPointF class
//

class CPointF
{
public:
  // constructor
  CPointF(float x = 0.0f, float y = 0.0f);

  // public accessors
  inline float GetX() const { return m_x; }
  inline float GetY() const { return m_y; }

  // public modifiers
  inline void Set(float x, float y) { 
    m_x = x; 
    m_y = y;
  }
  inline void Offset(float cx, float cy) {
    Set(m_x + cx, m_y + cy);
  }

private:
  float m_x; // x coordinate
  float m_y; // y coordinate
};

//
// CRectF class
//

class CRectF
{
public:
  // constructor
  CRectF(float left = 0.0f, float top = 0.0f, float right = 0.0f, float bottom = 0.0f);

  // public accessors
  inline float GetLeft() const { return m_left; }
  inline float GetTop() const { return m_top; }
  inline float GetRight() const { return m_right; }
  inline float GetBottom() const { return m_bottom; }
  inline float GetWidth() const { return ( m_right - m_left ); }
  inline float GetHeight() const { return ( m_bottom - m_top ); }

  // public modifies
  inline void Set(float left, float top, float right, float bottom) {
    ASSERT(left <= right);
    ASSERT(top <= bottom);
    m_left = left; 
    m_top = top; 
    m_right = right; 
    m_bottom = bottom;
  }
  inline void Offset(float cx, float cy) {
    Set(m_left + cx, m_top + cy, m_right + cx, m_bottom + cy);
  }

  // public functions
  inline bool PointInRect(const CPointF& point) const {
    return 
      m_left <= point.GetX() && point.GetX() <= m_right &&
      m_top <= point.GetY() && point.GetY() <= m_bottom;
  }
  inline bool IsIntersect(const CRectF& rect) const {
    return 
      (!(m_left > rect.m_right || m_right < rect.m_left ||
         m_top > rect.m_bottom || m_bottom < rect.m_top));
  }

private:
  float m_left, m_top, m_right, m_bottom;
};

//
// CVectorF class
//

class CVectorF
{
public:
  // constructors
  CVectorF();
  CVectorF(float x, float y);
  CVectorF(const CPointF& p1, const CPointF& p2);

  // public accessors
  inline float GetX() const { return m_x; }
  inline float GetY() const { return m_y; }
  inline float GetSize() const { return m_size; }
  
  // public modifiers
  inline void Set(float x, float y) {
    m_x = x; 
    m_y = y;
    m_size = _hypotf(x, y);
    if ( m_size > 0.0f )
    {
      m_x /= m_size;
      m_y /= m_size;
    }
  }
  inline void SetSize(float size) { 
    ASSERT(size >= 0.0f);
    m_size = size;
  }

  // friends
  friend CVectorF operator + (const CVectorF& v1, const CVectorF& v2);

private:
  float m_x, m_y; // unitary vector X,Y (X-cosine, Y-sine of angle between axis X and vector direction)
  float m_size; // vector length
};

CVectorF operator + (const CVectorF& v1, const CVectorF& v2);
