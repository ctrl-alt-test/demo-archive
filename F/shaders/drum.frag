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
  vec3(0.9, 0.2, 0.2),
  vec3(0.8, 0.5, 0.2),
  vec3(0.8, 0.8, 0.2),
  vec3(0.2, 0.9, 0.2),
  vec3(0.2, 0.8, 0.5),
  vec3(0.2, 0.8, 0.8),
  vec3(0.2, 0.5, 0.8),
  vec3(0.2, 0.2, 0.9),
  vec3(0.5, 0.2, 0.8),
  vec3(0.8, 0.2, 0.8),
  vec3(0.5, 0.5, 0.8),
  vec3(0.5, 0.5, 0.5));

vec4 getAlbedo_tex()
{
  if (time < 165900 || time > 216000) return vec4(1.f);

  vec2 p = vTexCoord;
  float len = length(p) - trans;

  float amp = min(smoothstep(0., 0.1, len),
                  smoothstep(0.1, 0., len));
  amp *= pow(1. - length(p), 2.);
  vec4 col;
  col.rgb = vec3(1.);
  col.a = mix(1., 0., amp);
  return col;
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
