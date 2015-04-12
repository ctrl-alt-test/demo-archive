// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec3 vTangent;
varying vec2 texCoord;
varying mat3 tanSpace;

varying vec3 vPos;

attribute vec3 vTan;

uniform int time;

const int forestStart = 241050;
const int forestStartEffect = 245050;

float circleHeight(vec2 pos)
{
  float dist = distance(pos, vec2(50., 10.));
  float t = 0.015 * (time - forestStartEffect);
  float intensity = clamp(1. - 0.1 * abs(2. * t - dist), 0., 1.);

  return mix(0., intensity * (1. + sin(0.7 * dist - t)), min(0.1 * dist, 1.));
}

float growingHeight(float height)
{
  float intensity = 1. - 0.00025 * (time - forestStartEffect - 4000);
  return clamp(height - 5.5 * intensity * intensity * intensity, 0., height);
}

void main(void)
{
  vec4 pos = gl_Vertex;
  vPos = pos.xyz;

  if (time > forestStart && time < forestStartEffect + 8000)
  {
    if (time < forestStartEffect)
      pos.y = 0.;
    if (time >= forestStartEffect && time < forestStartEffect + 4000)
      pos.y = circleHeight(pos.xz);
    if (time >= forestStartEffect + 3000)
	pos.y = growingHeight(pos.y);
  }
  vec4 vVertex = gl_ModelViewMatrix * pos;

  vColor = gl_Color;
  texCoord = gl_MultiTexCoord0.xy;

  vNormal = gl_NormalMatrix * gl_Normal;
  vTangent = normalize(gl_NormalMatrix * vTan);
  vec3 vBinormal = cross(vNormal, vTangent);
  tanSpace = mat3(vTangent, vBinormal, vNormal);

  // Vecteur incident dans l'espace tangent
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  // Vecteur at dans l'espace tangent
  vViewVec = tanSpace * -vVertex.xyz;

  gl_Position = gl_ModelViewProjectionMatrix * pos;
}
