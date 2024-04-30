#version 120

uniform sampler2D tex1; // Relief
attribute vec4 aCol;
varying vec4 vColor;

//[
float getShift();
//]

vec3 getDisplacement(vec2 uv)
{
  uv.y += getShift();

  float underwaterHeight = -10.;
  float hillsHeight = 20. + 40. * texture2D(tex1, uv).a;
  float riftTop = 50. + 50. * texture2D(tex1, uv).a;
  float riftPass = 80. + 60. * texture2D(tex1, uv).a;
  float xb = 2. * (1. - 2. * uv.x);
  float riftHeight = mix(riftPass, riftTop, 1. - 1./(xb*xb + 1.)); // Col du rift avec un 1/x^2
  float desertEntryHeight = 5. + 40. * texture2D(tex1, uv).a;
  float side = pow(abs(uv.x - 0.5)*2., 3.) * 100.;
  float desertHeight = -5. + (20. + side) * texture2D(tex1, uv).a;
  float mountainHeight = 120. * texture2D(tex1, uv).a;

  float coastLine = 3.35 + 0.25 * texture2D(tex1, vec2(uv.x, 0.)).a;
  float riftLine = 22. - 0.2 * texture2D(tex1, vec2(uv.x, 0.5)).a;
  float desertLine = 22.;
  float mountainLine = 22.6;

  float coastSteep = 50.;
  float riftUpSteep = 1.2;
  float riftDownSteep = 1.;

  float riftDesertSlope = 1. - riftDownSteep / (40.*clamp(uv.y - riftLine, 0., 1.) + riftDownSteep);

  // Route
  float xc = 30. * (0.98 - 2. * uv.x); // Comme pour le rift, mais en plus serré
  float roadHeight = mix(37., hillsHeight, 1. - 1./(xc*xc + 1.));
  hillsHeight = mix(hillsHeight, roadHeight, smoothstep(4.5, 4.8, uv.y) * smoothstep(17., 15., uv.y));

  // Train
  float xd = 10. * (0. - 2. * uv.x); // Comme pour le rift, mais en plus serré
  float railHeight = mix(35., hillsHeight, 1. - 1./(xd*xd + 1.));
  hillsHeight = mix(hillsHeight, railHeight, smoothstep(14., 14.5, uv.y) * smoothstep(19., 18.6, uv.y));

  float y = underwaterHeight;
  y = mix(y, hillsHeight, clamp(coastSteep * (uv.y - coastLine), 0., 1.));
  y = mix(y, riftHeight, clamp(1.-riftUpSteep * (riftLine - uv.y), 0., 1.));
  y = mix(y, desertEntryHeight, riftDesertSlope);
  y = mix(y, desertHeight, smoothstep(0., 1., 5.*(uv.y - desertLine)));
  y = mix(y, mountainHeight, smoothstep(0., 1., 5.*(uv.y - mountainLine)));

  return vec3(0., y, 0.);
}

void exportCustomColor(float h, vec3 n)
{
  vColor = mix(vec4(0.8, 0.75, 0.6, 1.), aCol, smoothstep(14., 18., h) * smoothstep(0.25, 0.5, n.y));
}
