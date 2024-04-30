#version 120

uniform int time;
uniform sampler2D tex0; // color

//
// Affichage du titre à partir d'une texture - effet de glow
//
// R : glow du logo hAND
// G : glow du logo Ctrl-Alt-Test
// B : glow du bouton Level One
// A : glow de l'artwork autour du logo hAND
//
// Voir titleScreen.frag pour le titre en avant plan
//
void main()
{
  float t = float(time);
  float start = 2000.;

  vec2 uv = gl_TexCoord[0].xy;
  vec4 src = texture2D(tex0, uv);

  float showScreen = smoothstep(start, start + 100., t) * smoothstep(17000., 16000., t);
  float showCredits = smoothstep(15000., 14500., t);
  float showGlow = smoothstep(6000., 8000., t) * showCredits;
  float showLvlGlow = smoothstep(10000., 10100., t);

  vec4 handColor = vec4(0.22, 0.25, 0.5, 1.);
  vec4 catColor = vec4(0.22, 0.25, 0.5, 1.);
  vec4 lvlColor = vec4(1., 0.96, 0.8, 1.);
  vec4 backColor = vec4(1., 1., 1., 1.);

  float showHand = (0.6 + 0.2 * sin(0.003*t)) * showGlow;
  vec4 hand = src.r * showHand * handColor;

  float showCAT = (0.6 + 0.2 * sin(0.003*t)) * showGlow;
  vec4 cat = src.g * showCAT * catColor;

  float showLvl = clamp(1. + 0.6*sin(0.01*t), 0., 1.) * showLvlGlow;
  vec4 lvl = src.b * showLvl * lvlColor;

  float showBack = mix(0.2, 0.6, 0.5 + 0.5 * sin(0.003*t)) * showGlow;
  vec4 back = src.a * showBack * backColor;


  vec4 color = hand + cat + lvl + back;
  color.a *= showScreen;

  gl_FragColor = color;
}
