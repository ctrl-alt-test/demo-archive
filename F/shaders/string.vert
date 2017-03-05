// Pour le tableau
#version 120

//
// Shader pour les cordes
//

uniform int id;
uniform int time;
uniform float trans;

attribute vec3 vTan;

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vViewVec;
varying vec3 vLight0;
varying vec3 vLight1;
varying vec4 vColor;
varying vec3 vSpeed;

varying float fogFactor;

//[
float getFogFactor(vec3 vVertex);
vec3 getLightGlobal(int i, vec3 vVertex);
vec3 getSpeed();
void computeShadowCoord();
//]

const vec3 colors[12] = vec3[12](
  vec3(0.98, 0.61, 0.68),  // Rose f981ad
  vec3(0.80, 0.11, 0.35),  // Fuschia #cb1d58
  vec3(0.95, 0.22, 0.16),  // Rouge #f3392a
  vec3(1.0, 0.41, 0.05),   // Orange #fe690c
  vec3(0.98, 0.93, 0.22),  // Jaune #faec38
  vec3(0.11, 0.79, 0.36),  // Vert clair #1dc95d
  vec3(0.07, 0.45, 0.39),  // Vert foncé #117464
  vec3(0.28, 0.77, 0.96),  // Bleu clair #48c4f4
  vec3(0.09, 0.55, 0.91),  // Bleu roi #188be9
  vec3(0.11, 0.22, 0.55),  // Bleu marine #1c378d
  vec3(0.26, 0.18, 0.43),  // Violet #422d6e
  vec3(1., 1., 1.));       // Blanc

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;

  float t = float(time);
  vec4 wave = vec4(sin(30. * t), 0., cos(35. * t), 0.) * pow(trans, 50.) * 0.1;
  vec3 vVertex = (gl_ModelViewMatrix * (gl_Vertex + wave)).xyz;

  // couleur aux cordes qui vibrent (à partir de l'envolée)
  float p = float(step(165900., t)) * pow(trans, 50.);
  int i = id - 12 * (id / 12);
  vColor.rgb = mix(vec3(1.), colors[i], p);
  vColor.a = 1. - 0.8 * pow(trans, 100.);

  computeShadowCoord();

  vNormal = normalize(gl_NormalMatrix * gl_Normal);
  vTangent = normalize(gl_NormalMatrix * vTan);
  vLight0 = getLightGlobal(0, vVertex);
  vLight1 = getLightGlobal(1, vVertex);
  vViewVec = -vVertex;

  fogFactor = getFogFactor(vVertex);

  gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + wave);
  vSpeed = getSpeed();
}
