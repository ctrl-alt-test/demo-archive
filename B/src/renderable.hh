//
// Renderable thing
//

#ifndef		RENDERABLE_HH
# define	RENDERABLE_HH

#include "animid.hh"
#include "basicTypes.hh"
#include "cube.hh"
#include "textureid.hh"
#include "shaderid.hh"
#include "vboid.hh"

class Renderable
{
public:
  Renderable(date deathDate,
	     Shader::id shaderId = Shader::default,
	     date birthDate = 0,
	     float size = 1.f,
	     unsigned int numberOfVertices = Cube::numberOfVertices,
	     VBO::id vboId = VBO::singleCube);
  ~Renderable();

  int		sortHint() const;
  bool		visible() const;
  void		setVisible(bool visible);
//   bool		transparent() const;
  unsigned int	numberOfVertices() const;

  int		id() const;
  void		setId(int id);

  Anim::id	animId() const;
  Texture::id	textureId() const;
  Texture::id	texture2Id() const;
  Texture::id	bumpTextureId() const;
  Texture::id	specularTextureId() const;
  Shader::id	shaderId() const;
  VBO::id	vboId() const;

  /*
  void		setTexture(Texture::id textureId);
  void		setBumpTexture(Texture::id bumpTextureId);
  void		setSpecularTexture(Texture::id specularTextureId);
  */
  void		setTextures(Texture::id textureId,
			    Texture::id bumpTextureId = Texture::defaultBump,
			    Texture::id specularTextureId = Texture::defaultSpecular);
  void		setTexture2(Texture::id texture2Id);


  float		size() const;
  float		distance() const;
  const float *	viewMatrix() const;
  date		birthDate() const;
  date		deathDate() const;
  void		setDeathDate(date deathDate);

  float		animationStepShift() const;
  date		animationStartDate() const;
  date		animationStopDate() const;

  void		setAnimation(Anim::id animId);
  void		setAnimation(Anim::id animId, date startDate);
  void		setAnimationStepShift(float stepShift);
  void		setAnimationStopDate(date stopDate);

  bool		operator > (const Renderable &) const;

private:
  int		_sortHint;

  bool		_visible;
//   bool		_transparent;

  int		_id;
  Anim::id	_animId;
  Shader::id	_shaderId;
  Texture::id	_textureId;
  Texture::id	_texture2Id;
  Texture::id	_bumpTextureId;
  Texture::id	_specularTextureId;
  VBO::id	_vboId;
  unsigned int	_numberOfVertices;
  float		_size;
  float		_distance;
  float		_viewMatrix[16];
  date		_birthDate;
  date		_deathDate;

  float		_animationStepShift;
  date		_animationStartDate;
  date		_animationStopDate;
};


#include "renderable.hxx"

#endif		// RENDERABLE_HH
