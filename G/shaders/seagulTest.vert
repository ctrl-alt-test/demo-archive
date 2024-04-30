#version 120

uniform int id;
uniform sampler2D tex6;

varying vec3 vLightVec[2];
varying vec3 vViewVec;

attribute float aId;
attribute vec3 aTan;

//[
vec3 getLightTangentSpace(int i, vec3 vVertex, mat3 tanSpace);
void exportColor();
void exportBaryCoords();
void exportDepth();
void exportFogFactor(vec3 vVertex);
void exportShadowCoord(vec4 vertex);
void exportSpeed(vec4 oldVertex, vec4 curVertex);
//]

mat3 computeTBN(mat3 normalMatrix)
{
  vec3 vNormal = normalize(normalMatrix * gl_Normal);
  vec3 vTangent = normalize(normalMatrix * aTan);
  vec3 vBinormal = cross(vNormal, vTangent);

  return mat3(vTangent, vBinormal, vNormal);
}

void main()
{
  //  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  // FIXME : recuperer la matrice, et multiplier par vertex
  vec4 vx = texture2D(tex6, vec2(0.5/4., (aId + 0.5) / 4000.));
  vec4 vy = texture2D(tex6, vec2(1.5/4., (aId + 0.5) / 4000.));
  vec4 vz = texture2D(tex6, vec2(2.5/4., (aId + 0.5) / 4000.));
  vec4 vt = texture2D(tex6, vec2(3.5/4., (aId + 0.5) / 4000.));
  mat4 m = mat4(vx, vy, vz, vt);
  vec4 vertex = m * gl_Vertex;

  mat3 tanSpace = computeTBN(mat3(vx.xyz, vy.xyz, vz.xyz));

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
