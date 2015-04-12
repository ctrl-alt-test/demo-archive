// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 texCoord;

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  vColor = gl_Color;
  texCoord = gl_MultiTexCoord0.xy;
  vNormal = gl_NormalMatrix * gl_Normal;

  vViewVec = normalize(-vVertex.xyz);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = ftransform();
}
