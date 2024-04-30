#version 120

//
// Essai d'effet avec des bandes ou brûlures verticales
//
// A retravailler
//

uniform int time;
uniform sampler2D tex0; // color

vec3 addVBurn(vec3 color, vec2 uv, float position, float width, float height)
{
  float hFactor = clamp(200. * (0.5 * width - abs(uv.x - position)), 0., 1.);
  float vFactor = pow((1.2 / height) * max(height - uv.y, 0.), 8.);
  float light = mix(0.7, 1.5, vFactor * hFactor);
  float additive = mix(0., 0.2, vFactor * hFactor);
  return additive + light * color;
}

vec3 addVLine(vec3 color, vec2 uv, float position, float width)
{
  float hFactor = clamp(200. * (0.5 * width - abs(uv.x - position)), 0., 1.);
  float light = mix(0.7, 1.5, hFactor);
  float additive = mix(0., 0.2, hFactor);
  return additive + light * color;
}

void main()
{
  vec2 uv = gl_TexCoord[0].xy;

  vec3 color = texture2D(tex0, uv).rgb;
  color = (addVBurn(color, uv, 0.6 + 0.01 * sin( 0.001 * time), 0.1, 0.5) +
	   addVBurn(color, uv, 0.5 + 0.3 * sin( 0.0003 * time), 0.02, 0.25) +
	   addVBurn(color, uv, 0.8 + 0.1 * sin( 0.0006 * time), 0.01, 0.8) +
	   addVLine(color, uv, 0.2 + 0.2 * sin( 0.0006 * time), 0.2) +
	   addVLine(color, uv, 0.85 + 0.04 * sin( 0.0007 * time), 0.03) +
	   addVLine(color, uv, 0.4 + 0.11 * sin( 0.0005 * time), 0.02)) * 0.2;
  gl_FragColor = vec4(color, 1.);
}
