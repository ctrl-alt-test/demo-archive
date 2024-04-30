#version 120

uniform sampler2D tex2; // glow
uniform vec2 center;
uniform vec4 lensOrbsSize;
uniform vec4 lensOrbsCol;

varying vec2 aberration;

void main()
{
  gl_FrontColor =
    texture2D(tex2, gl_Vertex.xy * vec2(1., center.y/center.x)) * vec4(lensOrbsCol.rgb * lensOrbsCol.a, 1.);

  aberration = 0.1 * (gl_Vertex.xy - center) * vec2(1., -1.);

  vec2 size = gl_Vertex.z > 0 ? lensOrbsSize.xy : lensOrbsSize.zw;
  gl_PointSize = 0.5 * mix(size.x, size.y, abs(gl_Vertex.z));
  gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xy, 0, gl_Vertex.w);
}
