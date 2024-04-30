#version 120

uniform vec4 lightColor[2];

varying vec3 vLightVec[2];
varying vec3 vNormal;

//[
vec2 vTexCoord;
vec3 nNormal;

vec2 getTexCoord();
vec3 getAlbedo_tex();
vec3 getNormal();
//]

vec3 getLightColor(int i)
{
  return lightColor[i].rgb;
}

// Avec atténuation
float getDiffuseIntensity(vec3 nLightVec, float lightAttenuation)
{
  return lightAttenuation * clamp(dot(nLightVec, nNormal), 0., 1.);
}

vec3 getDiffuse(int i, float diffuse)
{
  return diffuse * getDiffuseColor(i);
}

// Eclairage de Lambert sans composante spéculaire
vec4 getFragColor()
{
  vec3 albedo = getAlbedo_tex();

  vec3 vAmbient = vec3(0.);//getAmbient();

  vec3 linearAttenuations = vec3(gl_LightSource[0].linearAttenuation,
				 gl_LightSource[1].linearAttenuation,
				 gl_LightSource[2].linearAttenuation);

  vec3 dists = vec3(length(vLightVec[0]),
		    length(vLightVec[1]));
  vec3 lightAttenuations = clamp(1. - linearAttenuations * dists, 0., 1.);

  vec3 nLightVec0 = normalize(vLightVec[0]);
  vec3 nLightVec1 = normalize(vLightVec[1]);

  float diffuse0 = getDiffuseIntensity(nLightVec0, lightAttenuations[0]);
  float diffuse1 = getDiffuseIntensity(nLightVec1, lightAttenuations[1]);

  vec3 vDiffuse = (getDiffuse(0, diffuse0) +
		   getDiffuse(1, diffuse1));

  // FIXME: distinguer le cas emission du cas diffusion
  vec3 color = albedo * (vAmbient + vDiffuse);

  return vec4(color, 1.);
}

void main()
{
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  gl_FragColor = getFragColor();
}
