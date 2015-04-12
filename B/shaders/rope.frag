// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 texCoord;

uniform sampler2D cMap;
uniform sampler2D c2Map;
uniform sampler2D sMap;
uniform float texFade;

void main(void)
{
  vec4 albedo = vec4(vColor.rgb * texture2D(cMap, texCoord).a, vColor.a);

  float distSqr = dot(vLight, vLight);

  vec3 nLight = vLight * inversesqrt(distSqr);
  vec3 nNormal = normalize(vNormal);
  vec3 nViewVec = normalize(vViewVec);

  float invRadius = 0.001;
  float att = clamp(1. - invRadius * sqrt(distSqr), 0., 1.);

  vec4 vAmbient = gl_LightSource[0].ambient;// * gl_FrontMaterial.ambient;

  float diffuse = att * clamp(dot(nLight, nNormal), 0., 1.);
  vec4 vDiffuse = vec4(diffuse * gl_LightSource[0].diffuse.rgb,
		       1. - diffuse * (1. - gl_LightSource[0].diffuse.a));

  float specular = 0.;

  if (diffuse > 0.)
  {
    vec3 vReflect = reflect(-nLight, nNormal);
    float shininess = 20.; // gl_FrontMaterial.shininess
    specular = pow(clamp(dot(vReflect, nViewVec), 0., 1.), shininess) * texture2D(sMap, texCoord).a;
  }

  vec3 color = (albedo.a * (vAmbient.rgb + vDiffuse.rgb) + 1. - albedo.a) * albedo.rgb + specular;
  float glow = min(min(albedo.a, 1. - 0.3 * specular),
		   min(vAmbient.a, vDiffuse.a));
  gl_FragColor = vec4(color, glow);
}
