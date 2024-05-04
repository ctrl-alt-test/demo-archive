#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float opacity;
uniform float time;
uniform int lightTypes[i_NUM_LIGHTS];
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;

in vec2 texCoord;
in vec3 bitangent;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 tangent;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];
in vec4 viewSpacePosition;
in vec4 worldSpacePosition;

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

  float shadowAttenuation = getProjectorLightCone(shadowCoord[0], lightTypes[0], time) * getVSMAttenuation(shadowMaps[0], shadowCoord[0]);
  float attenuation = shadowAttenuation / (1. + dot(lightVecs[0], lightVecs[0]));
  float bubblesExtraDensity = 12.; // Tweaked until the coloration looked good.
  vec3 lightIntensity = attenuation * (lightColors[0] *
    // Hard coded value: 67 was found by incrementally searching the
    // distance at between the light source and the water surface.
    // This means that if the light position changes, this will break.
    getDistanceAbsorption(mediumColor, bubblesExtraDensity * mediumDensity * (sqrt(dot(lightVecs[0], lightVecs[0])) - 67.)));
  vec3 scatteredLight = lightIntensity *
    getDistanceAbsorption(mediumColor, bubblesExtraDensity * mediumDensity * abs(worldSpacePosition.y));

  vec3 diffuse = scatteredLight * mediumColor;
  vec3 specular = lightIntensity * vec3(smoothstep(-0.25, 0., N.y)); // Approximate the light coming from the top.
  vec3 fresnel = Schlick(vec3(1.325, 1.33, 1.335), vec3(1.), E, N);
  vec3 color = mix(diffuse, specular, fresnel); // Approximate all the scattering.

  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  fragmentColor = vec4(color, mix(fresnel.r, 1. - d * d, 0.4));
}
