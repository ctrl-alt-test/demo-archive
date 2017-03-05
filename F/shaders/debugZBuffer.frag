//
// Affiche une représentation du Z-buffer
//

uniform sampler2D cMap;
uniform vec2 center;

//[
float getDepth(vec2 uv);
//]

void main()
{
  float d = getDepth(gl_TexCoord[0].xy);
  float r = 1. - min(2. * d, 1.);
  float g = 1. - abs(2. * d - 1.);
  float b = max(2. * d - 1., 0.);

  float dist = distance(gl_TexCoord[0].xy, center);

  gl_FragColor = mix(vec4(r, g, b, 1.), // Rendu en tons pour mieux évaluer
		     vec4(d, d, d, 1.), // Rendu noir vers blanc naïf
		     smoothstep(0.05, 0.3, dist));
}
