// -*- glsl -*-

// GLSL version 1.20 minimum, pour avoir Transpose()
#version 120

uniform int id;

varying vec3 vViewVecSpec;
varying vec3 vViewVecInv;
//varying mat3 vLight;
varying vec3 vLight;

varying vec4 vColor;
varying mat3 tanSpace;

varying vec3 vSpeed;

varying vec4 vVertex;

varying float fogFactor;

mat3 computeTBN();
vec3 getSpeed();

//
// Fog exp2
//
float getFogfactor()
{
  gl_FogFragCoord = length(vVertex);
  return clamp(exp2(gl_Fog.density * gl_Fog.density *
		    gl_FogFragCoord * gl_FogFragCoord *
		    -1.442695), // log2
	       0., 1.);
}

void main(void)
{
  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  tanSpace = computeTBN();

  vVertex = gl_ModelViewMatrix * gl_Vertex;

  // Vecteur incident dans l'espace tangent
  vLight = (gl_LightSource[0].position.xyz -
	    vVertex.xyz * gl_LightSource[0].position.w);
/*
  vLight = (mat3(gl_LightSource[0].position.xyz,
		 gl_LightSource[1].position.xyz,
		 gl_LightSource[2].position.xyz) -
	    mat3(vVertex.xyz * gl_LightSource[0].position.w,
		 vVertex.xyz * gl_LightSource[1].position.w,
		 vVertex.xyz * gl_LightSource[2].position.w));
*/

  // Vecteur at
  mat3 invTanSpace = transpose(tanSpace);
  vViewVecInv = normalize(invTanSpace * -vVertex.xyz);

  vViewVecSpec = normalize(-vVertex.xyz);

  fogFactor = getFogfactor();
  gl_TexCoord[1] = vec4(vec2(0.5, 0.25 + 0.5 * float(id > 2)) + 2. * gl_Vertex.xz / vec2(500, 600), 0., 1.);

  gl_Position = ftransform();
  vSpeed = getSpeed();
}
