// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 texCoord;

uniform int time;
uniform int id;

vec4 computeColor()
{
  float age = float(time - id);

  vec3 baseColor = vec3(0.44, 0.91, 0.00);
  vec3 color1 = vec3(0.00, 0.71, 0.40);
  vec3 color2 = vec3(0.85, 0.98, 0.00);
  vec3 color3 = vec3(0.89, 0.00, 0.30);

  float i = pow(sin(age * 0.01) * 0.5 + 0.5, 2.);
  float alpha = age * 0.001;
  float w1 = cos(alpha) * 0.5 + 0.5;
  float w2 = cos(alpha + 1.047) * 0.5 + 0.5;
  float w3 = cos(alpha + 2.094) * 0.5 + 0.5;
  float invW = 1. / (w1 + w2 + w3);

  vec3 endColor = (w1 * color1 + w2 * color2 + w3 * color3) * invW;
  vec3 color = mix(endColor, baseColor, i);

  float glow = mix(0.1, 0.85 * (1. - i), smoothstep(100., 1200., age));
  return vec4(color, glow);
}

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  vColor = computeColor();
  texCoord = gl_MultiTexCoord0.xy;
  vNormal = gl_NormalMatrix * gl_Normal;

  vViewVec = normalize(-vVertex.xyz);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = ftransform();
}
