// Pour le tableau
#version 120

uniform float trans;
uniform int time;
uniform int id;

varying vec3 vViewVec;
varying vec3 vSpeed;
varying float fogFactor;

//[
vec2 getTexCoord();
vec3 getNormal();
vec2 vTexCoord;
vec3 nNormal;
vec3 nViewVec;

vec4 getBump();
vec4 addFog(vec4 color, float intensity);

vec3 getSpeedColor(vec3 speed);
//]

const vec3 colors[12] = vec3[12](
  vec3(0.98, 0.61, 0.68),  // Rose f981ad
  vec3(0.80, 0.11, 0.35),  // Fuschia #cb1d58
  vec3(0.95, 0.22, 0.16),  // Rouge #f3392a
  vec3(1.0, 0.41, 0.05),   // Orange #fe690c
  vec3(0.98, 0.93, 0.22),  // Jaune #faec38
  vec3(0.11, 0.79, 0.36),  // Vert clair #1dc95d
  vec3(0.07, 0.45, 0.39),  // Vert foncé #117464
  vec3(0.28, 0.77, 0.96),  // Bleu clair #48c4f4
  vec3(0.09, 0.55, 0.91),  // Bleu roi #188be9
  vec3(0.11, 0.22, 0.55),  // Bleu marine #1c378d
  vec3(0.26, 0.18, 0.43),  // Violet #422d6e
  vec3(1., 1., 1.));       // Blanc

vec4 getAlbedo_tex()
{
  float glow = sin(float(time + id) / 200.f) * 0.125;
  glow = float(step(165900., float(time))) * (0.7 * trans - glow);
  int i = id - 12 * (id / 12);
  return vec4(colors[i], 1. - glow);
}

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  vec4 color = getBump();
  color = addFog(color, fogFactor);

  gl_FragData[0] = color;
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
