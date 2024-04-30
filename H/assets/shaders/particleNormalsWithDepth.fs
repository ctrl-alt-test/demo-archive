#version 330

in vec2 depthRange;
in vec2 texCoord;
in vec3 bitangent;
in vec3 tangent;

out vec4 fragmentColor;

void main()
{
  vec2 p = 2. * texCoord - 1.;
  float d = length(p);
  if (d > 1.)
    discard;

  vec3 T_ = tangent;
  vec3 B_ = bitangent;
  vec3 N_ = cross(T_, B_);
  vec3 N = (p.x * T_ + p.y * B_ + sqrt(1. - clamp(d * d, 0., 1.)) * N_) * smoothstep(1.01, 1., d);

  vec3 color = N * 0.5 + 0.5;

  float offset = (1. - d * d) * (depthRange.x / depthRange.y)*0.5;
  float depth = clamp(gl_FragCoord.z + offset * gl_FragCoord.w, 0.000001, 1.);
  gl_FragDepth = depth;
  fragmentColor = vec4(color, 1.);
}
