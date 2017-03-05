//
// Affiche une représentation de l'éclairage
//

varying vec3 vViewVec;
varying vec3 vLight0;
varying vec3 vLight1;
//varying vec4 vColor;

//[
vec3 getLightTangent(int i, vec3 vVertex, mat3 tanSpace);
mat3 computeTBN();
void computeShadowCoord();
//]

void main()
{
  //  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  mat3 tanSpace = computeTBN();
  vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;

  computeShadowCoord();

  // Vecteur incident dans l'espace tangent
  vLight0 = getLightTangent(0, vVertex, tanSpace);
  vLight1 = getLightTangent(1, vVertex, tanSpace);

  // Vecteur at
  vViewVec = -vVertex * tanSpace;

  gl_Position = ftransform();
}
