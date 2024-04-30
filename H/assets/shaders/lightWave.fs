vec3 lightWave(vec2 p, float time, vec3 color)
{
  p -= vec2(345., 10.);

  // light wave (city wakes up)
  // vec2 p = worldSpacePosition.xz - vec2(345., 10.);
  float dist = length(p);
  float angle = atan(p.x, p.y);
  float t = mix(-10., 430., smoothstep(10.000, 30.000, time));
  float rand = sin(angle * 10.) * 2. + sin(p.x + angle * 100.) * 0.5;
  
  float powArg = min(1., dist - t + rand);
  float pow_ = 1. / pow(2., -1. * powArg) * 1.3;
  float x = smoothstep(1.3, 1., pow_) * pow_;
//  if (time < 300.)
//    return color + color * x * 1000. * vec3(2., 1., 0.75);
//  else
    return color;
}
