#version 330

uniform sampler2D texture0;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

void main()
{
  vec4 color = texture2D(texture0, texCoord);

  float gamma = 2.2;
  color.rgb = pow(color.rgb, vec3(1.0/gamma));

  // Luma in the alpha channel for the FXAA pass
  color.a = dot(color.rgb, vec3(0.299, 0.587, 0.114));

  fragmentColor = color;
}
