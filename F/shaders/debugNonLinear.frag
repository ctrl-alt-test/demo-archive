uniform sampler2D cMap;
uniform float zNear;
uniform float zFar;

void main()
{
  vec4 c = texture2D(cMap, gl_TexCoord[0].xy);
  gl_FragColor = (2.0 * zNear) / (zNear + mix(vec4(zFar), vec4(zNear), c));
}
