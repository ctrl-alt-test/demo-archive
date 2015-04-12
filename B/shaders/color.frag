// -*- glsl -*-

varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec3 vPos;

uniform int time;

vec4 mutation()
{
  const float start = 248150.;
  const float end = start + 2200.;

  int screenStart = 290050;

  if (time < start || time > screenStart)
    return vColor;

  vec4 newColor = vec4(0.1, 0.5, 0.1, 0.7);
  if (time > end)
    return newColor;

  float t = float(time);
  float progress = smoothstep(start, end, t) * 1.8 - 0.8;

  float mut = smoothstep(progress - 0.05, progress + 0.05, 0.5 + vPos.y);
  vec4 col = mix(newColor, vColor, mut);

  mut = progress - 0.5 + vPos.y;
  if (mut < 0.)
    col.a = pow(abs(mut), 0.2);
  if (mut >= 0.)
    col.a = newColor.a;

  return col;
}

void main(void)
{
  vec4 albedo = mutation();

  vec3 nLight = normalize(vLight);

  vec3 nNormal = normalize(vNormal);

  vec4 vAmbient = vec4(0.85, 0.85, 0.85, 1.);

  float diffuse = 0.15 * max(0.0, dot(nLight, nNormal));
  vec4 vDiffuse = vec4(diffuse * gl_LightSource[0].diffuse.rgb,
		       1. - diffuse * (1. - gl_LightSource[0].diffuse.a));

  vec3 color = (vAmbient.rgb + vDiffuse.rgb) * albedo.rgb;
  float glow = min(albedo.a, vDiffuse.a);
  gl_FragColor = vec4(color, glow);
}
