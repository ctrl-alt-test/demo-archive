// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 texCoord;

uniform int id;

float rand(vec2 co)
{
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  vec3 color1 = vec3(0.44, 0.54, 0.94);
  vec3 color2 = vec3(0.53, 0.44, 0.94);
  vec3 color3 = vec3(0.42, 0.70, 0.93);
  /*
  vec3 color1 = vec3(0.05, 0.16, 0.6);
  vec3 color2 = vec3(0.16, 0.05, 0.61);
  vec3 color3 = vec3(0.03, 0.32, 0.57);
  */

  vec3 seed = vec3(id, sin(3. * id), sin(7. * id));
  float w1 = smoothstep(0., 1., rand(seed.xy));
  float w2 = smoothstep(0., 1., rand(seed.yz));
  float w3 = smoothstep(0., 1., rand(seed.zx));
  float invW = 1. / (w1 + w2 + w3);

  vec3 endColor = (w1 * color1 + w2 * color2 + w3 * color3) * invW;
  vColor = vec4(endColor, 1.);

  texCoord = gl_MultiTexCoord0.xy;
  vNormal = gl_NormalMatrix * gl_Normal;

  vViewVec = normalize(-vVertex.xyz);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = ftransform();
}
