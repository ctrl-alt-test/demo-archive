//
// Renderable thing
//

#include "renderable.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "anims.hh"

Renderable::Renderable(date deathDate,
		       Shader::id shaderId,
		       date birthDate,
		       float size,
		       unsigned int numberOfVertices,
		       VBO::id vboId):
  _visible(true),
//   _transparent(false),
  _id(0),
  _shaderId(shaderId),
  _textureId(Texture::none),
  _texture2Id(Texture::none),
  _bumpTextureId(Texture::none),
  _specularTextureId(Texture::none),
  _vboId(vboId),
  _numberOfVertices(numberOfVertices),
  _size(size),
  _distance(0),
  _birthDate(birthDate),
  _deathDate(deathDate),
  _animId(Anim::none),
  _animationStartDate(birthDate),
  _animationStopDate(-1),
  _animationStepShift(0)
{
  assert(size >= 0.f);
  assert(_birthDate <= _deathDate);

  _sortHint = ((_shaderId & 0xff) << 16) | ((_vboId & 0xff) << 8);

  // Le renderable est créé avec la matrice de transformation en l'état
  glGetFloatv(GL_MODELVIEW_MATRIX, _viewMatrix);
}

Renderable::~Renderable()
{
}

// ============================================================================

/*
#define ACCESSOR(t, x) t Renderable::x() const {return _##x;}

ACCESSOR(int, sortHint);
ACCESSOR(bool, visible);

bool Renderable::operator > (const Renderable & other) const
{
  // FIXME : fonction de comparaison plus efficace
  assert(visible() && other.visible());

  // FIXME : ajouter la distance au sortHint
  return (_sortHint > other.sortHint());
}

void Renderable::setVisible(bool visible)
{
  _visible = visible;
}

ACCESSOR(bool, transparent);

//
// identifiants de l'objet, de textures, de shader, de VBO
//

ACCESSOR(int, id);
*/
void Renderable::setId(int id) { _id = id; }
/*
ACCESSOR(Anim::id, animId);
ACCESSOR(Texture::id, textureId);
ACCESSOR(Texture::id, bumpTextureId);
ACCESSOR(Texture::id, specularTextureId);
ACCESSOR(Shader::id, shaderId);
ACCESSOR(VBO::id, vboId);
*/
void Renderable::setAnimation(Anim::id animId)
{
  _animId = animId;
}

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

void Renderable::setAnimationStepShift(float stepShift)
{
  assert(stepShift >= 0.f); // Simple warning
  _animationStepShift = stepShift;
}

void Renderable::setAnimationStopDate(date stopDate)
{
  _animationStopDate = stopDate;
}

/*
void Renderable::setTexture(Texture::id textureId)
{
  _textureId = textureId;
}

void Renderable::setBumpTexture(Texture::id bumpTextureId)
{
  _bumpTextureId = bumpTextureId;
}

void Renderable::setSpecularTexture(Texture::id specularTextureId)
{
  _specularTextureId = specularTextureId;
}
*/
void Renderable::setTextures(Texture::id textureId,
			     Texture::id bumpTextureId,
			     Texture::id specularTextureId)
{
  _textureId = textureId;
  _texture2Id = textureId;
  _bumpTextureId = bumpTextureId;
  _specularTextureId = specularTextureId;

  _sortHint = (_sortHint & 0xffffff00) | (_textureId & 0xff);
}

/*
void Renderable::setTexture(Texture::id texture2Id)
{
  _texture2Id = texture2Id;
}
ACCESSOR(unsigned int, numberOfVertices);
ACCESSOR(float, size);

ACCESSOR(float, distance);

ACCESSOR(const float *, viewMatrix);

ACCESSOR(date, birthDate);
ACCESSOR(date, deathDate);
*/
void Renderable::setDeathDate(date deathDate)
{
  _deathDate = deathDate;
}
/*
ACCESSOR(float, animationStepShift);
ACCESSOR(date, animationStartDate);
ACCESSOR(date, animationStopDate);
*/
