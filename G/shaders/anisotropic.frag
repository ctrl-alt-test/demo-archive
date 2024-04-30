#version 120

uniform float shininess;
uniform float trans;

varying float vFogFactor;
varying vec3 vSpeed;
varying vec3 vViewVec;

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nTangent;
vec3 nViewVec;

vec2 getTexCoord();
vec3 addFog(vec3 color, float intensity);
vec3 getAlbedo_tex();
vec3 getNormal();
vec3 getSpeedColor(vec3 speed);
void getLightShadingBlinnPhongVSM(out vec3 diffuseComponent, out vec3 specularComponent);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();
  nNormal = getNormal();
  nTangent = vec3(1., 0., 0.);

  vec3 diffuseComponent;
  vec3 specularComponent;
  getLightShadingBlinnPhongVSM(diffuseComponent, specularComponent);

  // FIXME: distinguer le cas emission du cas diffusion
  vec3 albedo = getAlbedo_tex();
  vec3 color = albedo * diffuseComponent + specularComponent;

  gl_FragData[0] = vec4(addFog(color, vFogFactor), 1.);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
