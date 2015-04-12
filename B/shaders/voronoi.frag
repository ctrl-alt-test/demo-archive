// -*- glsl -*-

varying vec2 texCoord;

uniform int time;

float cells[12] = float[12](0., 0.4, 0.3, 0.6, 0.6, 0., 0.3, 0.5, 0.8, 0.6, 0.7, 0.8);

void main(void)
{
  float minf = 10.;
  float mins[3] = float[3](10., 10., 10.);
  vec2 shift[3];
  float dist = 0.01;
  shift[0] = texCoord + vec2(-dist, -dist);
  shift[1] = texCoord;
  shift[2] = texCoord + vec2(dist, dist);

  float t = abs(sin(float(time) / 1000.));
  for(int i = 0; i < 12; i+=2) {
    vec2 v = vec2(cells[i], cells[i+1]);
    if (i == 0) v.x += t;
    if (i == 4) v.y += t;
    mins[0] = min(distance(shift[0], v), mins[0]);
    mins[1] = min(distance(shift[1], v), mins[1]);
    mins[2] = min(distance(shift[2], v), mins[2]);
  }

  // emboss simplifié
  float col = - 2. * mins[0] + mins[1] + 2. * mins[2];
  col = pow(1. - col, 3.);
  gl_FragColor = vec4(col);
}
