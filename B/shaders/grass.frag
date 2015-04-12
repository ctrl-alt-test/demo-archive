// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec3 vTangent;
varying vec2 texCoord;
varying mat3 tanSpace;
varying vec3 vPos;

uniform sampler2D cMap;
uniform sampler2D sMap;
uniform sampler2D nMap;

uniform sampler2D c2Map;
uniform int time;

const int startRosace = 253200;

float rosace()
{
  float t = float(time - startRosace) / 200.;
  float ang = t * 0.1;

  vec2 center = vec2(50., 10. + t);
  vec2 p = center - vPos.xz;
  p *= mat2(vec2(cos(ang), -sin(ang)), vec2(sin(ang), cos(ang))); // rotation !
  p = clamp(0.5 + p * 0.05, 0., 1.);
  float alpha = texture2D(c2Map, p).r;
  alpha = max(alpha, smoothstep(263500., 265500., float(time)));
  return mix(0.6, 1., alpha);
}

void main(void)
{
  vec4 albedo = vec4(vColor.rgb * texture2D(cMap, texCoord).a, vColor.a);
  if (time > startRosace)
    albedo.a *= rosace();

  float distSqr = dot(vLight, vLight);

  vec3 nLight = vLight * inversesqrt(distSqr);
  vec3 bump = texture2D(nMap, texCoord).xyz * 2. - 1.;
  vec3 nNormal = tanSpace * bump;
  vec3 nViewVec = normalize(vViewVec);

  /*
  float invRadius = 0.01;
  float att = clamp(1. - invRadius * sqrt(distSqr), 0., 1.);
  */

  vec4 vAmbient = gl_LightSource[0].ambient;

  float diffuse = /* att * */ clamp(dot(nLight, nNormal), 0., 1.);
  vec4 vDiffuse = vec4(diffuse * gl_LightSource[0].diffuse.rgb,
		       1. - diffuse * (1. - gl_LightSource[0].diffuse.a));

  float specular = 0.;
  if (diffuse > 0.)
  {
    vec3 vReflect = reflect(-nLight, nNormal);
    float shininess = 20.;
    specular = pow(clamp(dot(vReflect, nViewVec), 0., 1.), shininess) * texture2D(sMap, texCoord).a;
  }

  vec3 color = (albedo.a * (vAmbient.rgb + vDiffuse.rgb) + 1. - albedo.a) * albedo.rgb + specular;
  float glow = min(min(albedo.a, 1. - 0.3 * specular),
		   min(vAmbient.a, vDiffuse.a));
  gl_FragColor = vec4(color, glow);
}
