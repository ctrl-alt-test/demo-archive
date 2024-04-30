#version 120

uniform float shininess;
uniform int time;
uniform sampler2D tex3; // material
uniform sampler2D tex4; // shadow
uniform vec4 ambient;
uniform float lightAtt[2];
uniform vec4 lightCol[2];

varying vec3 vLightVec[2];
varying vec4 vShadowCoord;

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nTangent;
vec3 nViewVec;

vec3 getHDRcolor(vec4 rgbe);
//]

//
// Couleur des lumières
//
vec3 getAmbientColor() { return getHDRcolor(ambient); }
vec3 getLightColor(int i) { return getHDRcolor(lightCol[i]); }


//
// Approximation de Schlick du coefficient de Fresnel
//
// f0 est le taux de réflexion pour un rayon incident à 90°, sa valeur
// va de 0.02 à 0.04 pour la plupart des matériaux, et 0.1 pour
// certains cristaux
//
float getFresnelTerm(vec3 h, float f0)
{
  return pow(1. - clamp(dot(h, nViewVec), 0., 1.), 5.) * (1. - f0) + f0;
}

//
// Intensité de la composante diffuse de l'éclairage
// (fonction de la normale et de la direction de la lumière, mais pas
// de la position de la caméra)
//
float getDiffuseIntensity(vec3 nLightVec)
{
  return clamp(dot(nLightVec, nNormal), 0., 1.);
}

//
// Intensité de la composante diffuse de l'éclairage,
// version anisotropique
//
float getAnisotropicDiffuseIntensity(vec3 nLightVec)
{
  // <L,N'> = sqrt(1 - <L,T>²)
  float LT = dot(nLightVec, nTangent);
  return sqrt(1. - LT * LT);
}

//
// Intensité de la composante spéculaire de l'éclairage
// (fonction de la normale, de la direction de la lumière et de la
// position de la caméra)
//

// Phong
/*
float getPhongSpecularIntensity(vec3 nLightVec)
{
  vec3 vReflect = reflect(-nLightVec, nNormal);
  float specular = pow(clamp(dot(vReflect, nViewVec), 0., 1.),
		       shininess) *
    (shininess + 2.) / 2. *
    texture2D(tex3, vTexCoord).a;

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
float getSpecularIntensity(vec3 nLightVec)
{
  vec3 vHalfVec = 0.5 * (nViewVec + nLightVec);
  float specular = pow(clamp(dot(normalize(vHalfVec), nNormal), 0., 1.),
		       shininess) *
    (shininess + 4.) / 8.;
  //   * texture2D(tex3, vTexCoord).a;

  return clamp(specular, 0., 1.);
}

//
// Intensité de la composante spéculaire de l'éclairage,
// version anisotropique
//
float getAnisotropicSpecularIntensity(vec3 nLightVec)
{
  // <V,R> = sqrt(1 - <L,T>²) * sqrt(1 - <V,T>²) - <L,T> * <V,T>
  vec3 vHalfVec = 0.5 * (nViewVec + nLightVec);
  float LT = dot(nLightVec, nTangent);
  float VT = dot(nViewVec, nTangent);
  float specular = pow(max(0., sqrt(1. - LT * LT) * sqrt(1. - VT * VT) - LT * VT),
		       shininess) *
    (2. + shininess);
  return 0.023 * specular;
}

//
// Facteur à appliquer à l'éclairage, fonction de la distance
//
float getDistanceAttenuation(int i)
{
  return 1. / (lightAtt[i] * dot(vLightVec[i], vLightVec[i]) + 1.);
}

//
// Facteur à appliquer à l'éclairage, fonction de l'ombre
//
float getShadowAttenuation()
{
  vec3 coord = vShadowCoord.xyz / vShadowCoord.w;
  if (vShadowCoord.w > 0. &&
      coord.xy == clamp(coord.xy, 0., 1.))
  {
    float falloff = clamp(20. * (0.95 - 2. * length(coord.xy - vec2(0.5))), 0., 1.);
    float shadowDist = texture2D(tex4, coord.xy).x;
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
  vec3 coord = vShadowCoord.xyz / vShadowCoord.w;
  if (vShadowCoord.w > 0. &&
      coord.xy == clamp(coord.xy, 0., 1.))
  {
    float falloff = 1.;//clamp(20. * (0.95 - 2. * length(coord.xy - vec2(0.5))), 0., 1.);
    // Read the moments from the variance shadow map.
    vec2 moments = texture2D(tex4, coord.xy).xy;
    // Compute the Chebyshev upper bound.
    return ChebyshevUpperBound(moments, coord.z) * falloff;
  }
  return 1.; // En dehors de la shadow map
}

//
// Composantes diffuse et spéculaire
//
// Fonction au choix selon le matériau
//
void getLightDiffuseAndSpecularBlinnPhong(int i, out vec3 diffuse, out vec3 specular)
{
  vec3 nLightVec = normalize(vLightVec[i]);
  vec3 vHalfVec = 0.5 * (nViewVec + nLightVec);

  float fresnel = getFresnelTerm(vHalfVec, 0.02);
  float diffuseIntensity = getDiffuseIntensity(nLightVec);
  float specularIntensity = getSpecularIntensity(nLightVec) * float(diffuseIntensity > 0.);

  float attenuation = getDistanceAttenuation(i);
  diffuse = (1. - fresnel) * diffuseIntensity * attenuation * getLightColor(i);
  specular = fresnel * specularIntensity * attenuation * getLightColor(i);
}

void getLightDiffuseAndSpecularAnisotropic(int i, out vec3 diffuse, out vec3 specular)
{
  vec3 nLightVec = normalize(vLightVec[i]);
  vec3 vHalfVec = 0.5 * (nViewVec + nLightVec);

  float fresnel = getFresnelTerm(vHalfVec, 0.02);
  float diffuseIntensity = getDiffuseIntensity(nLightVec);
  float specularIntensity = getAnisotropicSpecularIntensity(nLightVec) * float(diffuseIntensity > 0.);

  float attenuation = getDistanceAttenuation(i);
  diffuse = (1. - fresnel) * diffuseIntensity * attenuation * getLightColor(i);
  specular = fresnel * specularIntensity * attenuation * getLightColor(i);
}

void getLightDiffuseAndSpecularFullAnisotropic(int i, out vec3 diffuse, out vec3 specular)
{
  vec3 nLightVec = normalize(vLightVec[i]);
  vec3 vHalfVec = 0.5 * (nViewVec + nLightVec);

  float fresnel = getFresnelTerm(vHalfVec, 0.02);
  float diffuseIntensity = getAnisotropicDiffuseIntensity(nLightVec);
  float specularIntensity = getAnisotropicSpecularIntensity(nLightVec) * float(diffuseIntensity > 0.);

  float attenuation = getDistanceAttenuation(i);
  diffuse = (1. - fresnel) * diffuseIntensity * attenuation * getLightColor(i);
  specular = fresnel * specularIntensity * attenuation * getLightColor(i);
}

void getLightShadingBlinnPhongVSM(out vec3 diffuseComponent, out vec3 specularComponent)
{
  vec3 diffuse[2];
  vec3 specular[2];

  getLightDiffuseAndSpecularBlinnPhong(0, diffuse[0], specular[0]);
  getLightDiffuseAndSpecularBlinnPhong(1, diffuse[1], specular[1]);

  float shadowed = getVSMAttenuation();

  diffuseComponent = shadowed * diffuse[0] + diffuse[1] + getAmbientColor();
  specularComponent = shadowed * specular[0] + specular[1];
}

void getLightShadingAnisotropicPhongVSM(out vec3 diffuseComponent, out vec3 specularComponent)
{
  vec3 diffuse[2];
  vec3 specular[2];

  getLightDiffuseAndSpecularAnisotropic(0, diffuse[0], specular[0]);
  getLightDiffuseAndSpecularAnisotropic(1, diffuse[1], specular[1]);

  float shadowed = getVSMAttenuation();

  diffuseComponent = shadowed * diffuse[0] + diffuse[1] + getAmbientColor();
  specularComponent = shadowed * specular[0] + specular[1];
}

void getLightShadingFullAnisotropicPhongVSM(out vec3 diffuseComponent, out vec3 specularComponent)
{
  vec3 diffuse[2];
  vec3 specular[2];

  getLightDiffuseAndSpecularFullAnisotropic(0, diffuse[0], specular[0]);
  getLightDiffuseAndSpecularFullAnisotropic(1, diffuse[1], specular[1]);

  float shadowed = getVSMAttenuation();

  diffuseComponent = shadowed * diffuse[0] + diffuse[1] + getAmbientColor();
  specularComponent = shadowed * specular[0] + specular[1];
}
