#version 120

varying vec2 vDepth;

vec2 getMoments(float depth)
{
  vec2 moments;
  // First moment is the depth itself.
  moments.x = depth;
  // Compute partial derivatives of depth.
  float dx = dFdx(depth);
  float dy = dFdy(depth);
  // Compute second moment over the pixel extents.
  moments.y = depth * depth + 0.25 * (dx * dx + dy * dy);

  return moments;
}

void main()
{
  float depth = vDepth.x / vDepth.y * 0.5 + 0.5;

  gl_FragColor = vec4(getMoments(depth), 0., 1.);
}
