#pragma once

#include "geometry.h"

//
// CBall class
//

class CBall
{
public:
  // constructor
  CBall(float radius = 1.0f);

  // public accessors
  inline float GetRadius() const {
    return m_radius;
  }
  inline const CPointF& GetPosition() const {
    return m_position;
  }
  inline const CVectorF& GetSpeed() const {
    return m_speed;
  }
  inline CRectF GetBoundBox() const {
    return CRectF(
      m_position.GetX() - m_radius, 
      m_position.GetY() - m_radius, 
      m_position.GetX() + m_radius, 
      m_position.GetY() + m_radius);
  }
  inline COLORREF GetColor() const {
    return m_color;
  }

  // public modifiers
  inline void SetPosition(const CPointF& position) {
    m_position = position;
  }
  inline void SetSpeed(const CVectorF& speed) {
    m_speed = speed;
  }  
  inline void SetColor(COLORREF color) {
    m_color = color;
  }

private:
  const float m_radius; // ball radius  
  CPointF m_position; // ball position
  CVectorF m_speed; // vector of ball speed
  COLORREF m_color; // color of ball
};

//
// CCollisionsResolver class 
// 

class CCollisionsResolver
{
public:
  CCollisionsResolver(const CRectF& area);

  void Resolve(CBall* balls[], size_t ballsCount) const;

private:
  bool _ResolveAreaCollisions(CBall* balls[], size_t ballsCount) const;
  static bool _ResolveBallsCollisions(CBall* balls[], size_t ballsCount);
  static bool _ResolveBallsCollision(CBall* ball1, CBall* ball2);
  static CVectorF _ReflectVector(const CVectorF& vector, const CVectorF& plane);

private:
  const CRectF m_area;
  const CPointF m_leftTop;
  const CPointF m_leftBottom;
  const CPointF m_rightTop;
  const CPointF m_rightBottom;
};

//
// CBallsSystem class
//

class CBallsSystem
{
public:
  typedef std::vector<CBall*> Balls;

  CBallsSystem(const CRectF& area, size_t ballCount, float ballRadius);
  ~CBallsSystem();

  void Update(size_t milliseconds);

  inline Balls::const_iterator begin() const {
    return m_balls.begin();
  }
  inline Balls::const_iterator end() const {
    return m_balls.end();
  }

private:
  void _Clear();
  void _UpdateBall(CBall* ball, size_t milliseconds);

  static float _RandomSign();
  static float _Random(float max);

private:
  Balls m_balls;
  CCollisionsResolver m_collisionsResolver;
};
