#version 330

uniform samplerCube texture0;

in vec3 normal;

out vec4 fragmentColor;

void main()
{
  vec3 N = normalize(normal);
  vec3 color = texture(texture0, N).rgb;
  fragmentColor = vec4(color, 1.);
}
