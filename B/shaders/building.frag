// -*- glsl -*-

varying vec3 vViewVecSpec;
varying vec3 vViewVecInv;
varying vec3 vLight;

varying vec2 texCoord;
varying mat3 tanSpace;

uniform sampler2D cMap;
uniform sampler2D nMap;
uniform sampler2D sMap;

uniform int time;
uniform int id;
varying float bid;

float rand(float seed)
{
  return fract(sin(seed));
}

vec4 light(vec4 albedo)
{
  // si on est sur une fenetre...
  if (texture2D(sMap, texCoord).a > 0.8)
    {
      float count = (id == 1 ? 4. : 3.); // nombre de fenetres
      float x = float(int(count * texCoord.x)) * 5.5 + 1.;
      float y = float(int(count * texCoord.y)) * 3.7 + 4.;

      float nl = mix(-1., 0.2, rand(x + y));
      float n2 = mix(0.4, 0.6, rand(x * bid + y));
      float seed = x * 0.1 + y * bid + float(time)/60000.;
      float on = rand(seed) > 0.8 ? 0.6 : 1.;
      albedo = mix(vec4(0.8, n2, 0.2, nl), albedo, on);
    }
  return albedo;
}

void main (void)
{
  vec3 nViewVec = normalize(vViewVecSpec);
  vec3 nViewVecInv = normalize(vViewVecInv);
  float height = texture2D(nMap, texCoord).w;
  vec2 vOffset = 0.03 * height * nViewVecInv.xy;

  vec2 vTexCoord = texCoord + vOffset;

  vec4 albedo = texture2D(cMap, vTexCoord);
  albedo = light(albedo);

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
    specular = pow(clamp(dot(vReflect, nViewVec), 0., 1.), shininess) * texture2D(sMap, vTexCoord).a;
  }

  vec3 color = (albedo.a * (vAmbient.rgb + vDiffuse.rgb) + 1. - albedo.a) * albedo.rgb + specular;
  float glow = min(min(albedo.a, 1. - 0.3 * specular),
		   min(vAmbient.a, vDiffuse.a));
  gl_FragColor = vec4(color, glow);
}
