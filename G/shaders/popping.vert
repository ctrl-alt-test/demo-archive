#version 120

uniform int birthDate;
uniform int id;
uniform int time;

varying vec3 vLightVec[2];
varying vec3 vViewVec;

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

vec4 animate(vec4 vertex, float t)
{
  float p = smoothstep(0., 250., t);

  float vibration =
    0.1 * sin(t / 75.) *
    min(smoothstep(0., 150., t),
        1. - smoothstep(150., 800., t));

  vec4 v = vertex;
  v.y *= vibration + p;
  return v;
}

void main()
{
  //  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  // Animation
  vec4 vertex = animate(gl_Vertex, float(time - birthDate));
  vec4 oldVertex = animate(gl_Vertex, float(time - birthDate) - 0.1);

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
  exportSpeed(oldVertex, vertex);
}
