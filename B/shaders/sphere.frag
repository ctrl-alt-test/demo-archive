// -*- glsl -*-

//
// Ce shader est celui de la sphère
//
// Une belle sphère rouge plastique
//

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;

void main(void)
{
  vec4 albedo = vec4(1., 0, 0, 1.);

  float distSqr = dot(vLight, vLight);

  vec3 nLight = vLight * inversesqrt(distSqr);
  vec3 nNormal = normalize(vNormal);
  vec3 nViewVec = normalize(vViewVec);

  float invRadius = 0.001;
  float att = clamp(1. - invRadius * sqrt(distSqr), 0., 1.);

  vec4 vAmbient = gl_LightSource[0].ambient;// * gl_FrontMaterial.ambient;

  float diffuse = max(0., dot(nLight, nNormal));
  vec4 vDiffuse = att * diffuse * gl_LightSource[0].diffuse; // * gl_FrontMaterial.diffuse;

  float specular = 0.;

  if (diffuse > 0.)
  {
    vec3 vReflect = reflect(-nLight, nNormal);
    float shininess = 6.; // gl_FrontMaterial.shininess
    // Ligne voulue :
    specular = 0.4 * pow(clamp(dot(vReflect, nViewVec), 0., 1.), shininess);
  }

  gl_FragColor = vAmbient * albedo + vDiffuse * albedo + specular; // + gl_FrontMaterial.emission;
}
