#version 330

uniform float	time;
uniform mat4	inverseModelViewProjectionMatrix;
uniform mat4	modelViewProjectionMatrix;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec2	invResolution;
uniform vec2	resolution;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor0;
layout(location = 1) out vec4 fragmentColor1;
//]

float rand(vec2 co) {
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void randPos(vec2 uv, out vec3 newPos, out vec3 newSpeed, out float death)
{
  float rnd0 = rand(uv);
  float rnd1 = rand(vec2(rnd0, 1.));
  float rnd2 = rand(vec2(rnd0, 1.1));

  float phi = 2. * 3.14159265358979 * rnd0;

  //newPos = 25.*(2.*vec3(uv.x, 1., uv.y)-1.);
  newPos = vec3(vec2(cos(phi), sin(phi)) * sqrt(rnd1), 5.).xzy;
  newSpeed = vec3(vec2(cos(phi), sin(phi)) * sqrt(rnd1) * 10., mix(10., 50., 1./((1.+rnd1)*(1.+rnd2)))).xzy;
  death = time + mix(.5, 5., rand(vec2(rnd0, 1.2)));
}

void applyPhysics(vec3 oldPos, vec3 oldSpeed, out vec3 newPos, out vec3 newSpeed)
{
  float dt = 16./1000.; // FIXME: uniform
  float g = 9.801;      //
  float damping = 0.5;  //

  vec3 a = vec3(0., -g, 0.);
  newSpeed = a * dt + oldSpeed;
  newPos = newSpeed * dt + oldPos;
  newSpeed *= pow(0.92, 0.001*dot(newSpeed, newSpeed));

  vec2 newUV = (modelViewProjectionMatrix * vec4(newPos, 1.0)).xy;
  vec3 newGround = (inverseModelViewProjectionMatrix * vec4(newUV, texture2D(texture2, newUV * .5 + .5).x * 2. - 1., 1.)).xyz;

  if (dot(newPos, newPos - newGround) < 0.)
  {
    vec2 oldUV = (modelViewProjectionMatrix * vec4(oldPos, 1.0)).xy;
    vec3 u = newGround - (inverseModelViewProjectionMatrix * vec4(oldUV, texture2D(texture2, oldUV * .5 + .5).x * 2. - 1., 1.)).xyz;
    normalize(u);

    vec2 thirdPoint = newUV + (newUV - oldUV).yx;
    vec3 v = newGround - (inverseModelViewProjectionMatrix * vec4(thirdPoint, texture2D(texture2, thirdPoint * .5 + .5).x * 2. - 1., 1.)).xyz;
    normalize(v);

    vec3 w = cross(u, v);
    vec3 groundNormal = mat3(inverseModelViewProjectionMatrix) * w;

    newPos += 2. * (newGround - newPos);
    newSpeed = damping * reflect(newSpeed, groundNormal);
  }
}

void main()
{
  vec3 oldPos = texture2D(texture0, texCoord).xyz;
  vec3 oldSpeed = texture2D(texture1, texCoord).xyz;
  vec3 newPos = oldPos;
  vec3 newSpeed = oldSpeed;
  float death = texture2D(texture1, texCoord).w;

  if (time == 0. || time >= death/* ||
      abs(newPos.x) > 20. ||
      abs(newPos.y) > 10. ||
      abs(newPos.z) > 20.*/)
  {
    randPos(texCoord, oldPos, oldSpeed, death);
  }
  applyPhysics(oldPos, oldSpeed, newPos, newSpeed);

  fragmentColor0 = vec4(newPos, 1.);
  fragmentColor1 = vec4(newSpeed, death);
}
