// -*- glsl -*-

//
// Ce shader est celui du cube OpenGL
//
// La couleur des sommets est déterminée par l'id, passé en uniform
//
// Pas de texture, pas de bump, pas de spéculaire,
// juste un peu d'éclairage Lambert
//

varying vec3 vViewVec;
varying vec3 vLight;

varying vec4 vColor;
varying vec3 vNormal;
varying vec3 vPos;

uniform int id;

void main(void)
{
  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;

  vColor = gl_Color;
  vPos = gl_Vertex.xyz;

  int fourBits = id;
  if (fourBits != 0)
  {
    fourBits /= 16;
    const float dark = 0.5;
    const float light = 0.75;

    fourBits = gl_Vertex.x < 0. ? fourBits / 4096 : fourBits % 4096;
    fourBits = gl_Vertex.y < 0. ? fourBits / 64 : fourBits % 64;
    fourBits = gl_Vertex.z < 0. ? fourBits / 8 : fourBits % 8;

    fourBits %= 8;
    float r = light;
    if (0 == fourBits / 4)
      r = dark;

    fourBits %= 4;
    float g = light;
    if (0 == fourBits / 2)
      g = dark;

    fourBits %= 2;
    float b = light;
    if (0 == fourBits)
      b = dark;

    vColor = vec4(r, g, b, 1.);
  }

  vNormal = gl_NormalMatrix * gl_Normal;

  vViewVec = normalize(-vVertex.xyz);
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  gl_Position = ftransform();
}
