uniform sampler2D c2Map;
uniform float zNear;
uniform float zFar;

float getDepth(vec2 uv)
{
  float z = texture2D(c2Map, uv).x;
  return (2.0 * zNear) / (zNear + mix(zFar, zNear, z));
}
