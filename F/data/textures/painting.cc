#if TEXTURE_EXPOSE == TEXTURE_NAMES

painting,
paintingBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildPainting,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildPainting()
{
  Channel t;
  Channel t2;
  Channel h;

  t.Fade(0.);
  t2.Random();
  t2.HorizontalMotionBlur(0.25);
  t.Mix(0.9f, t2);
  t.AverageScale(4, 0.1f);
  t.Scale(0.f, 1.f);

  t2.Diamond();
  t2.ClampAndScale(0.25f, 1.f);
  t2.Mult(3.f);
  t2.Sin();
  t.Waves(90, 0.15f, t2);

  h.Random();
  h.ClampAndScale(0, 0.2f);
  h.HorizontalMotionBlur(0.02f);
  h.Waves(90, 0.15f, t2);

  Channel r = t2; r.Scale(0.f, 0.4f);
  Channel g = t; g.Scale(0.f, 0.8f);
  Channel b = t; b.Scale(0.6f, 1.f);

  queueTextureRGB(painting,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, false);

  buildAndQueueBumpMapFromHeightMap(paintingBump, h, true);

  // c'est le même peintre qui s'est occupé du plafond.
  h.Scale(0.f, 0.125f);
  t.Random();
  h.Mix(0.1f, t);
  h.GaussianBlur();
  buildAndQueueBumpMapFromHeightMap(ceilBump, h, true);



  // Vasarely

  t.Conic();
  t.ZoomOut(12.f, 12.f);
  t.ClampAndScale(0.03f, 0.05f);
  t2.Square();
  t2.Scale(0.3f, 1.f);
  t *= t2;
  t.Scale(1.f, 0.f);
  Channel r2 = t; r2.Scale(0.26f, 0.86f);
  Channel g2 = t; g2.Scale(0.19f, 0.70f);
  Channel b2 = t; b2.Scale(0.11f, 0.46f);

  queueTextureRGB(paintingV,
                  r2, g2, b2,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);
}
#endif
