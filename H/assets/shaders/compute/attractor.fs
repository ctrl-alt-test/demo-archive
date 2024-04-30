#version 330

uniform float time;
uniform sampler2D texture0;
uniform vec2 invResolution;
uniform vec2 resolution;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

float randA(vec2 co) { return fract(sin((co.x+co.y*1e3)*1e-3) * 1e5); }
float randB(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }

vec3 randPos(vec2 uv)
{
    vec2 rnd = vec2(randA(uv * resolution.x), randB(uv * resolution.y));
    vec2 jitter = 8. * invResolution * rnd;
    return vec3(0., 2. * (uv + jitter) - 1.);
}

// http://www.3d-meier.de/tut19/Seite13.html
vec3 Halvorsen(vec3 p)
{
  // Forme en triskel, très chouette. Ne pas mettre de dt trop grand
  // sinon ça devient instable.

  const float a = 1.4f;

  const float dt = 0.002f;
  return p + dt * vec3(-a * p.x - 4.f * p.y - 4.f * p.z - p.y * p.y,
		       -a * p.y - 4.f * p.z - 4.f * p.x - p.z * p.z,
		       -a * p.z - 4.f * p.x - 4.f * p.y - p.x * p.x);
}

void main()
{
  vec3 oldPos = texture2D(texture0, texCoord).xyz;
  vec3 newPos = oldPos;

  if (time < 0.1)
  {
    newPos = randPos(texCoord);
  }
  else
  {
    newPos = Halvorsen(oldPos);

    if (abs(newPos.x) > 50. ||
	abs(newPos.y) > 50. ||
	abs(newPos.z) > 50.)
    {
      // FIXME: ici on voudrait aussi un rand sur x en fait
      newPos = randPos(texCoord);
    }
  }
  float normalizedSpeed = 1. - 1./(1. + 8. * length(newPos - oldPos));

  fragmentColor = vec4(newPos, normalizedSpeed);
}
