#version 330

uniform float time;
uniform sampler2D texture0;
uniform sampler2D randomTexture;
uniform vec2 resolution;
uniform vec2 invResolution;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

#include "assets/shaders/randFunctions.fs"

/*
#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define Q_PRESET 29     // Almost the highest setting
#define GREEN_AS_LUMA 0 // Luma defined in the FinalCombine pass
//*/
//#//include "assets/shaders/screenspace/fxaa.fs"
#include "assets/shaders/screenspace/fxaa_size_optimized.fs"

void main()
{
  /*
  vec4 reference = FXAA(texCoord, texture0, invResolution, 0.75, 0.166, 0.0625);
  //*/
  vec4 color = FXAA_WM(texCoord, texture0, invResolution,
		       // float fxaaQualitySubpix,
		       //   1.00 - upper limit (softer)
		       //   0.75 - default amount of filtering
		       //   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
		       //   0.25 - almost off
		       //   0.00 - completely off
		       0.75,
		       // float fxaaQualityEdgeThreshold,
		       //   0.333 - too little (faster)
		       //   0.250 - low quality
		       //   0.166 - default
		       //   0.125 - high quality
		       //   0.063 - overkill (slower)
		       0.166,
		       // float fxaaQualityEdgeThresholdMin
		       //   0.0833 - upper limit (default, the start of visible unfiltered edges)
		       //   0.0625 - high quality (faster)
		       //   0.0312 - visible limit (slower)
		       0.0625);

  // Dithering
  //
  // Even though we work in a R11G11B10 buffer, the image goes to a
  // RGB8 window. So we need noise to avoid banding. I originally
  // thought that for an 8 bits buffer, we needed +/- 0.5/256, but
  // when testing 10 times as much is the minimum to get no noticeable
  // banding anymore.
  //
  // References:
  // https://www.shadertoy.com/view/MlV3R1
  // https://www.shadertoy.com/view/ltBSRG
  // https://www.shadertoy.com/view/MslGR8
  // https://www.shadertoy.com/view/4ssXRX
  color += 10.*mix(-1./255., 1./255., blueNoise(randomTexture, resolution * texCoord, 120. * time));

  fragmentColor = color;
  /*
  if (fract(texCoord.x * 40.) > 0.5) fragmentColor = reference * vec4(0.9, 1., 0.9, 1.);
  if (texCoord.y > 0.5) fragmentColor = 100. * abs(reference - color);
  //*/
}
