#version 330

uniform sampler2D texture0;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

void main()
{
  fragmentColor = texture2D(texture0, texCoord);
}
