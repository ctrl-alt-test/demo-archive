#version 120

uniform vec4 skyCol;
uniform vec4 fogCol;
varying vec3 viewVec;

vec3 nullSpeedColor()
{
  return vec3(0.5, 0.5, 0.);
}

void main()
{
  float lat = clamp(normalize(viewVec).y, 0., 1.);
  float transition = pow(smoothstep(0.02, .5, lat), 0.4);

  gl_FragData[0] = vec4(mix(fogCol.rgb, skyCol.rgb, transition), 1.);
  gl_FragData[1] = vec4(nullSpeedColor(), 1.);
}
