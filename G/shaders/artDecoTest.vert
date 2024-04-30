#version 120

uniform float trans;
uniform int id;

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

  vertex.y *= clamp(trans * 4., 0., 1.);
  if (aId == 1.)
  {
    float progression = clamp(trans - 0.15, 0., 1.);
    if (progression <= 0.)
    {
      vertex.xyz = vec3(0.);
    }
    else
    {
      float wave = 0.5 + 0.5 * sin(vertex.x + progression) * sin(vertex.z + progression) * sin(0.25 * vertex.y - 20. * progression);
      vertex.xz = vertex.xz + sign(vertex.xz) * (clamp(progression + wave, 0., 1.) - 1.);
    }
  }
  if (aId == 2.)
  {
    float progression = clamp((trans - 0.4) /0.8, 0., 1.);
    if (progression <= 0.)
    {
      vertex.xyz = vec3(0.);
    }
    else
    {
      float wave = 0.5 + 0.5 * sin(vertex.x + progression) * sin(vertex.z + progression) * sin(0.25 * vertex.y - 20. * progression);
      vertex.xz = vertex.xz + sign(vertex.xz) * (clamp(progression + wave, 0., 1.) - 1.);
    }
  }

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
