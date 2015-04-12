//
// L system
//

#include "lsystem.hh"

#include "sys/msys.h"

#include "lsystemalphabet.hh"
#include "renderable.hh"
#include "shaderid.hh"

namespace LSystem
{
  System::System(RenderList & r, // FIXME : trouver un moyen plus propre
		 date deathDate, date birthDate,
		 Shader::id shaderId,
		 Anim::id animId,
		 Texture::id textureId,
		 float angle, float reduction,
		 float cubeForward, unsigned char cubesPerForward,
		 int birthDateIncrement, int deathDateIncrement,
                 Function renderFct):
    _depthStack(10), _currentDepth(0),
    _renderList(r),
    _birthDate(birthDate),
    _deathDate(deathDate),
    _birthDateIncrement(birthDateIncrement),
    _deathDateIncrement(deathDateIncrement),
    _animId(animId),
    _shaderId(shaderId),
    _textureId(textureId),
    _angle(angle), _reduction(reduction),
    _cubeForward(cubeForward), _cubesPerForward(cubesPerForward),
    _renderFct(renderFct)
  {
    assert(_birthDate <= _deathDate);
  }

  void System::setDates(date birthDate, date deathDate)
  {
    _birthDate = birthDate;
    _deathDate = deathDate;
  }

  void System::setAngle(float angle)
  {
    _angle = angle;
  }

  void System::setAnimation(Anim::id animId)
  {
    _animId = animId;
  }

  void System::setPropagation(date birthDate, date deathDate,
			      int birthDateIncrement, int deathDateIncrement)
  {
    _birthDate = birthDate;
    _deathDate = deathDate;
    _birthDateIncrement = birthDateIncrement;
    _deathDateIncrement = deathDateIncrement;
  }

  //
  // Première partie : développement de la grammaire
  //

  //
  // Applique la règle sur l'état et renvoie un nouvel état alloué
  //
  state applyRule(state currentState,
		  const Rule & X, const Rule & F,
		  const Rule & G, const Rule & H)
  {
    const int stateLength = msys_strlen(currentState);
    int longest = (X.length() > F.length() ? X.length() : F.length());
    if (G.length() > longest)
      longest = G.length();
    if (H.length() > longest)
      longest = H.length();
    const int maxLength = stateLength * longest;
    state newState = (state)msys_mallocAlloc((maxLength + 1) * sizeof(char));

    int j = 0;
    for (int i = 0; i < stateLength; ++i)
    {
      switch (currentState[i])
      {
      case (char)FORWARD1:
	for (int k = 0; k < X.length(); ++k)
	{
	  newState[j] = X[k];
	  ++j;
	}
	break;
      case (char)FORWARD2:
	for (int k = 0; k < F.length(); ++k)
	{
	  newState[j] = F[k];
	  ++j;
	}
	break;
      case (char)FORWARD3:
	for (int k = 0; k < G.length(); ++k)
	{
	  newState[j] = G[k];
	  ++j;
	}
	break;
      case (char)FORWARD4:
	for (int k = 0; k < H.length(); ++k)
	{
	  newState[j] = H[k];
	  ++j;
	}
	break;
      default:
	newState[j] = currentState[i];
	++j;
      }
    }
    newState[j] = '\0';

    return newState;
  }

  //
  // Construit le résultat de n applications de la règle sur le
  // postulat de départ F.
  //
  state System::iterate(const Rule & X, const Rule & F,
		const Rule & G, const Rule & H,
		int iterations)
  {
    state currentState = (state)msys_mallocAlloc(2 * sizeof(char));
    currentState[0] = (char)FORWARD1;
    currentState[1] = '\0';

    for (int i = 0; i < iterations; ++i)
    {
      state newState = applyRule(currentState, X, F, G, H);
      msys_mallocFree(currentState);
      currentState = newState;
    }

    return currentState;
  }


  //
  // Seconde partie : interprétation de l'état
  //

  void System::readState(state state)
  {
    const int length = msys_strlen(state);

    reader functions[] = {
      NULL,
      &System::left,
      &System::right,
      &System::down,
      &System::up,
      &System::counterClock,
      &System::clock,
      &System::push,
      &System::pop,
      &System::forward,
      &System::forward,
      &System::forward,
      &System::forward
    };

    _currentDepth = 0;

    for (int i = 0; i < length; ++i)
    {
      alphabetElement operation = (alphabetElement)state[i];
      reader f = functions[operation];
      if (f != NULL)
      {
	(this->*f)();
      }
    }
  }

  void System::forward()
  {
    // Translation en avant
    for (unsigned char i = 0; i < _cubesPerForward; i++)
    {
      ++_currentDepth;
      const date birth = _birthDate + _currentDepth * _birthDateIncrement;
      const date death = _deathDate + _currentDepth * _deathDateIncrement;

      // Si on dépasse les bornes on arrête
      if (birth >= death)
      {
	break;
      }

      glTranslatef(0, 0, _cubeForward);
      if (_renderFct) // FIXME : pas génial pour l'efficacité
      {
        _renderFct(birth, death, _currentDepth);
      }
      else
      {
        Renderable node(death, _shaderId, birth);
	node.setTextures(_textureId, Texture::none, Texture::none);
	node.setAnimation(_animId);
	_renderList.add(node);
      }
    }
  }

  void System::left()
  {
    // Rotation à gauche
    glRotatef(_angle, 0, 1, 0);
  }

  void System::right()
  {
    // Rotation à droite
    glRotatef(-_angle, 0, 1, 0);
  }

  void System::down()
  {
    // Rotation vers le bas
    glRotatef(_angle, 1, 0, 0);
  }

  void System::up()
  {
    // Rotation vers le haut
    glRotatef(-_angle, 1, 0, 0);
  }

  void System::counterClock()
  {
    // Rotation dans le sens positif
    glRotatef(_angle, 0, 0, 1);
  }

  void System::clock()
  {
    // Rotation dans le sens négatif
    glRotatef(-_angle, 0, 0, 1);
  }

  void System::push()
  {
    // Sauvegarde de l'environnement
    glPushMatrix();
    glScalef(_reduction, _reduction, _reduction);
    _depthStack.push(_currentDepth);
  }

  void System::pop()
  {
    // Restauration de l'environnement
    _currentDepth = _depthStack.top();
    _depthStack.pop();
    glPopMatrix();
  }

  void System::addStateToList(state state)
  {
    glPushMatrix();
    readState(state);
    glPopMatrix();
  }
}
