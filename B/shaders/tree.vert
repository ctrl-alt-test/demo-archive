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

  vec3 baseColor = vec3(0.25, 0.51, 0.);

  vec3 color1 = vec3(0., 0.71, 0.4);
  vec3 color2 = vec3(0.85, 0.98, 0.);
  vec3 color3 = vec3(0.89, 0., 0.3);

  int r = id / 256;
  vec3 seed = vec3(r, r + 1, r + 2);
  float w1 = smoothstep(0., 1., rand(seed.xy));
  float w2 = smoothstep(0., 1., rand(seed.yz));
  float w3 = smoothstep(0., 1., rand(seed.zx));
  float invW = 1. / (w1 + w2 + w3);

  vec3 endColor = (w1 * color1 + w2 * color2 + w3 * color3) * invW;

  int depth = id % 256;
  float coeff = clamp(0.08 * depth, 0., 1.);
  vColor = vec4(mix(baseColor, endColor, coeff), 1.);

  texCoord = gl_MultiTexCoord0.xy;
  vNormal = gl_NormalMatrix * gl_Normal;

  vViewVec = normalize(-vVertex.xyz);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = ftransform();
}
