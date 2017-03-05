uniform mat4 oldModelView;
uniform float zNear;
uniform float zFar;

attribute vec3 vTan;

varying vec3 vSpeed;
varying vec4 shadowCoord;

//
// Fog exp2
//
float getFogFactor(vec3 vVertex)
{
  gl_FogFragCoord = length(vVertex) / zFar;
  return clamp(exp2(gl_Fog.density * gl_Fog.density *
		    gl_FogFragCoord * gl_FogFragCoord *
		    -1.442695), // log2
	       0., 1.);
}

//
// Vecteur lumière incidente dans l'espace camera
//
vec3 getLightGlobal(int i, vec3 vVertex)
{
  return (gl_LightSource[i].position.xyz - vVertex * gl_LightSource[i].position.w);
}

//
// Vecteur lumière incidente dans l'espace tangent
//
vec3 getLightTangent(int i, vec3 vVertex, mat3 tanSpace)
{
  return (gl_LightSource[i].position.xyz - vVertex * gl_LightSource[i].position.w) * tanSpace;
}

//
// Représentation de la vitesse en un sommet
//
// - direction en xy,
// - intensité en z (avec une correction type gamma pour avoir de la
//   précision)
//
vec3 getSpeed()
{
  vec4 oldScreenCoord = gl_ProjectionMatrix * oldModelView * gl_Vertex;
  vec4 newScreenCoord = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
  return vec3(newScreenCoord.xy - oldScreenCoord.xy, newScreenCoord.w);
}

//
// Coordonnées de texture pour la shadow map
//
void computeShadowCoord()
{
  shadowCoord = gl_TextureMatrix[4] * gl_Vertex;
}

//
// Matrice TBN pour passer entre le repère global et le repère tangent
//
mat3 computeTBN()
{
  vec3 vNormal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 vTangent = normalize(gl_NormalMatrix * vTan);
  vec3 vBinormal = cross(vNormal, vTangent);

  return mat3(vTangent, vBinormal, vNormal);
}
