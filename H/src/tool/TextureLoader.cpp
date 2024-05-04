#include "TextureLoader.hh"

#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "engine/core/FileIO.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"

#if TEXTURE_EDIT
#include <iostream>
#include <string>
#endif

#define ENABLE_SAVE_TEXTURES_TO_DISK 0

using namespace Tool;

#define MAX_BUILT_TEXTURES 1024

namespace LoadingBar { void update(void* = nullptr); }

#if TEXTURE_EDIT
void ExportTextureNames(const char* fileName, const char* macroName, const Container::Array<RecompilableTextureBuilder>& textureBuilders)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', texture ids won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "enum TextureName {\n");
		for (int i = 0; i < textureBuilders.size; ++i)
		{
			const RecompilableTextureBuilder& builder = textureBuilders[i];
			if (builder.exportInfo.builderName != nullptr)
			{
				fprintf(fp, "	TextureName_%s,\n", builder.exportInfo.builderName);
			}
		}
		fprintf(fp, "\n");

		fprintf(fp, "	NumberOfTextures,\n");
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}

void ExportTextureBuilders(const char* fileName, const char* macroName, const Container::Array<RecompilableTextureBuilder>& textureBuilders)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', texture definitions won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);

		fprintf(fp, "// Forward declarations:\n");
		for (int i = 0; i < textureBuilders.size; ++i)
		{
			const RecompilableTextureBuilder& builder = textureBuilders[i];
			if (builder.exportInfo.builderName != nullptr)
			{
				fprintf(fp, "void %s_BuildPipe(texture::Texture& dest);\n", builder.exportInfo.builderName);
				fprintf(fp, "void %s_Builder(void* out_pixelData, const texture::InputPixelInfo& in);\n", builder.exportInfo.builderName);
			}
		}
		fprintf(fp, "\n");

		fprintf(fp, "Gfx::TextureID textureIDs[%u];\n", textureBuilders.size * MAX_TEXTURE_LAYERS);
		fprintf(fp, "RecompilableTextureBuilder textureBuilders[] = {\n");
		for (int i = 0; i < textureBuilders.size; ++i)
		{
			const RecompilableTextureBuilder& builder = textureBuilders[i];
			if (builder.exportInfo.builderName != nullptr)
			{
				fprintf(fp, "	{ %s,	0,	0,	%d, NULL },\n",
					builder.exportInfo.builderExpression,
					builder.numberOfLayers);
			}
		}
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}

void ExportTextureSizes(const char* fileName, const Container::Array<RecompilableTextureBuilder>& textureBuilders)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', texture sizes won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#include \"engine/core/Settings.hh\"\n\n", __FUNCTION__, __FILE__, __LINE__);

		fprintf(fp, "// Forward declarations:\n");
		for (int i = 0; i < textureBuilders.size; ++i)
		{
			const RecompilableTextureBuilder& builder = textureBuilders[i];
			fprintf(fp, "extern const int* %s_layerSizes;\n", builder.exportInfo.builderName);
		}
		fprintf(fp, "\n");

		fprintf(fp, "void GetTextureSize(int i, const Core::Settings& settings, int* width, int* height)\n{\n	static const int sizes[] = {\n");
		for (int i = 0; i < textureBuilders.size; ++i)
		{
			const RecompilableTextureBuilder& builder = textureBuilders[i];
			fprintf(fp, "		%s, %s,\n", 
				builder.exportInfo.widthExpression,
				builder.exportInfo.heightExpression);
		}
		fprintf(fp, "	};\n	*width = sizes[i * 2];\n	*height = sizes[i * 2 + 1];\n}\n\n");

		fprintf(fp, "const int* GetTextureLayers(int i)\n{\n	const int* layersSizes[] = {\n");
		for (int i = 0; i < textureBuilders.size; ++i)
		{
			const RecompilableTextureBuilder& builder = textureBuilders[i];
			fprintf(fp, "		%s_layerSizes,\n", builder.exportInfo.builderName);
		}
		fprintf(fp, "	};\n	return layersSizes[i];\n}\n");
		fclose(fp);
	}
}
#endif // TEXTURE_EDIT

TextureLoader::TextureLoader(Gfx::IGraphicLayer* gfxLayer,
							 runtime::RuntimeCompiler* runtimeCompiler):
#if TEXTURE_EDIT
	m_gfxLayer(gfxLayer),
#ifdef ENABLE_RUNTIME_COMPILATION
	m_runtimeCompiler(runtimeCompiler),
#endif // ENABLE_RUNTIME_COMPILATION
	m_textureBuilders(MAX_BUILT_TEXTURES),
	m_textureIDs(MAX_BUILT_TEXTURES * MAX_TEXTURE_LAYERS)
#else // !TEXTURE_EDIT
	m_gfxLayer(gfxLayer)
#endif // !TEXTURE_EDIT
{
#if TEXTURE_EDIT && defined(ENABLE_RUNTIME_COMPILATION)
	runtimeCompiler->AddListener(this);
#else // !TEXTURE_EDIT || !ENABLE_RUNTIME_COMPILATION
	UNUSED_EXPR(runtimeCompiler);
#endif // !TEXTURE_EDIT || !ENABLE_RUNTIME_COMPILATION
}

TextureLoader::~TextureLoader()
{
#if TEXTURE_EDIT
	ExportTextureNames("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_textureNames.hh), "EXPORTED_TEXTURE_IDS", m_textureBuilders);
	ExportTextureBuilders("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_textureBuilders.hh), "EXPORTED_TEXTURE_BUILDERS", m_textureBuilders);
	ExportTextureSizes("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_textureSizes.cpp), m_textureBuilders);
#endif // TEXTURE_EDIT
}

#if TEXTURE_EDIT

#ifdef ENABLE_RUNTIME_COMPILATION
void TextureLoader::OnConstructorsAdded()
{
	Rebuild();
}

void TextureLoader::Rebuild()
{
	for (int i = 0; i < m_textureBuilders.size; i++)
	{
		RecompilableTextureBuilder& builder = m_textureBuilders[i];
		if (m_runtimeCompiler->UpdateCompiledCode(builder.compilationData))
		{
			BuildTexture(m_gfxLayer, builder, &m_textureIDs[i * MAX_TEXTURE_LAYERS]);
		}
	}
}
#endif // ENABLE_RUNTIME_COMPILATION

Gfx::TextureID* TextureLoader::Load(RecompilableTextureBuilder& builder)
{
	// Search for this builder in the list of textures already loaded.
	for (int i = 0; i < m_textureBuilders.size; ++i)
	{
		RecompilableTextureBuilder& test = m_textureBuilders[i];
		if (test.builder == builder.builder)
		{
			if (test.width != builder.width ||
				test.height != builder.height)
			{
				LOG_WARNING("Two textures %s declared with different sizes.", test.exportInfo.builderName);
				LOG_RAW("%s: See first declaration of %s.\n", test.exportInfo.loadLocation, test.exportInfo.builderName);
				LOG_RAW("%s: See conflicting declaration of %s.\n", builder.exportInfo.loadLocation, builder.exportInfo.builderName);
				if (builder.width > test.width)
				{
					test.width = builder.width;
					test.exportInfo.widthExpression = builder.exportInfo.widthExpression;
				}
				if (builder.height > test.height)
				{
					test.height = builder.height;
					test.exportInfo.heightExpression = builder.exportInfo.heightExpression;
				}
			}
			return &m_textureIDs[i * MAX_TEXTURE_LAYERS];
		}
	}

#ifdef ENABLE_RUNTIME_COMPILATION
	m_runtimeCompiler->LoadCompiledCode(builder.compilationData);
#endif // ENABLE_RUNTIME_COMPILATION

	int index = m_textureIDs.size;
	for (unsigned int i = 0; i < MAX_TEXTURE_LAYERS; ++i)
	{
		m_textureIDs.add(m_gfxLayer->CreateTexture());
	}
	m_textureBuilders.add(builder);

	BuildTexture(m_gfxLayer, m_textureBuilders.last(), &m_textureIDs[index]);
	LoadingBar::update();

	return &m_textureIDs[index];
}

#else // !TEXTURE_EDIT

#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_textureBuilders.hh)
#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_textureSizes.cpp)

// FIXME: when TEXTURE_EDIT is not defined, this function must be called
// before using the textures. Otherwise the program might crash on the
// first drawcall with little explanation of why. Ideally, we'd want
// LOAD_TEXTURE to report the error explicitely.
void TextureLoader::LoadTextures(const Core::Settings& settings)
{
	for (int i = 0; LoadTexturesIncrementally(i, settings); ++i)
	{
	}
}

bool TextureLoader::LoadTexturesIncrementally(int i, const Core::Settings& settings)
{
	if (i >= ARRAY_LEN(textureBuilders))
	{
		return false;
	}
	RecompilableTextureBuilder& builder = textureBuilders[i];
	GetTextureSize(i, settings, &builder.width, &builder.height);
	builder.layerSizes = GetTextureLayers(i);
	for (unsigned int j = 0; j < MAX_TEXTURE_LAYERS; ++j)
	{
		textureIDs[i * MAX_TEXTURE_LAYERS + j] = m_gfxLayer->CreateTexture();
	}
	BuildTexture(m_gfxLayer, builder, &textureIDs[i * MAX_TEXTURE_LAYERS]);
	return true;
}

Gfx::TextureID* TextureLoader::GetTexture(const TextureName& name) const
{
	return &textureIDs[name * MAX_TEXTURE_LAYERS];
}

#endif // !TEXTURE_EDIT

void TextureLoader::BuildTexture(Gfx::IGraphicLayer* gfxLayer,
								 RecompilableTextureBuilder& textureBuilder,
								 Gfx::TextureID* textureIDs)
{
#if TEXTURE_EDIT
	LOG_INFO("Building texture %s, %dx%d...",
		textureBuilder.exportInfo.builderName,
		textureBuilder.width, textureBuilder.height);

#ifdef ENABLE_RUNTIME_COMPILATION
	assert(textureBuilder.compilationData.hasCodeChanged);
	textureBuilder.compilationData.hasCodeChanged = false;
#endif // ENABLE_RUNTIME_COMPILATION
#endif // TEXTURE_EDIT

	int floatsPerPixel = 0;
	for (int i = 0; i < textureBuilder.numberOfLayers; ++i)
	{
		floatsPerPixel += textureBuilder.layerSizes[i];
	}

	// Ideally, we'd like a pool to reuse these instead of reallocating
	// for each texture.
	unsigned int* rawData = new unsigned int[textureBuilder.width * textureBuilder.height];
	texture::Texture texture(textureBuilder.width, textureBuilder.height, floatsPerPixel);

	textureBuilder.builder.Build(texture);

	// We could make a texture builder setting if necessary.
	Gfx::TextureSampling textureSampling = {
		Gfx::TextureFilter::LinearMipmapLinear,
		Gfx::TextureFilter::Linear,
		8.0f,
		Gfx::TextureWrap::Repeat,
		Gfx::TextureWrap::Repeat,
		Gfx::TextureWrap::Repeat,
	};

#if TEXTURE_EDIT && ENABLE_SAVE_TEXTURES_TO_DISK
	std::string directoryPath = Core::GetFullPath("build/assets/textures/");
	if (!Core::CreateDirectoryPath(directoryPath.c_str()))
	{
		LOG_ERROR("Could not create directory %s.", directoryPath.c_str());
	}
#endif // TEXTURE_EDIT && ENABLE_SAVE_TEXTURES_TO_DISK

	int firstChannel = 0;
	for (int i = 0; i < textureBuilder.numberOfLayers; ++i)
	{
#if TEXTURE_EDIT && ENABLE_SAVE_TEXTURES_TO_DISK
		std::string fileName = directoryPath +
			textureBuilder.builderName + std::string("_") +
			std::to_string((long long)i) + std::string(".tga");
		texture::ExportAsTARGA(texture, fileName.c_str(), firstChannel, textureBuilder.layerSizes[i]);
#endif // TEXTURE_EDIT && ENABLE_SAVE_TEXTURES_TO_DISK

		texture.Export(rawData, firstChannel, textureBuilder.layerSizes[i]);
		gfxLayer->LoadTexture(textureIDs[i],
			texture.Width(), texture.Height(),
			Gfx::TextureType::Texture2D, Gfx::TextureFormat::RGBA8,
			0, -1, (void*)rawData, textureSampling);
		firstChannel += textureBuilder.layerSizes[i];
	}

	delete[] rawData;
}
