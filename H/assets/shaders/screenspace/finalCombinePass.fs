#version 330

uniform float exposure;
uniform float saturation;
uniform float time;
uniform float vignetting;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform vec2 center;
uniform vec2 resolution;
uniform vec2 invResolution;
uniform vec3 bloomColor;
uniform vec3 gain;
uniform vec3 gamma;
uniform vec3 lift;
uniform vec3 streakColor;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

//# include "assets/shaders/debug/debugFunctions.fs"


vec3 Vignetting(vec3 color, vec2 uv, float intensity)
{
  vec2 coord = uv - center;

  // Comment for anamorphic lens:
  //coord.y *= invResolution.x / invResolution.y;

  // Vignetting is a function of cos4(theta)
  // http://toothwalker.org/optics/vignetting.html
  //
  // Identity: cos x = 1/sqrt(1 + tan2 x)
  //
  // Thus     cos4 x = 1/(1 + tan2 x)²
  //                 = 1/(1 + (d/l)²)²
  // This depends on fov. At 60°, 1/l is around 0.4
  // (16/9)² ~= 3.16

  // FIXME: find an equivalent formula with less precision loss at the center.
  float tan2 =  (intensity * intensity) * 3.16 * dot(coord, coord);
  return color / (1. + tan2 * tan2);
}

vec3 Uncharted2Tonemap(vec3 x)
{
  float A = 0.15; // Shoulder strength (0.22 ~ 0.15)
  float B = 0.50; // Linear strength (0.30 ~ 0.50)
  float C = 0.10; // Linear angle (0.10)
  float D = 0.20; // Toe strength (0.20)
  float E = 0.02; // Toe numerator (0.01 ~ 0.02) (E/F: Toe angle)
  float F = 0.30; // Toe denominator (0.30)
  return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 ToneMapping(vec3 color)
{
  float W = 11.2; // Linear white point value
  vec3 linearColor = Uncharted2Tonemap(color * exposure) / Uncharted2Tonemap(vec3(W));

  float gamma = 2.2;
  return pow(linearColor, vec3(1.0 / gamma));
}

vec3 ColorGrading(vec3 color, float saturation, vec3 lift, vec3 gamma, vec3 gain)
{
  vec3 bw = vec3(dot(clamp(color, 0., 1.), vec3(0.2126, 0.7152, 0.0722)));
  color = mix(bw, color, saturation);
  return pow(gain * (lift * (vec3(1.) - color) + color), vec3(1.) / gamma);
}

void main()
{
  vec3 color = (texture2D(texture0, texCoord).rgb +
		texture2D(texture1, texCoord).rgb * bloomColor +
		texture2D(texture2, texCoord).rgb * streakColor +
		texture2D(texture3, texCoord).rgb * streakColor);
  color = Vignetting(color, texCoord, vignetting);
  color = ToneMapping(color);
  color = ColorGrading(color, saturation, lift, gamma, gain);

  // Luma in the alpha channel for the FXAA pass
  float luma = dot(color, vec3(0.299, 0.587, 0.114));

  //color = debugFrameMarks(texCoord, color, 0.2);
  fragmentColor = vec4(color, luma);
}
