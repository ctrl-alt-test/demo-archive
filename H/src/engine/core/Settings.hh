#ifndef SETTINGS_HH
#define SETTINGS_HH

#define ENABLE_QUALITY_OPTION 1 // SIZE: The option costs 100 bytes on the compressed binary.

namespace Core
{
	class Settings
	{
	public:
#if ENABLE_QUALITY_OPTION
		enum GraphicsQuality : int {
#if DEBUG
			RidiculouslyLow,
			VeryLow,
#endif // DEBUG
			Low,
			Medium,
			High,

			MaxQuality = High,
		};
#endif // ENABLE_QUALITY_OPTION

		Settings():
			renderWidth(1280),
			renderHeight(720),
			windowWidth(1280),
			windowHeight(720),
			displayAspectRatio(1280.f/720.f),
			fullscreen(true)
#if ENABLE_QUALITY_OPTION
			,
			m_textureSizeMultiplier(2),
			m_textureSizeDivider(1)
#endif // ENABLE_QUALITY_OPTION
		{
#if DEBUG
			fullscreen = false;
			SetQuality(Core::Settings::RidiculouslyLow);
#endif // DEBUG
		}

#if ENABLE_QUALITY_OPTION
		void SetQuality(GraphicsQuality quality);
		int TextureSizeAbout(int size) const;
#else // !ENABLE_QUALITY_OPTION
		int TextureSizeAbout(int size) const;
#endif // !ENABLE_QUALITY_OPTION

		int TextureSizeAtMost(int size) const;
		int TextureSizeAtLeast(int size) const;

		int				renderWidth;
		int				renderHeight;

		int				windowWidth;
		int				windowHeight;
		float			displayAspectRatio;

		bool			fullscreen;

#if ENABLE_QUALITY_OPTION
	private:
		int				m_textureSizeMultiplier;
		int				m_textureSizeDivider;
#endif // ENABLE_QUALITY_OPTION
	};
}

#endif // SETTINGS_HH
