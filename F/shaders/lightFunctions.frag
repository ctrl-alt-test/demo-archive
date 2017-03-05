uniform sampler2D mMap;
uniform sampler2D sMap;
uniform float shininess;
uniform int time;

varying vec4 shadowCoord;

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nTangent;
vec3 nViewVec;
//]

//
// Couleur des composantes ambiante, diffuse et spéculaire des lumières
//
vec4 getAmbientColor(int i) { return gl_LightSource[i].ambient; }
vec4 getDiffuseColor(int i) { return gl_LightSource[i].diffuse; }
vec4 getSpecularColor(int i){ return gl_LightSource[i].specular;}

//
// Intensité de la composante diffuse de l'éclairage
// (fonction de la normale et de la direction de la lumière, mais pas
// de la position de la caméra)
//
float getDiffuseIntensity(vec3 nLight)
{
  return clamp(dot(nLight, nNormal), 0., 1.);
}

//
// Approximation de Schlick du coefficient de Fresnel
//
// f0 est le taux de réflexion pour un rayon incident à 90°, sa valeur
// va de 0.02 à 0.04 pour la plupart des matériaux, et 0.1 pour
// certains cristaux
//
/*
float getFresnelTerm(vec3 h, float f0)
{
  return f0 + (1. - f0) * pow(1. - dot(h, nViewVec), 5.);
}
*/

//
// Intensité de la composante diffuse de l'éclairage,
// version anisotropique
//
float getAnisotropicDiffuseIntensity(vec3 nLight)
{
  // <L,N'> = sqrt(1 - <L,T>²)
  float LT = dot(nLight, nTangent);
  return sqrt(1. - LT * LT);
}

//
// Intensité de la composante spéculaire de l'éclairage
// (fonction de la normale, de la direction de la lumière et de la
// position de la caméra)
//

// Phong
/*
float getSpecularIntensity(vec3 nLight, float lightAttenuation)
{
  vec3 vReflect = reflect(-nLight, nNormal);
  float specular = pow(clamp(dot(vReflect, nViewVec), 0., 1.),
		       shininess) *
    (shininess + 2.) / 2. *
    lightAttenuation *
    texture2D(mMap, vTexCoord).a;

  return 0.2 * specular;
}

float getPhongSpecular(vec3 nLight)
{
  vec3 vReflect = reflect(-nLight, nNormal);
  float specular = pow(max(dot(vReflect, nViewVec), 0.), shininess) *
    (shininess + 2.) / 2.;

  return specular;
}
*/

// Blinn-Phong
//
// D'après Ryg, le facteur de normalisation est :
// (n+2)(n+4) / (8pi * (2^(-n/2) + n))
//
// Une approximation est :
// (n+4) / 8pi

float getSpecularIntensity(vec3 nLight, float lightAttenuation)
{
  vec3 vHalfVec = 0.5 * (nViewVec + nLight);
  float specular = pow(clamp(dot(normalize(vHalfVec), nNormal), 0., 1.),
		       shininess) *
    (shininess + 4.) / 8. *
    lightAttenuation *
    texture2D(mMap, vTexCoord).a;

  return /* getFresnelTerm(vHalfVec, 0.02) */ 0.2 * specular;
}
/*
float getBlinnPhongSpecular(vec3 nLight)
{
  vec3 nHalfVec = 0.5 * (nViewVec + nLight);
  float specular = pow(max(dot(nHalfVec, nNormal), 0.), shininess) *
    //(shininess+2) * (shininess+4) / (8 * (pow(2, -shininess/2) + shininess));
    (shininess + 4.) / 8.;
  return specular;
}

float getSpecularIntensity(vec3 nLight, float lightAttenuation)
{
  return (getBlinnPhongSpecular(nLight) *
	  getFresnelTerm(0.5 * (nViewVec + nLight)) *
// 	  getFresnelTerm(nNormal) *
	  lightAttenuation);
}
*/

//
// Intensité de la composante spéculaire de l'éclairage,
// version anisotropique
//
float getAnisotropicSpecularIntensity(vec3 nLight, float lightAttenuation)
{
  // <V,R> = sqrt(1 - <L,T>²) * sqrt(1 - <V,T>²) - <L,T> * <V,T>
  vec3 vHalfVec = 0.5 * (nViewVec + nLight);
  float LT = dot(nLight, nTangent);
  float VT = dot(nViewVec, nTangent);
  float specular = pow(max(0., sqrt(1. - LT * LT) * sqrt(1. - VT * VT) - LT * VT),
		       shininess) * lightAttenuation * (2. + shininess);
  return /* getFresnelTerm(vHalfVec, 0.5) */ 0.023 * specular;
}

//
// Facteur à appliquer à l'éclairage, fonction de la distance
//
float getDistanceAttenuation(int i, vec3 vLight)
{
  float dist = length(vLight);
  return clamp(1. - gl_LightSource[i].linearAttenuation * dist, 0., 1.);
}

//
// Facteur à appliquer à l'éclairage, fonction de l'ombre
//
float getShadowAttenuation()
{
  vec3 coord = shadowCoord.xyz / shadowCoord.w;
  if (shadowCoord.w > 0. &&
      coord.xy == clamp(coord.xy, 0., 1.))
  {
    float falloff = clamp(20. * (0.95 - 2. * length(coord.xy - vec2(0.5))), 0., 1.);
    float shadowDist = texture2D(sMap, coord.xy).x;
    return float(shadowDist >= coord.z) * falloff;
  }
  return 0.; // En dehors de la shadow map
}

float linstep(float vmin, float vmax, float v)
{
  return clamp((v - vmin) / (vmax - vmin), 0., 1.);
}

float ChebyshevUpperBound(vec2 moments, float t)
{
  // One-tailed inequality valid if t > Moments.x
  float p = float(t <= moments.x);
  // Compute variance.
  float minVariance = 0.000001;
  float variance = max(moments.y - (moments.x * moments.x), minVariance);
  // Compute probabilistic upper bound.
  float d = t - moments.x;
  float pmax = linstep(0.3, 1., variance / (variance + d * d));
  return max(p, pmax);
}

float getVSMAttenuation()
{
  if (time < 6000) // Gros hack : pas d'ombre pour la porte
    return 1.;

  vec3 coord = shadowCoord.xyz / shadowCoord.w;
  if (shadowCoord.w > 0. &&
      coord.xy == clamp(coord.xy, 0., 1.))
  {
    float falloff = clamp(20. * (0.95 - 2. * length(coord.xy - vec2(0.5))), 0., 1.);
    // Read the moments from the variance shadow map.
    vec2 moments = texture2D(sMap, coord.xy).xy;
    // Compute the Chebyshev upper bound.
    return ChebyshevUpperBound(moments, coord.z) * falloff;
  }
  return 0.; // En dehors de la shadow map
}
