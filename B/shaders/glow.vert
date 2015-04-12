// -*- glsl -*-

varying vec2 texCoord;

void main()
{
/*    vec2 inPos    = sign(gl_Vertex.xy); */
/*    gl_Position = vec4(inPos.xy, 0.0, 1.0); */
/*    texCoord.x  = 0.5 * ( 1.0 + inPos.x ); */
/*    texCoord.y  = 0.5 * ( 1.0 + inPos.y ); */


  gl_TexCoord[0] = gl_MultiTexCoord0;
  //  texCoord = gl_MultiTexCoord0.xy;
  gl_Position = ftransform();
}
