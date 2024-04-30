//
// Texture based hash functions.
//

// FIXME: Need to clarify the range of p; see hash2DTex(vec2) which doesn't divide by 256.

float hashTex(sampler2D randomTexture, float p)
{
  return texelFetch(randomTexture, ivec2(255. * p) % 256, 0).r;
}

float hashTex(sampler2D randomTexture, vec2 p)
{
  return texelFetch(randomTexture, ivec2(255. * p) % 256, 0).r;
}

float hashTex(sampler2D randomTexture, vec3 p)
{
  float h = texelFetch(randomTexture, ivec2(255. * p.yz) % 256, 0).r;
  return texelFetch(randomTexture, ivec2(255. * p.x, 255. * h) % 256, 0).r;
}

vec2 hash2DTex(sampler2D randomTexture, float p)
{
  float x = texelFetch(randomTexture, ivec2(255. * p) % 256, 0).r;
  return vec2(x, texelFetch(randomTexture, ivec2(255. * x), 0).r);
}

//vec2 hash2DTex(sampler2D randomTexture, vec2 p)
//{
//  return texelFetch(randomTexture, ivec2(255. * p) % 256, 0).rg;
//}

float valueNoiseTex(sampler2D randomTexture, vec3 p)
{
  vec3 p0 = floor(p);
  vec3 p1 = p0 + 1.;
  vec3 t = smoothstep(p0, p1, p);

  float h000 = hashTex(randomTexture, vec3(p0.x, p0.y, p0.z));
  float h001 = hashTex(randomTexture, vec3(p0.x, p0.y, p1.z));
  float h010 = hashTex(randomTexture, vec3(p0.x, p1.y, p0.z));
  float h011 = hashTex(randomTexture, vec3(p0.x, p1.y, p1.z));
  float h100 = hashTex(randomTexture, vec3(p1.x, p0.y, p0.z));
  float h101 = hashTex(randomTexture, vec3(p1.x, p0.y, p1.z));
  float h110 = hashTex(randomTexture, vec3(p1.x, p1.y, p0.z));
  float h111 = hashTex(randomTexture, vec3(p1.x, p1.y, p1.z));

  return
    mix(mix(mix(h000, h001, t.z),
            mix(h010, h011, t.z), t.y),
        mix(mix(h100, h101, t.z),
            mix(h110, h111, t.z), t.y), t.x);
}

float blueNoise(sampler2D randomTexture, vec2 p, float t)
{
  float h1 = texelFetch(randomTexture, ivec2(255. * t) % 256, 0).r;
  float h2 = texelFetch(randomTexture, ivec2(255. * p) % 256, 0).g;
  return texelFetch(randomTexture, ivec2(255. * h1, 255. * h2), 0).a;
}
