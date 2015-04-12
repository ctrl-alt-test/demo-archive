// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;

void main(void)
{
  vec4 albedo = vColor;

  vec3 nLight = normalize(vLight);
  vec3 nNormal = normalize(vNormal);
  vec3 nViewVec = normalize(vViewVec);

  vec4 vAmbient = vec4(0.85, 0.85, 0.85, 1.);

  float diffuse = 0.15 * max(0.0, dot(nLight, nNormal));
  vec4 vDiffuse = diffuse * vec4(1.);

  float specular = 0.0;
  if (diffuse > 0.0)
  {
    vec3 vReflect = reflect(-nLight, nNormal);
    float shininess = 10.; // gl_FrontMaterial.shininess
    // Ligne voulue :
    specular = 0.3 * pow(clamp(dot(vReflect, nViewVec), 0., 1.), shininess);
  }

  vec3 color = (vAmbient.rgb + vDiffuse.rgb) * albedo.rgb + specular;
  float glow = 1. - max(dot(albedo.xyz, albedo.xyz), 0.5 * specular);
  gl_FragColor = vec4(color, glow);
}
