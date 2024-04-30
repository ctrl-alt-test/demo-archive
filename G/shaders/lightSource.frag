#version 120

uniform int time;
uniform vec2 edgeParams;
uniform vec4 edgeRetroColor;
uniform vec4 faceRetroColor;
uniform vec4 retroParams;

varying float vFogFactor;
varying vec3 vSpeed;
varying vec4 vColor;

//[
vec2 vTexCoord;

float getEdge(float thickness, float alias, float flatness);
vec2 getTexCoord_no_bump();
vec3 addFog(vec3 color, float intensity);
vec3 getAlbedo_vColor_tex();
vec3 getSpeedColor(vec3 speed);
//]

void main()
{
  vTexCoord = getTexCoord_no_bump();

  float edge = getEdge(edgeParams.x, 0.42, edgeParams.y);
  vec3 albedo = getAlbedo_vColor_tex();

  vec3 color = addFog(albedo, vFogFactor);
  gl_FragData[0] = vec4(color, 1.);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
