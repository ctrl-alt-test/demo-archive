uniform sampler2D cMap;

void main()
{
  gl_FragColor = texture2D(cMap, gl_TexCoord[0].xy);
}
