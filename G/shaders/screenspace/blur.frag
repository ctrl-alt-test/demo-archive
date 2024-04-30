#version 120

uniform vec2 invResolution;
uniform vec2 center;
uniform float zNear;
uniform float zFar;

//
// Flou gaussien vertical
//
// Exemple d'utilisation :
// color = verticalGaussianBlur(tex0, gl_TexCoord[0].xy, 0.005);
//
vec4 directionalGaussianBlur(sampler2D tex, vec2 uv, vec2 spread)
{
  float weight[5] = float[5](0.2270270270,
			     0.1945945946,
			     0.1216216216,
			     0.0540540541,
			     0.0162162162);

  vec4 c = texture2D(tex, uv) * weight[0];
  for (int i = 1; i < 5; ++i)
  {
    vec2 offset = spread * float(i);
    c += texture2D(tex, uv + offset) * weight[i];
    c += texture2D(tex, uv - offset) * weight[i];
  }
  return c;
}

/*
//
// Fonctionne mais c'est vraiment moche je trouve...
// Flou gaussien en quatre points
//
// Exemple d'utilisation (à utiliser en plusieurs passes) :
// color = fourTapsGaussianBlur(tex0, gl_TexCoord[0].xy, numPass);
//
vec4 fourTapsGaussianBlur(sampler2D tex, vec2 uv, int pass)
{
  vec4 c = vec4(0.);

  vec2 pixelSize = invResolution;
  vec2 duv = pixelSize * float(pass) + 0.5 * pixelSize;

  c += texture2D(tex, uv + vec2(-duv.x, -duv.y));
  c += texture2D(tex, uv + vec2(-duv.x,  duv.y));
  c += texture2D(tex, uv + vec2( duv.x, -duv.y));
  c += texture2D(tex, uv + vec2( duv.x,  duv.y));

  return c / 4.;
}
*/

//
// Flou pour le light streak
// Calcule dans les deux directions opposées en parallèle
//
// Exemple d'utilisation (à utiliser en plusieurs passes) :
// directionalBlur(tex0, tex1, uv, vec2(invResolution.x, 0.), numPass, color0, color1);
//
void directionalBlur(sampler2D tex0, sampler2D tex1, vec2 uv, vec2 dir,
		     float power, int pass,
		     out vec4 color0, out vec4 color1)
{
  vec4 c0 = vec4(0.);
  vec4 c1 = vec4(0.);
  float sum = 0.;
  float b = pow(4., float(pass));
  for (int i = 0; i < 4; ++i)
  {
    // Le 0.25 est là pour augmenter la précision quand power tend vers 1
    float weight = pow(power, 0.25 * b * i);
    c0 += weight * texture2D(tex0, uv + b * i * dir);
    c1 += weight * texture2D(tex1, uv - b * i * dir);
    sum += weight;
  }
  color0 = c0/sum;
  color1 = c1/sum;
}

/*
//
// Flou axial
//
// Exemple d'utilisation :
// color = axialBlur(tex0, gl_TexCoord[0].xy, 0.05);
//
vec4 axialBlur(sampler2D tex, vec2 uv, float intensity)
{
  // FIXME : se comporte mal sur les bords
  vec2 dist = uv - center;
  vec2 offset = intensity * vec2(dist.y, -dist.x);
  vec4 c = vec4(0.);

  float i;
  float inc = 0.1;
  for(i = -1.; i <= 1.; i += inc)
  {
    c += 0.5 * inc * texture2D(tex, uv + i * offset);
  }
  return c;
}

//
// Flou radial
//
// Exemple d'utilisation :
// color = radialBlur(tex0, gl_TexCoord[0].xy, 0.05);
//
vec4 radialBlur(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = 2. * intensity * (center - uv);
  vec4 c = vec4(0.);

  float i;
  float inc = 0.05;
  for(i = 0.; i <= 1.; i += inc)
  {
    c += inc * texture2D(tex, uv + i * offset);
  }
  return c;
}
*/

//
// Flou de mouvement, d'après une velocity map
//
// Exemple d'utilisation :
// color = motionBlur(tex0, gl_TexCoord[0].xy, 1.);
//
vec3 motionBlur(sampler2D tex, sampler2D depth, sampler2D velocity,
		vec2 uv, float intensity)
{
  float depthHere = (2.0 * zNear) / (zNear + mix(zFar, zNear, texture2D(depth, uv).x));

  vec3 speedInfo = texture2D(velocity, uv).rgb;
  vec2 speed = (2. * speedInfo.xy - 1.) * (speedInfo.z + 0.001);
  vec2 offset = intensity * speed;
  vec3 c = vec3(0.);

  float inc = 0.2;
  float weight = 0.;
  for(float i = -1.; i <= 1.; i += inc)
  {
    vec2 there = uv + i * offset;
    float depthThere = (2.0 * zNear) / (zNear + mix(zFar, zNear, texture2D(depth, there).x));
    if (depthHere <= depthThere + 0.02)
    {
      c += texture2D(tex, clamp(uv + i * offset, vec2(0.), vec2(1., center.y / center.x))).rgb;
      weight += 1.;
    }
  }
  return c / weight;
}

//
// Flou de mouvement, avec abherration chromatique
//
// Exemple d'utilisation :
// color = motionBlur(tex0, gl_TexCoord[0].xy, 1., coord * 0.002);
//
vec3 motionBlurWithAbherration(sampler2D tex, sampler2D depth, sampler2D velocity,
			       vec2 uv, float intensity, vec2 aberration)
{
  float depthHere = (2.0 * zNear) / (zNear + mix(zFar, zNear, texture2D(depth, uv).x));

  vec3 speedInfo = texture2D(velocity, uv).rgb;
  vec2 speed = (2. * speedInfo.xy - 1.) * (speedInfo.z + 0.001);
  vec2 offset = intensity * speed;
  vec3 c = vec3(0.);

  float inc = 0.2;
  float weight = 0.;
  for (float i = -1.; i <= 1.; i += inc)
  {
    vec2 there = uv + i * offset;
    float depthThere = (2.0 * zNear) / (zNear + mix(zFar, zNear, texture2D(depth, there).x));
    if (depthHere <= depthThere + 0.02)
    {
      c += (vec3(1., 0., 0.) * texture2D(tex, clamp(there + aberration, vec2(0.), vec2(1., center.y / center.x))).r +
	    vec3(0., 1., 0.) * texture2D(tex, clamp(there, vec2(0.), vec2(1., center.y / center.x))).g +
	    vec3(0., 0., 1.) * texture2D(tex, clamp(there - aberration, vec2(0.), vec2(1., center.y / center.x))).b);
      weight += 1.;
    }
  }
  return c / weight;
}
