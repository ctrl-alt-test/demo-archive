// -*- glsl -*-

//
// Ce shader est celui de la sphère
//
// Une belle sphère rouge plastique
//

varying vec3 vViewVec;
varying vec3 vLight;
varying vec3 vNormal;

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  vNormal = gl_NormalMatrix * gl_Normal;

  vViewVec = normalize(-vVertex.xyz);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = ftransform();
}
