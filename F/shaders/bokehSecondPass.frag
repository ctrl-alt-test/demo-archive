#version 120

uniform sampler2D cMap; // Render target 1
uniform sampler2D dMap; // Render target 2
uniform sampler2D nMap; // Depth map
uniform vec2 invResolution;
uniform float skip; // Avoid depth compare

//uniform vec4 params;
//#define bokeh_radius params.x
//#define bokeh_aperture params.z
#define bokeh_radius 0.01

// Number of bokeh samples
#define N 16.
#define NIEME (1./N)

void main()
{
  // Screencoord of target pixel
  vec2 sc = gl_FragCoord.xy * invResolution;
  vec2 ratio = vec2(1., invResolution.y / invResolution.x);

  // First direction: bottom left
  //
  //      /|.
  //     / | .
  //    /  |  .
  //   |   |  .
  //   |  / . .
  //   ||_   ..
  //   |/     .
  //
  vec4 c1=vec4(0.);
  vec2 d1=vec2(0.8660254, 0.5) * ratio * bokeh_radius;
  float sum1 = 0.;
  float refDepth = texture2D(nMap, sc + d1 * NIEME / 10.).x;
  for (float i = NIEME / 10.; i < 1.; i += NIEME)
  {
    vec2 pc = sc + d1 * i;
    float pdepth = texture2D(nMap, pc).x;
    vec4 pval = texture2D(cMap, pc);
    float CoC = pval.a;
    float contrib = step(i, CoC) * float(pdepth < refDepth + 0.01 + skip);

    c1 += contrib * pval;
    sum1 += contrib;
  }

  // Final direction: bottom right
  //
  //      .|\
  //     . | \
  //    .  |  \
  //   .   |   |
  //   .  / \  |
  //   . /   _||
  //   ./     \|
  //    \     /
  //     \   /
  //      \ /
  //       ¨
  vec4 c2=vec4(0.);
  vec2 d2=vec2(-0.8660254, 0.5) * ratio * bokeh_radius;
  float sum2 = 0.;
  refDepth = texture2D(nMap, sc + d2 * NIEME / 10.).x;
  for (float i = NIEME; i < 1.; i += NIEME)
  {
    vec2 pc = sc + d2 * i;
    float pdepth = texture2D(nMap, pc).x;
    vec4 pval = texture2D(dMap, pc);
    float CoC = pval.a;
    float contrib = step(i, CoC) * float(pdepth < refDepth + 0.01 + skip);

    c2 += contrib * pval;
    sum2 += contrib;
  }

  gl_FragData[0] = (c1 + 2. * c2) / (sum1 + 2. * sum2);


  /*
  // Code de debug pour visualiser les passes

  if (gl_FragCoord.x * invResolution.x > 0.33)
    gl_FragData[0] = c1 / sum1;

  if (gl_FragCoord.x * invResolution.x > 0.66)
    gl_FragData[0] = c2 / sum2;


  if (gl_FragCoord.y * invResolution.y > 0.5)
  {
    gl_FragData[0] = texture2D(cMap,sc);
    if (gl_FragCoord.x * invResolution.x > 0.5)
      gl_FragData[0] = texture2D(aMap,sc);
  }

  /*
  if (gl_FragCoord.y * invResolution.y > 0.8 ||
      gl_FragCoord.y * invResolution.y < 0.2)
  {
    gl_FragData[0] = texture2D(cMap,sc);
  }
  */
}
