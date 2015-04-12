// -*- glsl -*-

varying vec3 vViewVecSpec;
varying vec3 vViewVecInv;
varying vec3 vLight;

varying vec4 vColor;
varying vec2 texCoord;
varying mat3 tanSpace;

uniform sampler2D cMap;
uniform sampler2D c2Map;
uniform sampler2D nMap;
uniform sampler2D sMap;
uniform float texFade;

/*
uniform int id;

vec3 saturate(vec3 col, float n)
{
  float grey = (col.r + col.g + col.b) / 3.;
  col.r += (col.r - grey) * n;
  col.g += (col.g - grey) * n;
  col.b += (col.b - grey) * n;
  return col;
}
*/

void main (void)
{
  vec3 nViewVec = normalize(vViewVecSpec);
  vec3 nViewVecInv = normalize(vViewVecInv);
  float height = texture2D(nMap, texCoord).w;
  vec2 vOffset = 0.03 * height * nViewVecInv.xy;

  vec2 vTexCoord = texCoord + vOffset;

  vec4 albedo = vColor * mix(texture2D(cMap, texCoord),
			     texture2D(c2Map, texCoord),
			     texFade);

  float distSqr = dot(vLight, vLight);

  vec3 nLight = vLight * inversesqrt(distSqr);
  vec3 bump = texture2D(nMap, vTexCoord).xyz * 2. - 1.;
  vec3 nNormal = tanSpace * bump;

  float invRadius = 0.01; // FIXME : récupérer depuis gl_LightSource ?
  float att = clamp(1. - invRadius * sqrt(distSqr), 0., 1.);

  vec4 vAmbient = gl_LightSource[0].ambient;

  float diffuse = att * clamp(dot(nLight, nNormal), 0., 1.);
  vec4 vDiffuse = vec4(diffuse * gl_LightSource[0].diffuse.rgb,
		       1. - diffuse * (1. - gl_LightSource[0].diffuse.a));

  float specular = 0.;
  if (diffuse > 0.)
  {
    vec3 vReflect = reflect(-nLight, nNormal);
    float shininess = 20.;
    specular = pow(clamp(att * dot(vReflect, nViewVec), 0., 1.), shininess) * texture2D(sMap, vTexCoord).a;
  }

  // Coloration par l'ID
  /*
  vec4 col = vec4(float(mod(id,4)), float(mod(id/16, 4)), float(mod(id/256, 4)), 0.);
  col = 1. - (col / 12.);
  albedo *= col;
  albedo.xyz = saturate(albedo.xyz, 0.2);
  */

  vec3 color = (albedo.a * (vAmbient.rgb + vDiffuse.rgb) + 1. - albedo.a) * albedo.rgb + specular;
  float glow = min(min(albedo.a, 1. - 0.3 * specular),
		   min(vAmbient.a, vDiffuse.a));
  gl_FragColor = vec4(color, glow);
}
