
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
  f.Scale(0.4f, 1.f);

  Texture::Channel g(256, 256);
  g.Perlin(40, 1, 0, 0);
  g.Scale(-0.8f, +0.8f);
  f += g;

  t *= f;
  t.HorizontalMotionBlur(0.02f);
  t.VerticalMotionBlur(0.02f);
  t.HorizontalMotionBlur(0.01f);
  t.VerticalMotionBlur(0.01f);

  g.Random();
  g.GaussianBlur();
  g.Scale(0.8f, 1.f);
  t *= g;

  queueTextureA(roundBokeh, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_CLAMP, false
		DBGARG("roundBokeh"));
}

#endif // TEXTURE_EXPOSE
