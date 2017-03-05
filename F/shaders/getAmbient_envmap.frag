uniform sampler2D aMap;
uniform mat4 cameraMatrix;

vec3 nNormal;

//
// Lumière ambiante d'après envmap
//
vec4 getAmbient()
{
  float invPi = 1. / 3.14159265358979;

  vec3 wNormal = (vec4(nNormal, 0.) * cameraMatrix).xyz;
  vec2 uv = vec2(0.5 * invPi * atan(wNormal.x, wNormal.z),
		 invPi * atan(length(wNormal.xz), wNormal.y));
  return gl_LightSource[0].ambient * vec4(vec3(texture2D(aMap, uv)), 1.);
}
