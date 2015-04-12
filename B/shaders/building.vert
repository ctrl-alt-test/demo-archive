// -*- glsl -*-

varying vec3 vViewVecSpec;
varying vec3 vViewVecInv;
varying vec3 vLight;

varying vec2 texCoord;
varying mat3 tanSpace;
varying float bid;

attribute vec3 vTan;

void main(void)
{
  vec3 vVertex = (gl_ModelViewMatrix * gl_Vertex).xyz;

  texCoord = gl_MultiTexCoord0.xy;


  vec3 vNormal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 vTangent = normalize(gl_NormalMatrix * vTan);
  vec3 vBinormal = cross(vNormal, vTangent);
  tanSpace = mat3(vTangent, vBinormal, vNormal);

  vec3 id = gl_Color.r * gl_Normal;
  bid = id.x + 2. * id.y + 3. * id.z;

  // Vecteur incident dans l'espace tangent
  vLight = gl_LightSource[0].position.xyz - vVertex.xyz;

  // Vecteur at
  mat3 invTanSpace = transpose(tanSpace);
  vViewVecInv = normalize(invTanSpace * -vVertex.xyz);
  vViewVecSpec = normalize(-vVertex.xyz);

  gl_Position = ftransform();
}
