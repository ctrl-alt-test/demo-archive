#version 120

uniform vec2 retroEdgeThick;
uniform vec4 retroEdgeColor;
uniform vec4 retroFaceColor;
uniform vec4 retroParams;
uniform float trans;
uniform int time;

varying float vFogFactor;
varying vec3 vSpeed;
varying vec3 vViewVec;
varying vec4 vColor;

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nViewVec;

float getEdge(float thickness, float alias, float flatness);
vec2 getTexCoord();
vec3 addFog(vec3 color, float intensity);
vec3 getAlbedo_tex();
vec3 getHDRcolor(vec4 rgbe);
vec3 getNormal();
vec3 getSpeedColor(vec3 speed);
void getLightShadingBlinnPhongVSM(out vec3 diffuseComponent, out vec3 specularComponent);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  vec3 diffuseComponent;
  vec3 specularComponent;
  getLightShadingBlinnPhongVSM(diffuseComponent, specularComponent);

  // FIXME: distinguer le cas emission du cas diffusion
  vec3 origAlbedo = getAlbedo_tex();
  vec3 faceAlbedo = mix(origAlbedo, getHDRcolor(retroFaceColor), retroParams.x);
  vec3 edgeAlbedo = mix(origAlbedo, getHDRcolor(retroEdgeColor), retroParams.y);

  vec3 shadedFaceColor = faceAlbedo * diffuseComponent + specularComponent;
  vec3 shadedEdgeColor = edgeAlbedo * diffuseComponent + specularComponent;

  float intensity = 20. + 80. * smoothstep(160000.,150000., float(time));
  vec3 emitColor = trans * intensity * vec3(1., .9, .8);
  vec3 faceColor = mix(faceAlbedo, shadedFaceColor, retroParams.z) + emitColor;
  vec3 edgeColor = mix(edgeAlbedo, shadedEdgeColor, retroParams.w);

  float edge = getEdge(retroEdgeThick.x, 0.42, retroEdgeThick.y);
  vec3 color = mix(faceColor, edgeColor, edge);

  gl_FragData[0] = vec4(addFog(color, vFogFactor), 1.);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
