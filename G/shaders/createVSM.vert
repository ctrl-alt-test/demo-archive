#version 120

varying vec2 vDepth;

void main()
{
  gl_Position = ftransform();
  vDepth = gl_Position.zw;
}
