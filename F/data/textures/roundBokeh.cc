
#if TEXTURE_EXPOSE == TEXTURE_NAMES

roundBokeh,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildRoundBokeh,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Bokeh parfaitement circulaire
//
void buildRoundBokeh()
{
  const float border = 0.03f;

  Texture::Channel t(256, 256);
  t.Conic();
  t.ClampAndScale(0.24f, 0.24f + border);
  t.Pow(2.f);

  Texture::Channel f(256, 256);
  f.Conic();
  f.ClampAndScale(1.f, 0.24f + border);
  f.Pow(10.f);
  f.Scale(0.8f, 1.f);

  t *= f;

  queueTextureA(roundBokeh, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_CLAMP);
}

#endif // TEXTURE_EXPOSE
