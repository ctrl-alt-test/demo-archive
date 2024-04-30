#version 330

in vec2 depth;

out vec4 fragmentColor;

vec2 getMoments(float z)
{
  float dzx = dFdx(z);
  float dzy = dFdy(z);
  return vec2(z, z * z + 0.25 * (dzx * dzx + dzy * dzy));
}

void main()
{
  fragmentColor = vec4(getMoments(depth.x / depth.y * 0.5 + 0.5), 0., 1.);
}
