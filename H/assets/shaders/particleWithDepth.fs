#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform mat4 projectionMatrix;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform samplerCube diffuseEnvMap;
uniform samplerCube specularEnvMap;
uniform vec3 diffuseColor;
uniform vec3 emissiveColor;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 specularColor;

in vec2 depthRange;
in vec2 texCoord;
in vec3 bitangent;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 tangent;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec2 p = 2. * texCoord - 1.;
  float d = length(p);
  if (d > 1.)
    discard;

  vec3 T_ = tangent;
  vec3 B_ = bitangent;
  vec3 N_ = cross(T_, B_);

  vec3 E = normalize(viewVec);
  vec3 N = (p.x * T_ + p.y * B_ + sqrt(1. - clamp(d * d, 0., 1.)) * N_) * smoothstep(1.01, 1., d);

  vec3 albedo = diffuseColor;
  vec3 f0 = specularColor;

  vec3 fresnel = Schlick(f0, E, N);
  vec3 color = emissiveColor +
	mix(albedo * texture(diffuseEnvMap, N).rgb*0.05, // Almost disabled diffuse env until the diffuse map is correct.
	texture(specularEnvMap, reflect(-E, N)).rgb,
	fresnel);
  float shadowAttenuations[i_NUM_LIGHTS];
  shadowAttenuations[0] = getVSMAttenuation(shadowMaps[0], shadowCoord[0]/* , 0.005*/);
  shadowAttenuations[1] = getVSMAttenuation(shadowMaps[1], shadowCoord[1]/* , 0.005*/);
  shadowAttenuations[2] = getVSMAttenuation(shadowMaps[2], shadowCoord[2]/* , 0.005*/);
  shadowAttenuations[3] = getVSMAttenuation(shadowMaps[3], shadowCoord[3]/* , 0.005*/);
  shadowAttenuations[4] = getVSMAttenuation(shadowMaps[4], shadowCoord[4]/* , 0.005*/);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = shadowAttenuations[i] / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, f0, roughness);
  }

  float offset = (1. - d * d) * (depthRange.x / depthRange.y)*0.5;
  float depth = clamp(gl_FragCoord.z + offset * gl_FragCoord.w, 0.000001, 1.);
  gl_FragDepth = depth;
  fragmentColor = vec4(color, 1.);
}
