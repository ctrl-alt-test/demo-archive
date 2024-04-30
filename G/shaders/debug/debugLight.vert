#version 120

//
// Affiche une représentation de l'éclairage
//

varying vec3 vLightVec[2];
varying vec3 vViewVec;
//varying vec4 vColor;

//[
mat3 computeTBN();
vec3 getLightTangentSpace(int i, vec3 vVertex, mat3 tanSpace);
void exportShadowCoord(vec4 vertex);
//]

void main()
{
  //  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  mat3 tanSpace = computeTBN();
  vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;

  // Vecteur incident dans l'espace tangent
  vLightVec[0] = getLightTangentSpace(0, vVertex, tanSpace);
  vLightVec[1] = getLightTangentSpace(1, vVertex, tanSpace);

  // Vecteur at
  vViewVec = -vVertex * tanSpace;

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  exportShadowCoord(gl_Vertex);
}
