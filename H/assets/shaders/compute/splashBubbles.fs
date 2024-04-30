#version 330

uniform float height;
uniform float init;
uniform float time;
uniform sampler2D randomTexture;
uniform sampler2D texture0;
uniform vec2 invResolution;
uniform vec2 resolution;
uniform vec3 volumeSize;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

#include "assets/shaders/randFunctions.fs"

vec3 randPos(vec2 uv)
{
  vec2 rand = texelFetch(randomTexture, ivec2(resolution * uv) % 256, 0).rg;
  float h = uv.y;
  return vec3(rand.x, mix(h, h*h*h*h*h*h, .5), rand.y);
}

float segment(vec2 p, vec2 a, vec2 b, out float h)
{
  vec2 ap = p - a;
  vec2 ab = b - a;
  h = clamp(dot(ap, ab) / dot(ab, ab), 0., 1.);
  return length(ap - ab * h);
}

void main()
{
  vec3 oldPos = texture2D(texture0, texCoord).xyz;
  vec3 newPos = oldPos;

  float offset = 0.;
  if (height > 0.)
  {
    newPos = mix(vec3(-2., 0., -3.5), vec3(2., -5., 3.5), randPos(texCoord));
    offset = 1.;
  }
  float curve = 2. / (1. - newPos.y);

  vec2 a = vec2(0., 2.);
  float h;
  float d = segment(newPos.xz, a, -a, h) / (1. + curve);
  float size = (clamp((d - 2.) / (.5 - 2.), 0., 1.) + mix(-0.25, 0.25, randPos(texCoord.yx).x)) *
    smoothstep(height - 2.f, height + 1.f, newPos.y) * clamp(-newPos.y, 0., 1.);;
  newPos.xz += offset * curve * (newPos.xz - mix(a, -a, h));

  if (size > 0.)
  {
    // Since it works from the previous position, this function is
    // counter intuitive. The parameter inside sin/cos changes the
    // curvature, and the parameter outside changes the speed.
    newPos.y += 0.003 * size;

    vec2 flowAway = mix(a, -a, h) - newPos.xz;

    newPos.xz += -0.001 * flowAway;
    float rotation = 20. * time + 6.283 * texCoord.x;
    float speed = mix(0., 0.01, size);
    newPos.x += speed * sin(rotation);
    newPos.z += speed * cos(rotation);
  }

  fragmentColor = vec4(newPos, size);
}
