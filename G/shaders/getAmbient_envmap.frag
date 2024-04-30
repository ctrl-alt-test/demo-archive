#version 120

uniform mat4 cameraMatrix;
uniform sampler2D tex6;

vec3 nNormal;

//
// Lumière ambiante d'après envmap
//
vec3 getAmbient()
{
  float invPi = 1. / 3.14159265358979;

  vec3 wNormal = (vec4(nNormal, 0.) * cameraMatrix).xyz;
  vec2 uv = vec2(0.5 * invPi * atan(wNormal.x, wNormal.z),
		 invPi * atan(length(wNormal.xz), wNormal.y));
  return gl_LightSource[0].ambient.rgb * vec3(texture2D(tex6, uv));
}
