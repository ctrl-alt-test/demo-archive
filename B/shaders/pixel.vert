// -*- glsl -*-

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 texCoord;

uniform sampler2D cMap;

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  // Couleur de la texture multipliée par la couleur du sommet
  // FIXME : attention aux formats de résolution
  vColor = gl_Color * (0.1 + texture2D(cMap, vec2(gl_Vertex.x / 480., gl_Vertex.z / 270.)));

  vViewVec = normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
  vNormal = normalize(gl_NormalMatrix * gl_Normal);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
