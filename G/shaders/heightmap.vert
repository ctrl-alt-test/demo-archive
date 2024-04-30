#version 120

uniform int id;
uniform float yPos;
uniform float oldYPos;

varying vec3 vLightVec[2];
varying vec3 vViewVec;

attribute float aId;

//[
vec3 getDisplacement(vec2 uv);
vec3 getLightTangentSpace(int i, vec3 vVertex, mat3 tanSpace);
void exportCustomColor(float h, vec3 n);
void exportBaryCoords();
void exportDepth();
void exportFogFactor(vec3 vVertex);
void exportShadowCoord(vec4 vertex);
void exportSpeed(vec4 oldVertex, vec4 curVertex);
//]

float getShift()
{
  // Le +1 est là pour que la discontinuité due à la mise à jour de la texture soit sur la dernière ligne
  return floor(1. - yPos/8.) / 128.;
}

float getShiftFractional(float pos, float dp)
{
  return 8. * fract((pos - dp)/8.) + dp;
}

vec2 getHeightMapUV()
{
  vec2 uv = gl_TexCoord[0].st / gl_TexCoord[0].q;
  return vec2(1. - uv.x, uv.y);
}

void computeHeightMapTangents(out vec3 t, out vec3 b)
{
  float divs = 128.;
  float width = 256.;

  float i0 = mod(aId, divs) / divs;
  float j0 = floor(aId / divs) / divs;

  vec2 duv = vec2(1. / divs);

  vec2 uva = vec2(i0, j0);
  vec2 uvb = uva + vec2(duv.x, 0.);
  vec2 uvc = uva + vec2(0., duv.y);

  vec3 A = width * vec3(uva.x, 0., uva.y) + getDisplacement(uva);
  vec3 B = width * vec3(uvb.x, 0., uvb.y) + getDisplacement(uvb);
  vec3 C = width * vec3(uvc.x, 0., uvc.y) + getDisplacement(uvc);

  t = normalize(B - A);
  b = normalize(C - A);
}

mat3 computeTBN(vec3 tObjectSpace, vec3 bObjectSpace)
{
  vec3 t = gl_NormalMatrix * tObjectSpace;
  vec3 b = gl_NormalMatrix * bObjectSpace;
  vec3 n = cross(b, t);

  return mat3(t, b, n);
}

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vertex = gl_Vertex;
  vertex.xyz += getDisplacement(getHeightMapUV());
  vertex.z += getShiftFractional(yPos, 0.);

  vec4 oldVertex = gl_Vertex;
  oldVertex.xyz += getDisplacement(getHeightMapUV());
  oldVertex.z += getShiftFractional(oldYPos, oldYPos - yPos);

  vec3 t;
  vec3 b;
  computeHeightMapTangents(t, b);
  mat3 tanSpace = computeTBN(t, b);

  // Sommet dans l'espace caméra
  vec3 vVertex = (gl_ModelViewMatrix * vertex).xyz;

  // Vecteur incident dans l'espace tangent
  vLightVec[0] = getLightTangentSpace(0, vVertex, tanSpace);
  vLightVec[1] = getLightTangentSpace(1, vVertex, tanSpace);

  // Vecteur at dans l'espace tangent
  vViewVec = -vVertex * tanSpace;

  gl_Position = gl_ModelViewProjectionMatrix * vertex;

  exportCustomColor(vertex.y, cross(b, t));

  exportDepth();
  exportShadowCoord(vertex);
  exportFogFactor(vVertex);
  exportBaryCoords();
  exportSpeed(oldVertex, vertex);
}
