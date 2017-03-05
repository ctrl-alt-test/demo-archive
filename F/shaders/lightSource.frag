
varying vec3 vSpeed;
varying float fogFactor;

vec2 vTexCoord;

//[
vec2 getTexCoord_no_bump();
vec4 getAlbedo_tex();
vec3 getSpeedColor(vec3 speed);
vec4 addFog(vec4 color, float intensity);
//]

void main()
{
  vTexCoord = getTexCoord_no_bump();

  vec4 color = getAlbedo_tex();
  color = addFog(color, fogFactor);

  gl_FragData[0] = color;
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
