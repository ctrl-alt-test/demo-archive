#version 120

uniform int time;
uniform sampler2D tex0; // color


//
// Affichage du titre à partir d'une texture
//
// R : logos hAND et Ctrl-Alt-Test
// G : bouton Level One
// B : contour des logos hAND et Ctrl-Alt-Test
// A : contour du bouton Level One
//
// Voir titleScreenGlow.frag pour l'effet de glow en fond
//
void main()
{
  float t = float(time);
  float start = 2000.;

  vec2 uv = gl_TexCoord[0].xy;
  float limit = clamp((t - start) * 0.00018, 0., 1.);
  uv.y = clamp(uv.y, 0., limit);

  vec4 src = texture2D(tex0, uv);

  vec3 borderColor = vec3(0.26, 0.2, 0.42);
  vec3 handColor = vec3(1., 0.95, 0.8);
  vec3 catColor = vec3(1., 0.87, 0.5);
  vec3 lvlColor = vec3(1., 0.8, 0.18);

  float showScreen = smoothstep(start, start + 100., t) * smoothstep(17000., 16000., t);
  float showCredits = smoothstep(15000., 14500., t);
  vec4 hand = src.r * vec4(handColor, showCredits) * smoothstep(0.81, 0.8, uv.y);
  vec4 cat = src.r * vec4(catColor, showCredits) * smoothstep(0.8, 0.81, uv.y);;
  vec4 border = src.b * vec4(borderColor, showCredits);

  float showLvl = smoothstep(10000., 10100., t) * clamp(1. + 0.6*sin(0.01*t), 0., 1.);
  vec4 lvl = src.g * vec4(lvlColor, showLvl);
  vec4 lvlBorder = src.a * vec4(borderColor, showLvl);

  vec4 color = border + cat + hand + lvlBorder + lvl;
  color.a *= showScreen;

  gl_FragColor = color;
}
