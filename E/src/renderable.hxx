//
// Renderable thing
//

#ifndef		RENDERABLE_HXX
# define	RENDERABLE_HXX

#include "animid.hh"
#include "sys/msys.h"

#if DEBUG
#include "anim.hh"
#include "anims.hh"
#endif

#define INLINE_ACCESSOR(t, x) inline t Renderable::x() const {return _##x;}

INLINE_ACCESSOR(int, sortHint);
INLINE_ACCESSOR(bool, visible);

inline
bool Renderable::operator > (const Renderable & other) const
{
  // FIXME : fonction de comparaison plus efficace
  assert(visible() && other.visible());

  // FIXME : ajouter la distance au sortHint
  return (_sortHint > other.sortHint());
}

inline
void Renderable::setVisible(bool visible)
{
  _visible = visible;
}

// INLINE_ACCESSOR(bool, transparent);

//
// identifiants de l'objet, de textures, de shader, de VBO
//

INLINE_ACCESSOR(int, id);
/*
inline void Renderable::setId(int id) { _id = id; }
*/
INLINE_ACCESSOR(Anim::id, animId);
INLINE_ACCESSOR(Texture::id, textureId);
INLINE_ACCESSOR(Texture::id, texture2Id);
INLINE_ACCESSOR(Texture::id, bumpTextureId);
INLINE_ACCESSOR(Texture::id, specularTextureId);
INLINE_ACCESSOR(Shader::id, shaderId);
INLINE_ACCESSOR(VBO::id, vboId);

/*

inline
void Renderable::setAnimation(Anim::id animId)
{
  _animId = animId;
}

inline
void Renderable::setAnimation(Anim::id animId,
			      date startDate)
{
  _animId = animId;
  _animationStartDate = startDate;

#if DEBUG
  if (animId != Anim::none)
  {
    Anim::Anim & animation = Anim::list[animId];
    assert(animation.size > 1); // Voir ^^^^^^
    const Anim::Keyframe & last = animation.last();
    assert(last.date() > 0.98f && last.date() < 1.02f);
  }
#endif // DEBUG

}


inline
void Renderable::setAnimationStepShift(float stepShift)
{
  assert(stepShift >= 0.f); // Simple warning
  _animationStepShift = stepShift;
}


inline
void Renderable::setAnimationStopDate(date stopDate)
{
  _animationStopDate = stopDate;
}


inline
void Renderable::setTexture(Texture::id textureId)
{
  _textureId = textureId;
}

inline
void Renderable::setBumpTexture(Texture::id bumpTextureId)
{
  _bumpTextureId = bumpTextureId;
}

inline
void Renderable::setSpecularTexture(Texture::id specularTextureId)
{
  _specularTextureId = specularTextureId;
}

inline
void Renderable::setTextures(Texture::id textureId,
			     Texture::id bumpTextureId,
			     Texture::id specularTextureId)
{
  setTexture(textureId);
  setBumpTexture(bumpTextureId);
  setSpecularTexture(specularTextureId);
}
*/

inline
void Renderable::setTexture2(Texture::id texture2Id)
{
  _texture2Id = texture2Id;
}

INLINE_ACCESSOR(unsigned int, numberOfVertices);
INLINE_ACCESSOR(float, size);

INLINE_ACCESSOR(float, distance);

INLINE_ACCESSOR(const float *, viewMatrix);

INLINE_ACCESSOR(const float *, oldModelView);

// FIXME : là on dumpe la matrice modelview avec la position de la
// caméra, ce qui n'est pas génial
inline
void Renderable::dumpOldModelView()
{
  glGetFloatv(GL_MODELVIEW_MATRIX, _oldModelView);
}

INLINE_ACCESSOR(date, birthDate);
INLINE_ACCESSOR(date, deathDate);

/*
inline
void Renderable::setDeathDate(date deathDate)
{
  _deathDate = deathDate;
}
*/

INLINE_ACCESSOR(float, animationStepShift);
INLINE_ACCESSOR(date, animationStartDate);
INLINE_ACCESSOR(date, animationStopDate);

#undef INLINE_ACCESSOR

#endif		// RENDERABLE_HXX
