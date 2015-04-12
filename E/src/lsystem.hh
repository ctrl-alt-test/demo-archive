//
// L system
//

#ifndef		L_SYSTEM_HH
# define	L_SYSTEM_HH

#include "animid.hh"
#include "basicTypes.hh"
#include "lsystemrule.hh"
#include "renderlist.hh"
#include "stack.hh"
#include "shaderid.hh"

namespace LSystem
{
  typedef char * state;

  typedef void(*Function)(date birth, date death, unsigned int depth);

  class System
  {
    // Première partie : développement de la grammaire
  public:
    System(RenderList & r,
	   date deathDate, date birthDate = 0,
	   Shader::id shaderId = Shader::parallax,//default,
	   Anim::id animId = Anim::none,
	   Texture::id textureId = Texture::none,
	   float angle = 30.f, float reduction = 1.f,
           float cubeForward = 2.5f, unsigned char cubesPerForward = 3,
	   int birthDateIncrement = 0, int deathDateIncrement = 0,
           Function renderFct = NULL);

    void setDates(date birthDate, date deathDate);
    void setAngle(float angle);
    void setAnimation(Anim::id animId);
    void setPropagation(date deathDate, date birthDate, int birthDateIncrement, int deathDateIncrement);

    state iterate(const Rule & X, const Rule & F,
		  const Rule & G, const Rule & H,
		  int iterations);
    void addStateToList(state state);

  private:

    // Seconde partie : interprétation de l'état
    void readState(state state);
    void forward();
    void left();
    void right();
    void down();
    void up();
    void counterClock();
    void clock();
    void push();
    void pop();

  private:
    System& operator=(const System &) { assert(false); }

    stack<unsigned int> _depthStack;
    unsigned int _currentDepth;

    RenderList & _renderList;

    date _birthDate;
    date _deathDate;
    date _birthDateIncrement;
    date _deathDateIncrement;
    Anim::id	_animId;
    Shader::id	_shaderId;
    Texture::id	_textureId;

    float _angle;
    float _reduction;
    float _cubeForward;
    unsigned char _cubesPerForward;
    Function _renderFct;
  };

  typedef void(System::*reader)();
}

#endif		// L_SYSTEM_HH
