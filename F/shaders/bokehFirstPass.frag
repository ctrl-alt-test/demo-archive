#version 120

uniform sampler2D cMap; // Source image
uniform sampler2D nMap; // Depth map
uniform vec2 invResolution;
uniform float focus; // Focus plane
uniform float zNear;
uniform float zFar;
uniform float skip; // Avoid depth compare
uniform int time;

//uniform vec4 params;
//#define bokeh_radius params.x
//#define bokeh_aperture params.z
#define bokeh_radius 0.01
#define bokeh_aperture 0.8

// Number of bokeh samples
#define N 16.
#define NIEME (1./N)

float calcCoC(float z)
{
  float depth = (2.0 * zNear) / (zNear + mix(zFar, zNear, z));
  float c = abs((depth - focus) / focus * bokeh_aperture);
  return clamp(c, NIEME / 8., 1.);
}

void main(void)
{
  // Screencoord of target pixel
  vec2 sc = gl_FragCoord.xy * invResolution;
  vec2 ratio = vec2(1., invResolution.y / invResolution.x);

  // First direction: up
  //
  //      .|.
  //     . | .
  //    .  ^  .
  //   .   |   .
  //   .  . .  .
  //   . .   . .
  //   ..     ..
  //   ¨       ¨
  vec4 c1=vec4(0.);
  vec2 d1=vec2(0., -1.) * ratio * bokeh_radius;
  float sum1 = 0.;
  float refDepth = texture2D(nMap, sc + d1 * NIEME / 10.).x;
  for (float i = NIEME / 10.; i < 1.; i += NIEME)
  {
    // Read depth of source pixel
    vec2 pc = sc + d1 * i;
    float pdepth = texture2D(nMap, pc).x;
    float CoC = calcCoC(pdepth);
    vec4 pval = texture2D(cMap, pc);
    float contrib = step(i, CoC) * float(pdepth < refDepth + 0.01 + skip);

    pval.a = CoC;
    c1 += contrib * pval * (2. - texture2D(cMap, pc).a);
    sum1 += contrib;
  }
  gl_FragData[0] = c1 / sum1;

  // Second direction: down left
  //
  //      /|
  //     L |
  //    /  |
  //   |   |
  //   |  /
  //   | /
  //   |/
  //
  vec4 c2 = vec4(0.);
  vec2 d2=vec2(0.8660254, 0.5) * ratio * bokeh_radius;
  float sum2 = 0.;
  refDepth = texture2D(nMap, sc + d2 * NIEME / 10.).x;
  for (float i = NIEME; i < 1.; i += NIEME)
  {
    // Read depth of source pixel
    vec2 pc = sc + d2 * i;
    float pdepth = texture2D(nMap, pc).x;
    float CoC = calcCoC(pdepth);
    vec4 pval = texture2D(cMap, pc);
    float contrib = step(i, CoC) * float(pdepth < refDepth + 0.01 + skip);

    pval.a = CoC;
    c2 += contrib * pval * (2. - texture2D(cMap, pc).a);
    sum2 += contrib;
  }

  gl_FragData[1] = (c1 + c2) / (sum1 + sum2);


  /*
  // Code de debug

  if (gl_FragCoord.y * invResolution.y < 0.2)
  {
    float z = texture2D(nMap,sc).x;
    float CoC = calcCoC(z);
    gl_FragData[0] = vec4(vec3(CoC), 1.);
  }
  if (gl_FragCoord.y * invResolution.y > 0.8)
  {
    gl_FragData[0] = texture2D(cMap, sc);
  }
  */
}
