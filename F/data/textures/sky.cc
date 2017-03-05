#if TEXTURE_EXPOSE == TEXTURE_NAMES

sky,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildSky,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildSky()
{
  int width = 4;
  int height = 128;

  // 1 : couleur en haut
  // 2 : couleur médianne
  // 3 : couleur à l'horizon
  // H : position de la couleur médianne
  float r1 = 0.58f;
  float r2 = 0.93f;
  float r3 = 0.53f;
  float rh = 0.80f;

  float g1 = 0.76f;
  float g2 = 0.80f;
  float g3 = 0.53f;
  float gh = 0.51f;

  float b1 = 0.91f;
  float b2 = 0.52f;
  float b3 = 0.60f;
  float bh = 0.80f;

  Channel t(width, height);

  Channel r(width, height);
  r.Fade(0); r.ClampAndScale(0., rh); r.Pow(0.8f); r.Scale(r1, r2);
  t.Fade(0); t.ClampAndScale(1., rh); t.Pow(0.6f); t.Scale(r3, r2);
  r.MaximizeTo(t);

  Channel g(width, height);
  g.Fade(0); g.ClampAndScale(0., gh); g.Pow(0.2f); g.Scale(g1, g2);
  t.Fade(0); t.ClampAndScale(1., gh); t.Pow(0.8f); t.Scale(g3, g2);
  g.MaximizeTo(t);

  Channel b(width, height);
  b.Fade(0); b.ClampAndScale(0., bh); b.Pow(1.2f); b.Scale(b1, b2);
  t.Fade(0); t.ClampAndScale(1., bh); t.Pow(2.5f); t.Scale(b3, b2);
  b.MinimizeTo(t);

  queueTextureRGB(sky,
		  r, g, b,
		  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		  true, GL_REPEAT, false);
}
#endif
