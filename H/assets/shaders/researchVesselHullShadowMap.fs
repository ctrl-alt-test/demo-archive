#version 330

in vec2 depth;
in vec3 objectSpacePosition;

out vec4 fragmentColor;

vec2 getMoments(float z)
{
  float dzx = dFdx(z);
  float dzy = dFdy(z);
  return vec2(z, z * z + 0.25 * (dzx * dzx + dzy * dzy));
}

void main()
{
  if (objectSpacePosition.y/9. >
      mix(0.3, 0.6 + 0.15 * objectSpacePosition.z/25. * objectSpacePosition.z/25.,
	  smoothstep(0.2, 0.1, objectSpacePosition.z/25.)))
  {
    discard;
  }

  fragmentColor = vec4(getMoments(depth.x / depth.y * 0.5 + 0.5), 0., 1.);
}
