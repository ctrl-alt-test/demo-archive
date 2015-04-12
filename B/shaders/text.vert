// -*- glsl -*-

varying vec4 vColor;
varying vec2 texCoord;

uniform int time;
uniform float trans;

void main(void)
{
  vColor = gl_Color;
  vColor.a *= smoothstep(0., 0.3, trans) * smoothstep(1., 0.7, trans);

  gl_TexCoord[0] = gl_MultiTexCoord0;
  texCoord = gl_MultiTexCoord0.xy;
  gl_Position = ftransform();

  if (time < 330000) // Pas d'effet sur le dernier crédit
  {
    // FIXME
    vec2 coord = 0.08 * (vec2(gl_Vertex.x < 0.5, gl_Vertex.y < 0.5) - 0.5);
    //  float t = smoothstep(1., 0, clamp(8. * trans, 0., 1.));
    float t = 1. - clamp(8. * trans, 0., 1.);
    gl_Position.x -= coord.x * t;
    gl_Position.y += 1.6 * coord.y * t;

    // translation
    gl_Position.x += 0.04 * trans;
  }
}
