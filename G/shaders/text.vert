#version 120

uniform float textStep;

varying vec4 vColor;

void main()
{
  vColor = gl_Color;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = ftransform();

  // translation
  gl_Position.x += 0.01 * textStep;
}
