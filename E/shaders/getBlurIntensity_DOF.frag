uniform float dof;

float getDepth(vec2 uv);

float getBlurIntensity(vec2 uv)
{
  float intensity = clamp(abs(getDepth(uv) - dof), 0., 1.);
  return intensity;
}
