
uniform sampler2D mMap;
uniform vec2 center;

void main()
{
  gl_FrontColor = texture2D(mMap, gl_Vertex.xy * vec2(1., center.y/center.x));

  gl_PointSize = 0.5 * gl_Vertex.z;
  gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xy, 0, gl_Vertex.w);
}
