//
// Toutes les animations utilisées
//

#include "anims.hh"

#include "anim.hxx"
#include "anims.hh"
#include "array.hxx"
#include "node.hxx"
#include "loading.hh"
#include "transformation.hh"
#include "tweakval.hh"

#include "sys/msys.h"

namespace Anim
{
  animFunc list[256] = {NULL};
  static int animCount = 0;
  Anim<Transformation> * nodeIndependantAnims = NULL;

  typedef void(*animBuilder)();

  static void nodeIndependantAnimation(const Node & node, date d)
  {
    Anim<Transformation> & animation = nodeIndependantAnims[node.animId()];
    Transformation t = animation.get(d);
    matrix4 animTransform = computeMatrix(t);

    // FIXME : c'est très laid comme façon de récupérer le résultat
    setToModelView(animTransform);
  }

  // ==========================================================================

#if 0
  //
  // Exemple : rotation autour de y
  //
  // 3 points pour ne pas avoir d'ambiguité sur le sens de rotation,
  // un 4e pour boucler
  //
  static void buildExample1()
  {
    list[exmeple1] = nodeIndependantAnimation;
    Anim<Transformation> & anim = nodeIndependantAnims[example1];
    anim.setAnim(4, 0);

    Transformation t;
    glLoadIdentity();            getFromModelView(t); anim.add(0, t);
    glRotatef(120.f, 0, 1.f, 0); getFromModelView(t); anim.add(5000, t);
    glRotatef(120.f, 0, 1.f, 0); getFromModelView(t); anim.add(10000, t);
    glLoadIdentity();            getFromModelView(t); anim.add(15000, t);

    anim.createCache();
  }

  //
  // Exemple : mouvement de va et vient astable suivant x
  //
  static void buildExample2()
  {
    list[exemple2] = nodeIndependantAnimation;
    Anim<Transformation> & anim = nodeIndependantAnims[example2];
    anim.setAnim(5, 0);

    Transformation t;
    glLoadIdentity();        getFromModelView(t); anim.add(0, t);    anim.add(1000, t);
    glTranslatef(1.f, 0, 0); getFromModelView(t); anim.add(2000, t); anim.add(3000, t);
    glLoadIdentity();        getFromModelView(t); anim.add(4000, t);
  }
#endif

  // ==========================================================================

  id registerAnim(animFunc func)
  {
    for (int i = 0; i < animCount; i++)
    {
      if (list[i] == func) return i;
    }
    list[animCount] = func;
    return animCount++;
  }
}
