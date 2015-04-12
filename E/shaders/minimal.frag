varying mat3 vLight;
varying vec3 vNormal;

vec2 vTexCoord;
vec2 getTexCoord();

vec3 nNormal;
vec3 getNormal();

vec4 getAlbedo();

vec4 getDiffuseColor(int i)
{
  return gl_LightSource[i].diffuse;
}

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

// Eclairage de Lambert sans composante spéculaire
vec4 getFragColor()
{
  vec4 albedo = getAlbedo();

  vec4 vAmbient = vec4(0., 0., 0., 1.);//getAmbient();

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
  vec4 resultLighting = vec4(vAmbient.rgb + vDiffuse.rgb, min(vAmbient.a, vDiffuse.a));
  vec3 diffuseComponent = albedo.rgb * mix(vec3(1.), resultLighting.rgb, albedo.a);

  vec3 color = diffuseComponent;
  float glow = min(albedo.a, resultLighting.a);
  return vec4(color, glow);
}

void main(void)
{
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  gl_FragColor = getFragColor();
}
