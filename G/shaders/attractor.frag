#version 120

uniform sampler2D tex0; // color
uniform int time;
uniform vec2 resolution;
uniform vec2 invResolution;

// http://www.3d-meier.de/tut19/Seite13.html
vec3 Halvorsen(vec3 p, float dt)
{
  // Forme en triskel, très chouette. Ne pas mettre de dt trop grand
  // sinon ça devient instable.
  const float a = 1.4f;

  //const float dt = 0.002f;
  return p + dt * vec3(-a * p.x - 4.f * p.y - 4.f * p.z - p.y * p.y,
        	       -a * p.y - 4.f * p.z - 4.f * p.x - p.z * p.z,
        	       -a * p.z - 4.f * p.x - 4.f * p.y - p.x * p.x);
}

// http://www.3d-meier.de/tut19/Seite20.html
vec3 SprottLinz(vec3 p, float dt)
{
  //const float dt = 0.05f;
  return p + dt * vec3(p.y,
                       - p.x + p.y * p.z,
                       1 - p.y * p.y);
}

// http://www.3d-meier.de/tut19/Seite41.html
vec3 Thomas(vec3 p)
{
  const float b = 0.19f;

  const float dt = 0.06f;
  return p + dt * vec3(-b * p.x + sin(p.y),
		       -b * p.y + sin(p.z),
		       -b * p.z + sin(p.x));
}

// http://www.3d-meier.de/tut19/Seite15.html
vec3 Nose_Hoover(vec3 p)
{
  const float a = 1.5f;

  const float dt = 0.02f;
  return p + dt * vec3(p.y,
		       -p.x + p.y * p.z,
		       a - p.y * p.y);
}

float randA(vec2 co) { return fract(sin((co.x+co.y*1e3)*1e-3) * 1e5); }
float randB(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }

vec3 randPos(vec2 uv)
{
    vec2 rnd = vec2(randA(uv * resolution.x), randB(uv * resolution.y));
    vec2 jitter = 8. * invResolution * rnd;
    return vec3(0., 2. * (uv + jitter) - 1.);
}

void main()
{
  vec2 uv = gl_TexCoord[0].xy;

  vec3 oldPos = texture2D(tex0, uv).xyz;
  vec3 newPos = oldPos;

  const int launchDate = 15000;
  const int fadeInStart = launchDate + 4800;
  const int fadeInDuration = 3000;

  if (time < launchDate)
  {
    newPos = randPos(uv);
  }
  else
  {
    float thomasScale = 3.f;
    float noseHooverScale = 4.f;

    newPos = (time < 25470 ? Halvorsen(oldPos, 0.002) :
              (time < 34500 ? SprottLinz(oldPos, 0.08) :
               (time < 52500 ? Halvorsen(oldPos, 0.005) :
               (time < 75000 ? thomasScale * Thomas(oldPos / thomasScale) :
                noseHooverScale * Nose_Hoover(oldPos / noseHooverScale)))));

    if (abs(newPos.x) > 50. ||
	abs(newPos.y) > 50. ||
	abs(newPos.z) > 50.)
    {
      // FIXME: ici on voudrait aussi un rand sur x en fait
      newPos = randPos(uv);
    }
  }

  vec3 dp = newPos - oldPos;
  float normalizedSpeed = 1. - 1./(1. + 8.*length(newPos - oldPos));
  float fade = smoothstep(float(fadeInStart), float(fadeInStart + fadeInDuration), float(time));

  gl_FragColor = vec4(newPos, normalizedSpeed * fade);
}
