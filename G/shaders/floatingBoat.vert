#version 120

uniform int id;
uniform int time;

varying vec3 vLightVec[2];
varying vec3 vViewVec;
varying vec3 vvPos;

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

#define PI 3.1415926535

vec3 GerstnerWave(float t, vec2 Xo, float A, vec2 uk)
{
  //
  // Gerstner
  //
  // Xo = (xo, yo) : position initiale
  // A : amplitude
  // K : vecteur direction de la vague
  // k = 2pi/lambda : amplitude de K
  //
  // x = Xo - sum(Ki/ki * Ai * sin(dot(Ki, Xo) - wi * t + phii))
  // y =      sum(        Ai * cos(dot(Ki, Xo) - wi * t + phii))
  //
  // Relation entre frequence et amplitude :
  // w²(K) = g * k  (g = 9.801 m/s²)
  //
  // Relation quand la profondeur D est faible :
  // w²(K) = g * k * tanh(k * D)
  //
  float lambda = A / 20.;
  vec2 k = uk * 2. * PI / lambda;
  float w = sqrt(9.801 * 2. * PI / lambda) * 0.2;

  float x = -uk.x * A * sin(dot(k, Xo) - w * t);
  float z = -uk.y * A * sin(dot(k, Xo) - w * t);
  float y =         A * cos(dot(k, Xo) - w * t);

  return vec3(x, y, z);
}

void main()
{
  float t = 0.001 * float(time);
  float A = 3.;
  vec2 uk = vec2(-0.961,  0.276);
  vec3 oldWave = 0.7*GerstnerWave(t - 0.1, vec2(0.51, 0.88), A, uk);
  vec3 wave = 0.7*GerstnerWave(t, vec2(0.51, 0.88), A, uk);

  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vertex = gl_Vertex;
  vertex.xyz += wave;
  vec4 oldVertex = gl_Vertex;
  oldVertex.xyz += oldWave;

  mat3 tanSpace = computeTBN();

  // Sommet dans l'espace caméra
  vec3 vVertex = (gl_ModelViewMatrix * vertex).xyz;
  vvPos = vertex.xyz;

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
