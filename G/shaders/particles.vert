
uniform int id;
uniform int time;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform mat4 oldModelView;
uniform float trans;

varying vec3 vvPos;
varying vec4 vColor;
varying vec3 vSpeed;

attribute float aId;

//[
void exportDepth();
void exportShadowCoord(vec4 vertex);
void exportFogFactor(vec3 vVertex);
void exportBaryCoords();
//]

vec3 getBuildingClamp(float start, float end)
{
  float t = clamp((float(time) - start) / (end - start), 0., 1.);

  float buildingWidth = 30.;
  float buildingLength = 25.;
  float buildingHeight = 0.;

  vec3 maxLimit = vec3(80.);
  vec3 minLimit = 0.5 * vec3(buildingHeight, buildingWidth, buildingLength);
  return mix(minLimit, maxLimit, 1.-sin(1.5707963 * t));
}

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;

  float scale = 5.;

  // Attention : ces deux nombres apparaissent dans fbos.cc et particules.cc
  float cols = 512.;//256.;
  float lines = float(131072 / 512 /* 65536 / 256 */);
  float line = floor(aId / cols);
  float col = aId - line * cols;


  vec4 oldData = texture2D(tex7, vec2((col + 0.5) / cols, (line + 0.5) / lines));
  vec4 data = texture2D(tex6, vec2((col + 0.5) / cols, (line + 0.5) / lines));

  float rnd = fract(0.17*aId); // Magic number pour essayer de ne pas avoir de motif
  float intensity = mix(100., 500., data.w) * mix(0.015, 1., smoothstep(0.05, 0., rnd));
  float tr = smoothstep(1., 2., trans);
  intensity *= mix(1., 1.3, pow(tr, 2.));
  float hue = smoothstep(0., 16., length(data.xyz));
  vColor = intensity * vec4(mix(vec3(0.11, 0.26, 1.), vec3(1., 0.9, 0.2), hue), 1.);

  if (time > 192000)
  {
    vec3 limit = getBuildingClamp(192000., 207000.) / scale;
    data.xyz = clamp(data.xyz, -limit, limit);
    data.x += limit.x;

    oldData.xyz = clamp(oldData.xyz, -limit, limit);
    oldData.x += limit.x;

    vColor *= smoothstep(207000., 206600., float(time));
  }

  vec3 oldCenter = scale * oldData.yxz;
  vec3 center = scale * data.yxz;

  vec4 vertex = gl_ModelViewMatrix * vec4(center, 1.) + vec4(gl_Vertex.xy, 0., 0.);
  vec3 vVertex = vertex.xyz;

  // Je me souviens plus ce que c'était sensé faire :
  //gl_TexCoord[1] = vec4(vec2(0.5, 0.25 + 0.5 * float(id > 2)) + 2. * vertex.xz / vec2(500, 600), 0., 1.);

  gl_Position = gl_ProjectionMatrix * vertex;

  exportDepth();
  exportShadowCoord(vertex);
  //exportShadowCoord(center);
  exportFogFactor(vVertex);
  exportBaryCoords();

  // Export speed
  vec4 oldVertex = gl_ModelViewMatrix * vec4(oldCenter, 1.) + vec4(gl_Vertex.xy, 0., 0.);
  vec4 old = gl_ProjectionMatrix * oldVertex;
  vec4 cur = gl_ProjectionMatrix * vertex;
  vSpeed = vec3(cur.xy * old.w - old.xy * cur.w, old.w * cur.w);
}
