#version 120

varying vec3 viewVec;

void main()
{
  vec4 vertex = vec4(2. * gl_Vertex.xy - vec2(1.), 0., 1.);

  mat4 tp = transpose(gl_ModelViewProjectionMatrix);
  vec4 xPlane = tp[3] - tp[0] * vertex.x;
  vec4 yPlane = tp[3] - tp[1] * vertex.y;

  viewVec = cross(xPlane.xyz, yPlane.xyz) * (-vertex.x * vertex.y);

  gl_Position = vertex;
}
