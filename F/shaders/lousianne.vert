
uniform int id;
uniform int time;
uniform float trans;

varying vec3 vViewVec;
//varying mat3 vLight;
varying vec3 vLight0;
varying vec3 vLight1;
//varying vec4 vColor;
varying vec3 vSpeed;

varying float fogFactor;

//[
mat3 computeTBN();
float getFogFactor(vec3 vVertex);
vec3 getLightTangent(int i, vec3 vVertex, mat3 tanSpace);
vec3 getSpeed();
void computeShadowCoord();
//]

void main()
{
  //  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vertex = gl_Vertex;
  vertex.y *= 1. + trans;

  mat3 tanSpace = computeTBN();

  vec3 vVertex = (gl_ModelViewMatrix * vertex).xyz;

  computeShadowCoord();

  // Vecteur incident dans l'espace tangent
  vLight0 = getLightTangent(0, vVertex, tanSpace);
  vLight1 = getLightTangent(1, vVertex, tanSpace);
/*
  vLight = (mat3(gl_LightSource[0].position.xyz,
		 gl_LightSource[1].position.xyz,
		 gl_LightSource[2].position.xyz) -
	    mat3(vVertex * gl_LightSource[0].position.w,
		 vVertex * gl_LightSource[1].position.w,
		 vVertex * gl_LightSource[2].position.w));
*/

  // Vecteur at
  vViewVec = -vVertex * tanSpace;

  fogFactor = getFogFactor(vVertex);
  gl_TexCoord[1] = vec4(vec2(0.5, 0.25 + 0.5 * float(id > 2)) + 2. * vertex.xz / vec2(500., 600.), 0., 1.);

  gl_Position = gl_ModelViewProjectionMatrix * vertex;
  vSpeed = getSpeed();
}
