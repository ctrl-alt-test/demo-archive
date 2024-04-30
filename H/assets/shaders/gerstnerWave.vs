#version 330

const int i_NUM_LIGHTS = 5;

uniform float time;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lightPositions[i_NUM_LIGHTS];
uniform vec4 clipPlane;

//[
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
//]
out vec2 texCoord;
out vec3 normal;
out vec3 viewVec;
out vec3 lightVecs[i_NUM_LIGHTS];
out vec4 clipSpacePosition;

#define PI 3.1415926535

vec3 GerstnerWave(float t, vec2 X, float A, vec2 D)
{
  //
  // Gerstner function
  // http://http.developer.nvidia.com/GPUGems/gpugems_ch01.html
  //
  // X: initial 2D position
  // D: wave 2D direction
  // A: wave amplitude
  // t: time
  //
  // w: wave sharpness
  //   = sqrt(2pi.g / L)
  //   g: 9.801 m/s²
  // L: wavelength

  // Q: steepness, 0 .. 1/(w * A)
  // c: speed, c = sqrt(L * g / 2PI)

  float L = 12. * A;
  float w = 2. * PI / L;
  float c = sqrt(9.801 * 2. * PI / L);
  float Q = 1./(6.* w * 0.5 * A);

  float phi = w * dot(D, X) + c * t;
  float x = Q * 0.5 * A * D.x * cos(phi);
  float y = Q * 0.5 * A * D.y * cos(phi);
  float z =     0.5 * A * sin(phi);

  return vec3(x, z, y);
}

vec3 getDisplacement(vec2 X)
{
  // A: wave height, in meters (but they add up).
  // Wave length is set with lambda, in the GerstnerWave function.
  float A[8] = float[8]( 1.1, 1.0,
			 0.8, 0.7,
			 0.5, 0.4,
			 0.1, 0.1 );
  vec2 D[8] = vec2[8](vec2(-0.961,  0.276), // 286
		      vec2(-0.5,    0.866), // 330
		      vec2( 0.53,   0.848), // 32
		      vec2(-0.326, -0.945), // 199

		      vec2( 0.695, -0.719), // 136
		      vec2( 0.087, -0.996), // 175
		      vec2( 0.891,  0.454), // 63
		      vec2(-0.992,  0.122));// 277

  vec3 wave = vec3(0);
  for (int i = 0; i < 6; ++i)
  {
    wave += GerstnerWave(time, X, A[i], D[i]);
  }

  return wave;
}

vec3 computeNormal(vec2 p)
{
  vec2 pb = p + vec2(.05, .0);
  vec2 pc = p + vec2(.0, -.05);

  vec3 A = vec3(p.x, 0., p.y) +   getDisplacement(p);
  vec3 B = vec3(pb.x, 0., pb.y) + getDisplacement(pb);
  vec3 C = vec3(pc.x, 0., pc.y) + getDisplacement(pc);

  vec3 dx = normalize(B - A);
  vec3 dz = normalize(C - A);
  return normalize(cross(dx, dz));
}

void main()
{
  vec3 displacedVertex = vertexPosition + getDisplacement(vertexPosition.xz);

  texCoord = vertexTexCoord;
  normal = normalize(mat3(modelViewMatrix) * computeNormal(vertexPosition.xz));
  vec4 viewSpacePosition = modelViewMatrix * vec4(displacedVertex, 1.0);
  viewVec = -viewSpacePosition.xyz;

  lightVecs[0] = lightPositions[0].xyz - lightPositions[0].w * viewSpacePosition.xyz;
  lightVecs[1] = lightPositions[1].xyz - lightPositions[1].w * viewSpacePosition.xyz;
  lightVecs[2] = lightPositions[2].xyz - lightPositions[2].w * viewSpacePosition.xyz;
  lightVecs[3] = lightPositions[3].xyz - lightPositions[3].w * viewSpacePosition.xyz;
  lightVecs[4] = lightPositions[4].xyz - lightPositions[4].w * viewSpacePosition.xyz;

  clipSpacePosition = projectionMatrix * viewSpacePosition;
  gl_Position = clipSpacePosition;
  gl_ClipDistance[0] = dot(viewSpacePosition, clipPlane);
}
