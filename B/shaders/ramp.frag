// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 texCoord;

uniform sampler2D cMap;
uniform sampler2D sMap;

void main(void)
{
  vec4 albedo = vColor * texture2D(cMap, texCoord);

  float distSqr = dot(vLight, vLight);

  vec3 nLight = vLight * inversesqrt(distSqr);
  vec3 nNormal = normalize(vNormal);
  vec3 nViewVec = normalize(vViewVec);

  float invRadius = 0.001;
  float att = clamp(1. - invRadius * sqrt(distSqr), 0., 1.);

  vec4 vAmbient = gl_LightSource[0].ambient;// * gl_FrontMaterial.ambient;

  float diffuse = max(0., dot(nLight, nNormal));
  vec4 vDiffuse = vec4(diffuse * gl_LightSource[0].diffuse.rgb,
		       1. - diffuse * (1. - gl_LightSource[0].diffuse.a));

  vec3 color = (albedo.a * (vAmbient.rgb + vDiffuse.rgb) + 1. - albedo.a) * albedo.rgb;
  float glow = min(albedo.a, min(vAmbient.a, vDiffuse.a));
  gl_FragColor = vec4(color, glow);
}
