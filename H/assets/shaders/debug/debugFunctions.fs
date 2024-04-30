
vec3 debugTextureCoordinates(vec2 uv)
{
  float scale1 = 4.;
  vec2 gd1 = scale1 * 2. * fwidth(uv);
  vec2 uvGrid1 = smoothstep(0.8 * gd1, 1.2 * gd1, abs(fract(scale1 * uv + 0.5) * 2. - 1.));
  float grid1 = mix(min(uvGrid1.x, uvGrid1.y), 1., smoothstep(0.1, 0.5, max(gd1.x, gd1.y)));

  float scale2 = 20.;
  vec2 gd2 = scale2 * 1. * fwidth(uv);
  vec2 uvGrid2 = smoothstep(0.8 * gd2, 1.2 * gd2, abs(fract(scale2 * uv + 0.5) * 2. - 1.));
  float grid2 = mix(min(uvGrid2.x, uvGrid2.y), 1., smoothstep(0.1, 0.5, max(gd2.x, gd2.y)));

  float checkerScale1 = 4.;
  vec2 cd1 = checkerScale1 * 2. * fwidth(uv);
  vec2 uvChecker1 = smoothstep(1. - cd1, 1. + cd1, 2. * abs(fract(checkerScale1 * uv/2. + 0.25) * 2. - 1.));
  float checker1 = mix(abs(uvChecker1.x - uvChecker1.y), 0., smoothstep(0.1, 0.5, max(cd1.x, cd1.y)));

  float checkerScale2 = 40.;
  vec2 cd2 = checkerScale2 * 2. * fwidth(uv);
  vec2 uvChecker2 = smoothstep(1. - cd2, 1. + cd2, 2. * abs(fract(checkerScale2 * uv/2. + 0.25) * 2. - 1.));
  float checker2 = mix(abs(uvChecker2.x - uvChecker2.y), 0., smoothstep(0.1, 0.5, max(cd2.x, cd2.y)));

  vec3 color = vec3(fract(uv), 0.);
  color = mix(color, vec3(1.), 0.2 * checker1);
  color = mix(color, vec3(0.), 0.4 * checker2);
  color = mix(color, vec3(0.), 0.9 * (1. - mix(grid1, grid2, 0.5)));
  return color;
}

vec3 debugDistance(float d)
{
  float scale1 = 4.;
  float gd1 = scale1 * 2. * fwidth(d);
  float grid1 = smoothstep(0.8 * gd1, 1.2 * gd1, abs(fract(scale1 * d + 0.5) * 2. - 1.));
  grid1 = mix(grid1, 1., smoothstep(0.1, 0.5, gd1));

  float scale2 = 20.;
  float gd2 = scale2 * 1. * fwidth(d);
  float grid2 = smoothstep(0.8 * gd2, 1.2 * gd2, abs(fract(scale2 * d + 0.5) * 2. - 1.));
  grid2 = mix(grid2, 1., smoothstep(0.1, 0.5, gd2));

  vec3 color = vec3(fract(d), fract(d/10.), float(d > 0.) * fract(d/100.));
  color = mix(color, vec3(0.), 0.9 * (1. - mix(grid1, grid2, 0.5)));

  return color;
}

vec3 debugHighlightNaN(vec3 color)
{
  vec2 pattern = smoothstep(-0.01,0.01, fract(gl_FragCoord.xy / 10.) * 2. - 1.);
  vec3 debugColor = (pattern.x == pattern.y) ? vec3(1., 1., 0.) : vec3(1., 0., 0.);
  float test = dot(color, color);

  return (test == test ? color : debugColor);
}

float debugStripes()
{
  return step(0.5, fract(0.02 * (gl_FragCoord.x + gl_FragCoord.y)));
}

vec3 debugFrameMarks(vec2 uv, vec3 color, float intensity)
{
  float gridThirds = 0.;
  gridThirds += smoothstep(0.332, 0.333, uv.x) * smoothstep(0.334, 0.333, uv.x);
  gridThirds += smoothstep(0.665, 0.666, uv.x) * smoothstep(0.667, 0.666, uv.x);
  gridThirds += smoothstep(0.332, 0.333, uv.y) * smoothstep(0.334, 0.333, uv.y);
  gridThirds += smoothstep(0.665, 0.666, uv.y) * smoothstep(0.667, 0.666, uv.y);

  float gridQuadrants = 0.;
  gridQuadrants += smoothstep(0.499, 0.5, uv.x) * smoothstep(0.501, 0.5, uv.x);
  gridQuadrants += smoothstep(0.4985, 0.5, uv.y) * smoothstep(0.5015, 0.5, uv.y);

  float gridGolden = 0.;
  gridGolden += smoothstep(0.381, 0.382, uv.x) * smoothstep(0.383, 0.382, uv.x);
  gridGolden += smoothstep(0.619, 0.618, uv.x) * smoothstep(0.617, 0.618, uv.x);
  gridGolden += smoothstep(0.381, 0.382, uv.y) * smoothstep(0.383, 0.382, uv.y);
  gridGolden += smoothstep(0.619, 0.618, uv.y) * smoothstep(0.617, 0.618, uv.y);

  float grids = intensity;
  color = mix(color, vec3(1., 0.25, 1.) * (1. - color), grids * gridThirds);
  color = mix(color, vec3(0.5, 1., 0.25) * (1. - color), grids * gridQuadrants);
  color = mix(color, vec3(1., 1., 0.5) * (1. - color), grids * gridGolden);
  return color;
}
