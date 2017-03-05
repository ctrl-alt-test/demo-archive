
//varying vec4 vColor;
varying vec3 vSpeed;
varying float fogFactor;

//[
float getFogFactor(vec3 vVertex);
vec3 getSpeed();
//]

void main()
{
  //  vColor = gl_Color;
  vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;
  fogFactor = getFogFactor(vVertex);
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = ftransform();
  vSpeed = getSpeed();
}
