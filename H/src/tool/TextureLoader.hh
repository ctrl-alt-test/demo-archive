//
// This is not part of the engine because it will generate code
// specific to each demo (include of generated list of textures).
//

#ifndef TEXTURE_LOADER_HH
#define TEXTURE_LOADER_HH

#include "engine/container/Array.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/Texture.hh"
#include "gfx/Geometry.hh"
#include "tool/IncludeUtils.hh"

#define MAX_TEXTURE_LAYERS	8

#if DEBUG
# define TEXTURE_EDIT 1
#else // !DEBUG
# define TEXTURE_EDIT 0
#endif // !DEBUG

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
/// <summary>
/// LOAD_TEXTURE stores in dest the Gfx::TextureID of the texture defined by
/// the builder passed as parameter.
/// In Debug it builds the texture;
/// in release it only gets the texture from a list of textures assumed to
/// have been built already.
/// </summary>
#define LOAD_TEXTURE_SIMPLE(loader, dest, name, width, height)	LOAD_TEXTURE_SIMPLE_IMPLEM(loader, dest, name, width, height)
#define LOAD_TEXTURE(loader, dest, name, width, height)			LOAD_TEXTURE_IMPLEM(loader, dest, name, width, height)

	struct TextureBuilderExport
	{
		const char*					builderExpression;
		const char*					widthExpression;
		const char*					heightExpression;
		const char*					builderName;
		const char*					loadLocation;
	};

	struct RecompilableTextureBuilder
	{
		texture::TextureBuilder		builder;
		int							width;
		int							height;
		int							numberOfLayers;
		const int*					layerSizes;

#if TEXTURE_EDIT
		TextureBuilderExport		exportInfo;

#ifdef ENABLE_RUNTIME_COMPILATION
		runtime::CompilationData	compilationData;
#endif // ENABLE_RUNTIME_COMPILATION
#endif // TEXTURE_EDIT
	};

#if TEXTURE_EDIT
# define LOAD_TEXTURE_SIMPLE_IMPLEM(loader, dest, name, width, height)	do { Tool::RecompilableTextureBuilder builder = RECOMPILABLE_TEXTURE_SIMPLE(name, width, height); (dest) = (loader).Load(builder); } while (false)
# define LOAD_TEXTURE_IMPLEM(loader, dest, name, width, height)			do { Tool::RecompilableTextureBuilder builder = RECOMPILABLE_TEXTURE(name, width, height); (dest) = (loader).Load(builder); } while (false)

#ifdef ENABLE_RUNTIME_COMPILATION
#define NON_RECOMPILABLE_TEXTURE							{ NULL, NULL, NULL, NULL, LOCATION_IN_SOURCE }, { NULL, InterfaceID(), NULL, ObjectId(), false }
#define RECOMPILABLE_TEXTURE_SIMPLE(name, width, height)	{ { name::BuildCaller, NULL,		}, (width), (height), name::s_numberOfLayers, name::s_layerSizes, { "{ " #name "_Builder, NULL }", #width, #height, #name, LOCATION_IN_SOURCE }, { #name, name::s_interfaceID, (void**)&name::instance, ObjectId(), true } }
#define RECOMPILABLE_TEXTURE(name, width, height)			{ { NULL, name::BuildPipeCaller,	}, (width), (height), name::s_numberOfLayers, name::s_layerSizes, { "{ NULL, " #name "_BuildPipe }", #width, #height, #name, LOCATION_IN_SOURCE }, { #name, name::s_interfaceID, (void**)&name::instance, ObjectId(), true } }
#else
#define NON_RECOMPILABLE_TEXTURE
#define RECOMPILABLE_TEXTURE_SIMPLE(name, width, height)	{ { name##_Builder, NULL,			}, (width), (height), name::s_numberOfLayers, name::s_layerSizes, "{ " #name "_Builder, NULL }", #width, #height, #name, LOCATION_IN_SOURCE }
#define RECOMPILABLE_TEXTURE(name, width, height)			{ { NULL, name##_BuildPipe,			}, (width), (height), name::s_numberOfLayers, name::s_layerSizes, "{ NULL, " #name "_BuildPipe }", #width, #height, #name, LOCATION_IN_SOURCE }
#endif

#else // !TEXTURE_EDIT
# include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_textureNames.hh)
# define LOAD_TEXTURE_SIMPLE_IMPLEM(loader, dest, name, width, height)	do { (dest) = (loader).GetTexture(::TextureName_##name); } while (false)
# define LOAD_TEXTURE_IMPLEM(loader, dest, name, width, height)			do { (dest) = (loader).GetTexture(::TextureName_##name); } while (false)
# define NON_RECOMPILABLE_TEXTURE
#endif // !TEXTURE_EDIT

#if TEXTURE_EDIT && defined(ENABLE_RUNTIME_COMPILATION)
	class TextureLoader : public IObjectFactoryListener
#else // !TEXTURE_EDIT || !ENABLE_RUNTIME_COMPILATION
	class TextureLoader
#endif // !TEXTURE_EDIT || !ENABLE_RUNTIME_COMPILATION
	{
	public:
		TextureLoader(Gfx::IGraphicLayer* gfxLayer,
					  runtime::RuntimeCompiler* runtimeCompiler);
		~TextureLoader();

#if TEXTURE_EDIT
		Gfx::TextureID* Load(RecompilableTextureBuilder& builder);
#else // !TEXTURE_EDIT

		/// <summary>
		/// Loads and compiles all the textures.
		/// </summary>
		void LoadTextures(const Core::Settings& settings);

		/// <summary>
		/// Loads and compiles the textures, one at a time.
		/// </summary>
		///
		/// <returns>True if there are textures left to compile, false
		/// otherwise.</returns>
		bool LoadTexturesIncrementally(int i, const Core::Settings& settings);

		Gfx::TextureID* GetTexture(const TextureName& name) const;
#endif // !TEXTURE_EDIT

#ifdef ENABLE_RUNTIME_COMPILATION
		void OnConstructorsAdded();
		void Rebuild();
#endif // ENABLE_RUNTIME_COMPILATION

		static void BuildTexture(Gfx::IGraphicLayer* gfxLayer,
								 RecompilableTextureBuilder& textureBuilder,
								 Gfx::TextureID* textureIDs);

	private:
		Gfx::IGraphicLayer*			m_gfxLayer;

#if TEXTURE_EDIT
#ifdef ENABLE_RUNTIME_COMPILATION
	public:
		runtime::RuntimeCompiler*	m_runtimeCompiler;
#endif // ENABLE_RUNTIME_COMPILATION

		Container::Array<RecompilableTextureBuilder> m_textureBuilders;
		Container::Array<Gfx::TextureID> m_textureIDs;
#endif // TEXTURE_EDIT
	};
}

#endif // TEXTURE_LOADER_HH
