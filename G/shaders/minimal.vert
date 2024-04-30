#version 120

//[
void exportBaryCoords();
void exportColor();
void exportDepth();
void exportFogFactor(vec3 vVertex);
void exportShadowCoord(vec4 vertex);
void exportSpeed(vec4 oldVertex, vec4 curVertex);
//]

void main()
{
  //  vColor = gl_Color;
  vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vertex = gl_Vertex;

  gl_Position = gl_ModelViewProjectionMatrix * vertex;

  exportColor();
  exportDepth();
  exportShadowCoord(vertex);
  exportFogFactor(vVertex);
  exportBaryCoords();
  exportSpeed(vertex, vertex);
}
