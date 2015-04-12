uniform sampler2D sMap;
uniform int time;

varying vec3 vViewVecSpec;
varying vec3 vLight;
//varying mat3 vLight;
varying float fogFactor;

vec2 vTexCoord;
vec3 nNormal;

vec4 getAlbedo();
vec4 getAmbient();

vec4 addFog(vec4 color)
{
  return mix(gl_Fog.color, color, mix(1., fogFactor, color.a));
//  return mix(gl_Fog.color, color, fogFactor);
}

vec4 getDiffuseColor(int i)
{
  // Hack :
  // On a perdu l'alpha, donc il est ici en dur
  return vec4(gl_LightSource[i].diffuse.rgb, 1.);
}

// Sans atténuation
/*
float getDiffuseIntensity()
{
  lightAttenuation = 1.;
  nLight = normalize(vLight);
  return clamp(dot(nLight, nNormal), 0., 1.);
}
*/

// Avec atténuation
float getDiffuseIntensity(vec3 nLight, float lightAttenuation)
{
  return lightAttenuation * clamp(dot(nLight, nNormal), 0., 1.);
}

vec4 getDiffuse(int i, float diffuse)
{
  vec4 diffuseColor = getDiffuseColor(i);
  return vec4(diffuse * diffuseColor.rgb, 1. - diffuse * (1. - diffuseColor.a));
}

vec4 getSpecularColor(int i)
{
  return gl_LightSource[i].specular;
}

float getSpecularIntensity(vec3 nLight, float lightAttenuation, float diffuse)
{
  float branchless = float(diffuse > 0.);
  vec3 vReflect = reflect(-nLight, nNormal);
  float shininess = 20.;
  vec3 nViewVec = normalize(vViewVecSpec);
  float specular = pow(clamp(lightAttenuation * dot(vReflect, nViewVec), 0., 1.),
		       shininess) * texture2D(sMap, vTexCoord).a;
  return branchless * specular;
}

vec4 getSpecular(vec3 nLight, float lightAttenuation, float diffuse)
{
  float specular = getSpecularIntensity(nLight, lightAttenuation, diffuse);
  vec4 specularColor = getSpecularColor(0);
  return vec4(specular * specularColor.rgb, 1. - 3.5 * specular * (1. - specularColor.a));
}

vec4 getSpecular(int i, mat3 nLights, vec3 lightAttenuations, float diffuse)
{
  float specular = getSpecularIntensity(nLights[i], lightAttenuations[i], diffuse);
  vec4 specularColor = getSpecularColor(i);
  return vec4(specular * specularColor.rgb, 1. - 3.5 * specular * (1. - specularColor.a));
}

// Eclairage de Lambert
vec4 getBump()
{
  vec4 albedo = getAlbedo();

  vec4 vAmbient = getAmbient();

  float linearAttenuation = gl_LightSource[0].linearAttenuation;
  float dist = length(vLight);
  float lightAttenuation = clamp(1. - linearAttenuation * dist, 0., 1.);
  vec3 nLight = normalize(vLight);
  float diffuse0 = getDiffuseIntensity(nLight, lightAttenuation);
  vec4 vDiffuse = getDiffuse(0, diffuse0);
  vec4 vSpecular = getSpecular(nLight, lightAttenuation, diffuse0);

  /*
  vec3 linearAttenuations = vec3(gl_LightSource[0].linearAttenuation,
				 gl_LightSource[1].linearAttenuation,
				 gl_LightSource[2].linearAttenuation);

  vec3 dists = vec3(length(vLight[0]),
		    length(vLight[1]),
		    length(vLight[2]));
  vec3 lightAttenuations = clamp(1. - linearAttenuations * dists, 0., 1.);

  mat3 nLights = mat3(normalize(vLight[0]),
		      normalize(vLight[1]),
		      normalize(vLight[2]));

  float diffuse0 = getDiffuseIntensity(nLights[0], lightAttenuations[0]);
  float diffuse1 = getDiffuseIntensity(nLights[1], lightAttenuations[1]);
  float diffuse2 = getDiffuseIntensity(nLights[2], lightAttenuations[2]);

  vec4 vDiffuse = (getDiffuse(0, diffuse0) +
		   getDiffuse(1, diffuse1) +
		   getDiffuse(2, diffuse2));
  vec4 vSpecular = //vec4(0., 0., 0., 1.);
                   (getSpecular(0, nLights, lightAttenuations, diffuse0) +
		    getSpecular(1, nLights, lightAttenuations, diffuse1) +
		    getSpecular(2, nLights, lightAttenuations, diffuse2));
  */

  vec4 resultLighting = vec4(vAmbient.rgb + vDiffuse.rgb, min(vAmbient.a, vDiffuse.a));
  vec3 diffuseComponent = albedo.rgb * mix(vec3(1.), resultLighting.rgb, albedo.a);

  vec3 color = diffuseComponent + vSpecular.rgb;
  float glow = min(min(albedo.a, resultLighting.a), vSpecular.a);

  return vec4(color, glow);
}

// Pas d'éclairage
/*
vec4 getBump()
{
  return getAlbedo();
}
*/
