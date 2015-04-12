// -*- glsl -*-

varying vec2 texCoord;

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  texCoord = gl_MultiTexCoord0.xy;
  gl_Position = ftransform();
}
