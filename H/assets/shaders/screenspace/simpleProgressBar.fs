#version 330

uniform float progress;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

void main()
{
  float rx = smoothstep(0.667, 0.666, abs(texCoord.x * 2. - 1.));
  float ry = smoothstep(0.006, 0.005, abs(texCoord.y * 2. - .9));
  fragmentColor = vec4(1.) * max(min(0.7 * rx * ry, smoothstep(progress, progress - 0.005, clamp((texCoord.x - 1./6.) / (2./3.), 0., 1.))), 0.1 * rx * ry) * smoothstep(1., 0.8, progress);
}
