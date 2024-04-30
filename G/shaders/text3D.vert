#version 120

uniform int time;
uniform float trans;

varying vec3 vLightVec[2];
varying vec3 vViewVec;
varying vec3 vvPos;
varying vec4 vColor;

attribute float aId;

//[
mat3 computeTBN();
vec3 getLightTangentSpace(int i, vec3 vVertex, mat3 tanSpace);
void exportBaryCoords();
void exportDepth();
void exportFogFactor(vec3 vVertex);
void exportShadowCoord(vec4 vertex);
void exportSpeed(vec4 oldVertex, vec4 curVertex);
//]

float rand(vec2 v)
{
  return fract(sin(dot(v, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 rand3(vec3 v)
{
  // FIXME : vérifier que ça ne fait pas de motif
  float seed = dot(v.xyz, v.yzx);
  return fract(43758.5453 * sin(seed * vec3(12.9898, 78.233, 91.2228)));
}

float norm(vec2 v) { return sqrt(v.x*v.x+v.y*v.y); }

vec3 getStartingPos(vec3 inp)
{
  //////////////

  // 1.
  // float d = fract(norm(inp)*0.03);
  // vec2 outp = vec2(inp.x, inp.y)*d*3.0*vec2(1.0, 5.0);

  // 2.
  // float x = 110.0;
  // float f = 0.08;
  // vec2 outp = vec2(inp.x, inp.y)+vec2(x*sin(f*inp.y),x*cos(f*inp.x));
  // outp += rand3(vec3(aId, 1., 1.)).xy * 25.;

  // 3.
  // float d = fract(inp.y*inp.y*0.001);
  // float e = fract(inp.x*inp.x*0.001);
  // float x = 510.0*d;
  // float f = 100.0*0.8*e;
  // vec2 outp = vec2(inp.x, inp.y)*(d+e)*3.0*vec2(1.0, 5.0);

  // 4.
  float r = rand(vec2(aId, 1.)) * 2. - 0.5;
  return vec3(inp.x + r * 400., -400., 0.);
}

vec3 getPos()
{
  vec3 shift = vec3(114., 462., 0.);
  float endx = floor(mod(aId, 1000.));
  float endy = floor(aId / 1000.);
  vec3 end = -vec3(endx, endy, 0.) + shift;

  vec3 start = getStartingPos(end);
  vec3 pos = mix(start, end, clamp(trans + (0.04-endx/8000.), 0., 1.));

  // Forme des particules (carrés)
  vec3 rnd = rand3(vec3(aId*0.00001));
  rnd.z = pow(rnd.z, 10.);
  vColor = mix(vec4(0., 0., 0., 1.), vec4(500., 1500., 0., 1.), rnd.z);
  if (time > 254000)
    vColor = mix(vec4(1., 0.01, 0.02, 0.4), vec4(1., 0.8, 0.02, 0.01), rnd.z);
  vColor.rgb *= mix(1., 10., smoothstep(276200., 278000., float(time)));

  float size = mix(0.3, 1.4, rnd.x);
  pos.xy += gl_Vertex.xy * size;
  pos.z += size * 5.;
  return pos;
}

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;

  mat3 tanSpace = computeTBN();

  vec4 vertex = vec4(getPos(), gl_Vertex.w);
  vec3 vVertex = (gl_ModelViewMatrix * vertex).xyz;
  vvPos = vertex.xyz;

  // Vecteur incident dans l'espace tangent
  vLightVec[0] = getLightTangentSpace(0, vVertex, tanSpace);
  vLightVec[1] = getLightTangentSpace(1, vVertex, tanSpace);

  // Vecteur at dans l'espace tangent
  vViewVec = -vVertex * tanSpace;


  //  gl_TexCoord[1] = vec4(vec2(0.5, 0.25 + 0.5 * float(id > 2)) + 2. * vertex.xz / vec2(500, 600), 0., 1.);

  gl_Position = gl_ModelViewProjectionMatrix * vertex;
  exportDepth();
  exportShadowCoord(gl_Vertex);
  exportFogFactor(vVertex);
  exportBaryCoords();
  exportSpeed(vertex, vertex);
}
