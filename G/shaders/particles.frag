#version 120

varying float vFogFactor;
varying vec3 vSpeed;
varying vec4 vColor;

//[
vec3 addFog(vec3 color, float intensity);
vec3 getSpeedColor(vec3 speed);
//]

void main()
{
  vec3 color = vColor.rgb;

  gl_FragData[0] = vec4(addFog(color, vFogFactor), 1.);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
