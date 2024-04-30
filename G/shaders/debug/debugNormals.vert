#version 120

//
// Affiche une représentation des normales
//

varying vec3 vViewVec;
varying vec3 vNormal;
varying mat3 tanSpace;
//[
mat3 computeTBN();
//]

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;
  vNormal = normalize(gl_NormalMatrix * gl_Normal);

  tanSpace = computeTBN();
  vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;
  vViewVec = normalize(-vVertex * tanSpace);

  gl_Position = ftransform();
}
