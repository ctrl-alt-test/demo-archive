#version 120

uniform mat4 oldModelView;
uniform float zNear;
uniform float zFar;
uniform vec4 lightPos[2];
uniform float fogDensity;

attribute vec3 aTan;
attribute vec4 aCol;
attribute vec4 aBary;

varying vec4 vColor;
varying vec4 vBaryCoord;
varying vec3 vSpeed;
varying vec2 vDepth;
varying vec4 vShadowCoord;
varying float vFogFactor;

//
// Matrice TBN pour passer entre le repère objet et le repère tangent
//
mat3 computeTBN()
{
  vec3 vNormal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 vTangent = normalize(gl_NormalMatrix * aTan);
  vec3 vBinormal = cross(vNormal, vTangent);

  return mat3(vTangent, vBinormal, vNormal);
}

//
// Vecteur lumière incidente dans l'espace objet
//
vec3 getLightObjectSpace(int i, vec3 vVertex)
{
  // lightPos est la position de la lumière en espace objet
  return (lightPos[i].xyz - lightPos[i].w * vVertex);
}

//
// Vecteur lumière incidente dans l'espace tangent
//
vec3 getLightTangentSpace(int i, vec3 vVertex, mat3 tanSpace)
{
  return getLightObjectSpace(i, vVertex) * tanSpace;
}

//
// Représentation de la vitesse en un sommet
//
// - direction en xy,
// - z pour les coordonnées homogènes
//
void exportSpeed(vec4 oldVertex, vec4 curVertex)
{
  vec4 old = gl_ProjectionMatrix * (oldModelView * oldVertex);
  vec4 cur = gl_ProjectionMatrix * (gl_ModelViewMatrix * curVertex);

  vSpeed = vec3(cur.xy * old.w - old.xy * cur.w, old.w * cur.w);
}

//
// Z en coordonnées homogènes pour la shadow map
//
void exportDepth()
{
  vDepth = gl_Position.zw;
}

//
// Coordonnées de texture pour la shadow map
//
void exportShadowCoord(vec4 vertex)
{
  vShadowCoord = gl_TextureMatrix[4] * vertex;
}

//
// Fog exp2
//
void exportFogFactor(vec3 vVertex)
{
  gl_FogFragCoord = length(vVertex) / zFar;
  gl_FogFragCoord = clamp(gl_FogFragCoord, 0., 1.);
  vFogFactor = pow(1. - gl_FogFragCoord, fogDensity);
  // vFogFactor = 1.;
  // gl_FogFragCoord = exp(-gl_FogFragCoord * gl_Fog.density);
  // vFogFactor = clamp(exp2(gl_Fog.density * gl_Fog.density *
  //       		 gl_FogFragCoord * gl_FogFragCoord *
  //       		 -1.442695), // log2
  //       	    0., 1.);
}

//
// Coordonnées barycentriques
//
void exportBaryCoords()
{
  vBaryCoord = aBary;
}

void exportColor()
{
  vColor = aCol;
}
