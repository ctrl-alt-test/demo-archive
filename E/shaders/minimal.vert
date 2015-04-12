uniform mat4 oldModelView;

attribute vec3 vTan; // Hotfix

varying mat3 vLight;
varying vec3 vNormal;
varying vec3 vTangent;

varying vec4 vColor;

void main(void)
{
  vColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
  vLight = (mat3(gl_LightSource[0].position.xyz,
		 gl_LightSource[1].position.xyz,
		 gl_LightSource[2].position.xyz) -
	    mat3(vVertex.xyz, vVertex.xyz, vVertex.xyz));
  vNormal = gl_NormalMatrix * gl_Normal;
  vTangent = gl_NormalMatrix * vTan;

  gl_Position = ftransform();
}
