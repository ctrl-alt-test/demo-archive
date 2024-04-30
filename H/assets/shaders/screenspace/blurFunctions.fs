//
// Directional Gaussian blur
//
// Example:
// color = directionnalGaussianBlur(texture0, texCoord, vec2(0., invResolution.y));
//
vec4 directionalGaussianBlur(sampler2D tex, vec2 uv, vec2 spread)
{
  float weights[5] = float[5](0.2270270270,
			      0.1945945946,
			      0.1216216216,
			      0.0540540541,
			      0.0162162162);

  vec4 c = texture2D(tex, uv) * weights[0];
  for (int i = 1; i < 5; ++i)
  {
    vec2 offset = spread * float(i);
    c += texture2D(tex, uv + offset) * weights[i];
    c += texture2D(tex, uv - offset) * weights[i];
  }
  return c;
}

float getDepth(sampler2D depthMap, vec2 uv, float zNear, float zFar)
{
  float z = texture2D(depthMap, uv).x;
  return (2.0 * zNear) / (zNear + mix(zFar, zNear, z));
}

vec4 directionalBilateralGaussianBlur(sampler2D tex, sampler2D depthMap, vec2 uv, vec2 spread,
				      float zNear, float zFar, float sharpness)
{
  float weights[5] = float[5](0.2270270270,
			      0.1945945946,
			      0.1216216216,
			      0.0540540541,
			      0.0162162162);

  float refDepth = getDepth(depthMap, uv, zNear, zFar);
  vec4 c = texture2D(tex, uv) * weights[0];
  float sum = weights[0];
  for (int i = 1; i < 5; ++i)
  {
    vec2 offset = spread * float(i);
    float depth = getDepth(depthMap, uv + offset, zNear, zFar);
    float weight = weights[i] / (1. + sharpness * abs(refDepth - depth));
    c += texture2D(tex, uv + offset) * weight;
    c += texture2D(tex, uv - offset) * weight;
    sum += 2. * weight;
  }
  return c / sum;
}

//
// Kawase light streak blur
// Computes two opposite directions at the same time.
//
// Example (requires multiple passes, from 0 to n-1):
// kawaseStreakBlur(texture0, texture1, texCoord, vec2(invResolution.x, 0.),
//                  0.9, numPass,
//                  color0, color1);
//
void KawaseStreakBlur(sampler2D tex0, sampler2D tex1, vec2 uv, vec2 dir,
		      vec3 power, int pass,
		      out vec3 color0, out vec3 color1)
{
  vec3 c0 = vec3(0.);
  vec3 c1 = vec3(0.);
  vec3 sum = vec3(0.);
  float b = pow(4., float(pass));
  for (int i = 0; i < 4; ++i)
  {
    // 0.25 meant to increase precision when power is close to 1
    vec3 weight = pow(power, vec3(0.25 * b * i));
    c0 += weight * texture2D(tex0, uv + b * i * dir).rgb;
    c1 += weight * texture2D(tex1, uv - b * i * dir).rgb;
    sum += weight;
  }
  color0 = c0 / sum;
  color1 = c1 / sum;
}
