#include "pong.hh"

#include "animid.hh"
#include "timing.hh"

#define START_DATE	pongStartDate
#define END_DATE	pongEndDate

// Dimensions de l'"ecran de jeu" du pong (raquettes exclues)
#define PONG_HEIGHT 0.5f // taille d'une case de la ville
#define PONG_WIDTH 0.25f // espace entre les 2 voies d'une avenue
// Case de la ville ou la scene commence
#define XSQUARE 30
#define YSQUARE 46
// Vitesse de deplacement de la scene dans la ville (cases/ms)
#define SPEED 0.002f
// Date a partir de laquelle la scene arrete de se deplacer
#define STOP_DATE 7000
// Dimensions des elements
#define PADDLE_SIZE 0.05f
#define BALL_SIZE 0.05f

// Position du coin superieur gauche de l'ecran de jeu
#define PONG_XPOS ((XSQUARE)/2 - 0.1f)
#define PONG_YPOS 0.05f
#define PONG_ZPOS ((YSQUARE)/2 - 0.25f)

#define PONG_REAL_WIDTH (PONG_WIDTH - BALL_SIZE)

namespace Pong
{
  /*
  struct PaddlePos {
    float pos;  // 0 = en bas de l'ecran, 1 = en haut
    unsigned duration;
  };
  struct BallPos {
    float xPos;
    float yPos;
    unsigned duration;
  };

  void createPaddle(const PaddlePos* yPos, float xPos, unsigned num, RenderList& renderList);
  void createBall(RenderList& renderList);

// Durée du jeu, avant accélération selon (END_DATE - START_DATE)
//#define LENGTH 22000
//
//#define REAL_SPEED (SPEED * (LENGTH * 1.f / (END_DATE - START_DATE)))
//#define REAL_STOP_DATE (STOP_DATE * (LENGTH * 1.f / (END_DATE - START_DATE)))

  static const PaddlePos leftPaddlePos[] = {
    {0.50f,    0},
    {0.60f, 1000},
    {0.25f,  750},
    {0.40f,  250},
    {0.60f, 1000},
    {0.30f, 1000},
    {0.15f,  500},
    {0.50f, 1500},
    {0.65f,  500},
    {0.30f, 1500},
    {0.20f,  500},
    {0.50f, 1500},
    {0.70f,  500},
    {0.20f, 1250},
    {0.35f,  250},
    {0.65f, 1000},
  };
  static const PaddlePos rightPaddlePos[] = {
    {0.50f,    0},
    {0.20f, 1000},
    {0.15f,  250},
    {0.80f, 1750},
    {0.90f,  500},
    {0.50f, 1500},
    {0.40f,  500},
    {0.40f, 1000},
    {0.70f,  500},
    {0.80f,  250},
    {0.80f, 1500},
    {0.70f,  250},
    {0.60f,  250},
    {0.60f,  750},
    {0.70f,  250},
    {0.30f,  750},
    {0.50f, 2000},
  };
  static const BallPos ballPos[] = {
    {0.00f, 0.50f,    0},
    {1.00f, 0.20f, 1000},
    {0.00f, 0.40f, 1000},
    {1.00f, 0.80f, 1000},
    {0.00f, 0.30f, 1000},
    {1.00f, 0.50f, 1000},
    {0.50f, 0.10f,  500},
    {0.00f, 0.50f,  500},
    {1.00f, 0.70f, 1000},
    {0.70f, 0.90f,  250},
    {0.00f, 0.30f,  750},
    {0.30f, 0.10f,  250},
    {1.00f, 0.70f,  750},
    {0.00f, 0.50f, 1000},
    {1.00f, 0.30f, 1000},
    {0.67f, 0.10f,  250},
    {0.00f, 0.50f,  750},
    {-1.0f, 1.10f, 1000},
  };
  */

  void create(RenderList& renderList)
  {
    glPushMatrix();
    glLoadIdentity();
    Renderable leftPaddle(END_DATE, Shader::default, START_DATE, PADDLE_SIZE);
    leftPaddle.setAnimation(Anim::pongLeftPaddle);
    renderList.add(leftPaddle);

    Renderable rightPaddle(END_DATE, Shader::default, START_DATE, PADDLE_SIZE);
    rightPaddle.setAnimation(Anim::pongRightPaddle);
    renderList.add(rightPaddle);

    Renderable ball(END_DATE, Shader::color, START_DATE, BALL_SIZE,
		    Cube::numberOfVertices, VBO::theCube);
    ball.setAnimation(Anim::pongBall);
    renderList.add(ball);

    glPopMatrix();
  }
}
