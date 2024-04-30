#version 120

uniform int id;
uniform int time;

varying vec3 vLightVec[2];
varying vec3 vViewVec;

attribute float aId;

//[
mat3 computeTBN();
vec3 getLightTangentSpace(int i, vec3 vVertex, mat3 tanSpace);
void exportColor();
void exportBaryCoords();
void exportDepth();
void exportFogFactor(vec3 vVertex);
void exportShadowCoord(vec4 vertex);
void exportSpeed(vec4 oldVertex, vec4 curVertex);
//]

void main()
{
  //  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vertex = gl_Vertex;
  float startGrow = floor(aId);
  float growDuration = 10000. * (aId - startGrow);
  float progression = smoothstep(startGrow, startGrow + growDuration, float(time));
/*   float growSpeed = 15.; */
/*   float progression = clamp((1. + growSpeed)*fract(0.0001*float(time)) - growSpeed * aId, 0., 1.); */
  vertex.y = mix(-0.1, vertex.y, progression);

  mat3 tanSpace = computeTBN();

  // Sommet dans l'espace caméra
  vec3 vVertex = (gl_ModelViewMatrix * vertex).xyz;

  // Vecteur incident dans l'espace tangent
  vLightVec[0] = getLightTangentSpace(0, vVertex, tanSpace);
  vLightVec[1] = getLightTangentSpace(1, vVertex, tanSpace);

  // Vecteur at dans l'espace tangent
  vViewVec = -vVertex * tanSpace;

  gl_Position = gl_ModelViewProjectionMatrix * vertex;

  exportColor();
  exportDepth();
  exportShadowCoord(vertex);
  exportFogFactor(vVertex);
  exportBaryCoords();
  exportSpeed(vertex, vertex);
}
