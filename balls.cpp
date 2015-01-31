#include "stdafx.h"
#include "balls.h"

//
// Consts
//

static const int MAX_ATTEMPTS_TO_RESOLVE = 3;
static const float SPEED_ATTENUATION = 1.0f; // 0.9995f;

//
// CBall class
//

CBall::CBall(float radius)
  : m_radius(radius)
  , m_color(RGB(rand()%256, rand()%256, rand()%256))
{
}

//
// CCollisionsResolver class
//

CCollisionsResolver::CCollisionsResolver(const CRectF& area)
  : m_area(area)
  , m_leftTop(area.GetLeft(), area.GetTop())
  , m_leftBottom(area.GetLeft(), area.GetBottom())
  , m_rightTop(area.GetRight(), area.GetTop())
  , m_rightBottom(area.GetRight(), area.GetBottom())
{
}

void CCollisionsResolver::Resolve(CBall* balls[], size_t ballsCount) const
{
  ASSERT(balls != NULL);
  ASSERT(ballsCount > 0);

  // NOTE
  // due optimization purposes assumed that all balls are inside field
  #ifdef _DEBUG
  if ( 0 )
  { 
    for ( size_t i = 0; i < ballsCount; ++i ) { 
      ASSERT(balls[i] != NULL);
      const CRectF& boundingBox = balls[i]->GetBoundBox();
      if ( boundingBox.GetLeft() < m_area.GetLeft() ||
        boundingBox.GetRight() > m_area.GetRight() ||
        boundingBox.GetTop() < m_area.GetTop() ||
        boundingBox.GetBottom() > m_area.GetBottom() )
      {
        // ball outside of area. wrong logic?
        ASSERT(FALSE);
      }
    }
  }
  #endif

  bool areaCollisions = true;
  bool ballsCollisions = true;
  int attemptsCounter = 0;
  do
  {
    // resolve collisions with area borders
    areaCollisions = _ResolveAreaCollisions(balls, ballsCount);

    // resolve collisions between balls
    ballsCollisions = _ResolveBallsCollisions(balls, ballsCount);
  }
  while ( (areaCollisions || ballsCollisions) && (++attemptsCounter < MAX_ATTEMPTS_TO_RESOLVE) );

  #ifdef _DEBUG
  {
    // if collision happens then output debug log
    if ( areaCollisions || ballsCollisions ) {
      TCHAR szMessage[512];
      _sntprintf_s(szMessage, sizeof(szMessage)/sizeof(szMessage[0]), 
        _T("Warning: CCollisionsResolver::Resolve exit with collisions ")
        _T("(AreaCollisions:%s, BallsCollisions:%s, Attempts: %i)\n")
        , areaCollisions ? _T("Yes") : _T("No")
        , ballsCollisions ? _T("Yes") : _T("No")
        , attemptsCounter
        );
      OutputDebugString(szMessage);
    }
  }
  #endif
}

bool CCollisionsResolver::_ResolveAreaCollisions(CBall* balls[], size_t ballsCount) const
{
  ASSERT(balls != NULL);
  ASSERT(ballsCount > 0);

  bool res = false;

  for ( size_t i = 0; i < ballsCount; ++i )
  {
    CBall* const ball = balls[i];
    ASSERT(ball != NULL);

    const float ballRadius = ball->GetRadius();
    const CPointF& ballPosition = ball->GetPosition();

    // check collision on left margin
    if ((ballPosition.GetX() - ballRadius) < m_area.GetLeft())
    {
      CVectorF plane(m_leftBottom, m_leftTop);
      ball->SetSpeed(_ReflectVector(ball->GetSpeed(), plane));
      ball->SetPosition(CPointF(m_area.GetLeft() + ballRadius, ballPosition.GetY()));
      res = true;
    }
    // check collision on right margin
    else if ((ballPosition.GetX() + ballRadius) > m_area.GetRight())
    {
      CVectorF plane(m_rightTop, m_rightBottom);
      ball->SetSpeed(_ReflectVector(ball->GetSpeed(), plane));
      ball->SetPosition(CPointF(m_area.GetRight() - ballRadius, ballPosition.GetY()));
      res = true;
    }
    // check collision on top margin
    if ((ballPosition.GetY() - ballRadius) < m_area.GetTop())
    {
      CVectorF plane(m_leftTop, m_rightTop);
      ball->SetSpeed(_ReflectVector(ball->GetSpeed(), plane));
      ball->SetPosition(CPointF(ballPosition.GetX(), m_area.GetTop() + ballRadius));
      res = true;
    }
    // check collision on bottom margin
    else if ((ballPosition.GetY() + ballRadius) > m_area.GetBottom())
    {
      CVectorF plane(m_rightBottom, m_leftBottom);
      ball->SetSpeed(_ReflectVector(ball->GetSpeed(), plane));
      ball->SetPosition(CPointF(ballPosition.GetX(), m_area.GetBottom() - ballRadius));
      res = true;
    }
  }

  return res;
}

bool CCollisionsResolver::_ResolveBallsCollisions(CBall* balls[], size_t ballsCount)
{
  ASSERT(balls != NULL);
  ASSERT(ballsCount > 0);

  bool res = false;

  // go through all pairs of balls and resolve their collisions
  const size_t ballsCountMinusOne = ballsCount - 1;
  for ( size_t i = 0; i < ballsCountMinusOne; ++i ) 
  {
    CBall* const ball1 = balls[i];
    ASSERT(ball1 != NULL);

    for ( size_t j = i + 1; j < ballsCount; ++j ) 
    {
      CBall* const ball2 = balls[j];
      ASSERT(ball2 != NULL);

      res |= _ResolveBallsCollision(ball1, ball2);
    }
  }

  return res;
}

bool CCollisionsResolver::_ResolveBallsCollision(CBall* ball1, CBall* ball2)
{
  ASSERT(ball1 != NULL);
  ASSERT(ball2 != NULL);

  const CPointF& position1 = ball1->GetPosition();
  const CPointF& position2 = ball2->GetPosition();

  const float summRadiuses = ball1->GetRadius() + ball2->GetRadius();
  
  const float dx = fabs(position1.GetX() - position2.GetX());
  if ( dx > summRadiuses )
    return false; // no collision

  const float dy = fabs(position1.GetY() - position2.GetY());
  if ( dy > summRadiuses )
    return false; // no collision

  const float distance = _hypotf(dx, dy);
  if ( distance > summRadiuses )
    return false; // no collision

  const CVectorF& speed1 = ball1->GetSpeed();
  const CVectorF& speed2 = ball2->GetSpeed();
  const CVectorF& plane = speed1 + speed2;

  // reflect speeds
  ball1->SetSpeed(_ReflectVector(speed1, plane));
  ball2->SetSpeed(_ReflectVector(speed2, plane));

  // fix positions
  const CVectorF ortSummSpeeds(-plane.GetY(), plane.GetX());
  const float distanceFix = ( summRadiuses - distance ) / 2.0f + 0.01f;
  const float z = speed1.GetX() * plane.GetY() - speed1.GetY() * plane.GetX();
  float dir1 = -1, dir2 = 1;
  if ( z < 0 ) {
    dir1 *= -1; dir2 *= -1;
  }

  // fix first ball position
  CPointF position = ball1->GetPosition();
  position.Offset(dir1 * ortSummSpeeds.GetX() * distanceFix, dir1 * ortSummSpeeds.GetY() * distanceFix);
  ball1->SetPosition(position);
  
  // fix second ball position
  position = ball2->GetPosition();
  position.Offset(dir2 * ortSummSpeeds.GetX() * distanceFix, dir2 * ortSummSpeeds.GetY() * distanceFix);
  ball2->SetPosition(position);

  return true;
}

CVectorF CCollisionsResolver::_ReflectVector(const CVectorF& vector, const CVectorF& plane)
{
  // cosine of angle between vector and plane is same as dot product of unitary vectors
  float cosine = vector.GetX() * plane.GetX() + vector.GetY() * plane.GetY();

  // fix precisions issues
  if ( cosine > 1.0 )
    cosine = 1.0;
  else if ( cosine < -1.0 )
    cosine = -1.0;

  // sine is
  float sine = sqrtf(1 - cosine * cosine);

  float cosine2a = 2 * cosine * cosine - 1;
  float sine2a = 2 * sine * cosine;

  // z of cross product for unitary vectors is
  const float z = vector.GetX() * plane.GetY() - vector.GetY() * plane.GetX();

  // if z greath than zero then reflect on angle
  // otherwise reflect on minus angle
  if ( z < 0 )
  {
    // cos(-a) = cos(a)
    // sin(-a) = -sin(a)
    sine2a = -sine2a;
  }

  // matrix of rotation
  float m11 = cosine2a;
  float m12 = sine2a;
  float m21 = -sine2a;
  float m22 = cosine2a;

  // transformation formula is
  // x' = x * m11 + y * m21 + dx, dx = 0
  // y' = x * m12 + y * m22 + dy, dy = 0

  // reflected vector is
  CVectorF result(vector.GetX() * m11 + vector.GetY() * m21,
                  vector.GetX() * m12 + vector.GetY() * m22);
  result.SetSize(vector.GetSize());

  return result;
}

//
// CBallsSystem class
//

CBallsSystem::CBallsSystem(const CRectF& area, size_t ballCount, float ballRadius)
  : m_collisionsResolver(area)
{
  try
  {
    m_balls.reserve(ballCount);
    for ( size_t i = 0; i < ballCount; ++i )
    {
      // create
      std::auto_ptr<CBall> ball(new CBall(ballRadius));

      // initialize ball
      // ...set position
      ball->SetPosition(
        CPointF((ballRadius + _Random(area.GetWidth() - ballRadius)),
                (ballRadius + _Random(area.GetHeight() - ballRadius))));
      // ...set speed
      ball->SetSpeed(
        CVectorF(_RandomSign() * _Random(5.0f), 
                 _RandomSign() * _Random(5.0f)));

      // add ball in collection
      m_balls.push_back(ball.get());
      ball.release();
    }
  }
  catch (...)
  {
    _Clear();
    throw;
  }
}

CBallsSystem::~CBallsSystem()
{
  _Clear();
}

void CBallsSystem::Update(size_t milliseconds)
{
  // update balls
  Balls::iterator i = m_balls.begin(), iend = m_balls.end();
  for ( ; i != iend; ++i ) {
    CBall* ball = *i;
    _UpdateBall(ball, milliseconds);
  }

  // resolve collisions in balls
  m_collisionsResolver.Resolve(&m_balls[0], m_balls.size());
}

void CBallsSystem::_Clear()
{
  // remove balls
  Balls::iterator i = m_balls.begin(), iend = m_balls.end();
  for ( ; i != iend; ++i ) {
    delete *i;
  }

  // cleanup collection
  m_balls.clear();
}

float CBallsSystem::_RandomSign()
{
  if ( (rand() & 0x1) == 0 )
    return 1;
  else
    return -1;
}

float CBallsSystem::_Random(float max)
{
  const float precision = 10;
  float value = (float)( rand() % (int)(precision * max) ) / precision;
  return value;
}

void CBallsSystem::_UpdateBall(CBall* ball, size_t milliseconds)
{
  ASSERT(ball != NULL);

  const float time = 0.05f * milliseconds;
  const float attenuation = SPEED_ATTENUATION;

  CVectorF speed = ball->GetSpeed();
  CPointF position = ball->GetPosition();

  // update position
  position.Offset(speed.GetX() * speed.GetSize() * time, speed.GetY() * speed.GetSize() * time);
  ball->SetPosition(position);

  // attenuate speed
  speed.SetSize(speed.GetSize() * attenuation);
  ball->SetSpeed(speed);
}
