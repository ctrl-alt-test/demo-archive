#include "Settings.hh"

#include "msys_temp.hh"

using namespace Core;

#if ENABLE_QUALITY_OPTION
void Settings::SetQuality(GraphicsQuality quality)
{
	m_textureSizeMultiplier = (quality > Medium ? 1 << (quality - Medium) : 1);
	m_textureSizeDivider = (quality < Medium ? 1 << (Medium - quality) : 1);
}

inline
int Settings::TextureSizeAbout(int size) const
{
	return msys_max(1, (size * m_textureSizeMultiplier) / m_textureSizeDivider);
}
#else // !ENABLE_QUALITY_OPTION
inline
int Settings::TextureSizeAbout(int size) const
{
	return size * 2;
}
#endif // !ENABLE_QUALITY_OPTION

int Settings::TextureSizeAtMost(int size) const
{
	return msys_min(size, TextureSizeAbout(size));
}

int Settings::TextureSizeAtLeast(int size) const
{
	return msys_max(size, TextureSizeAbout(size));
}
