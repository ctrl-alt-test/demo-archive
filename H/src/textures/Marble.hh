#pragma once

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;

	struct tilePattern
	{
		float				d;
		float				id;
		int					row;
		int					column;
		Algebra::vector2f	uv;
	};

	tilePattern interleavedTiles(Algebra::vector2f p, int columns, int rows, float offset);
	Algebra::vector3f whiteMarble(const Algebra::vector2f& uv);
	Algebra::vector3f redMarble(const Algebra::vector2f& uv);
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct FineWhiteMarbleWall : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct FineWhiteMarbleWall
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static FineWhiteMarbleWall* instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						FineWhiteMarbleWall_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct FineRedAndWhiteMarbleFloor : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct FineRedAndWhiteMarbleFloor
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static FineRedAndWhiteMarbleFloor* instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						FineRedAndWhiteMarbleFloor_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct GreetingsMarbleFloor : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct GreetingsMarbleFloor
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static GreetingsMarbleFloor* instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						GreetingsMarbleFloor_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct Gold : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct Gold
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static Gold*			instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						Gold_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
