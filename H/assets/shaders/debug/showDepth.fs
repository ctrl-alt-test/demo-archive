#version 330

uniform sampler2D texture0;

in vec2 texCoord;
layout(location = 0) out vec4 fragmentColor;

void main()
{
  float d0 = texture2D(texture0, texCoord).r;
  float d1 = fract(5.0*d0);

  float r = smoothstep(0., 1., abs(6.*d1-4.)-1.);
  float b = smoothstep(0., 1., abs(6.*d1-2.)-1.);
  float g = smoothstep(0., 1., abs(6.*d1)-1.) * smoothstep(0., 1., abs(6.*d1-6.)-1.);

  fragmentColor = vec4(mix(vec3(d0), vec3(r, g, b), 0.125), 1.0);
}
