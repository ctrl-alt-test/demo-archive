// -*- glsl -*-

varying vec2 texCoord;

void main(void)
{
  texCoord = gl_MultiTexCoord0.xy;
  gl_Position = ftransform();
}
