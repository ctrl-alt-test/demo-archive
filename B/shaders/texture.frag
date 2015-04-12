// -*- glsl -*-

varying vec2 texCoord;

uniform sampler2D cMap;
uniform sampler2D c2Map;
uniform float texFade;

void main(void)
{
  vec4 albedo = ((1. - texFade) * texture2D(cMap, texCoord) +
		 texFade * texture2D(c2Map, texCoord));

  gl_FragColor = albedo;
}
