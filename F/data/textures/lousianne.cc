#if TEXTURE_EXPOSE == TEXTURE_NAMES

fluteOrnaments,
flute,
soufflet,
souffletBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildLousianne,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildLousianne()
{
  Channel t;
  Channel t2;

  t.Random();
  t2.Cells(20);
  t.Mix(0.5f, t2);
  t2.AverageScale(4, 1.75f);
  t2.ClampAndScale(0.f, 0.75f);
  t.Mix(0.25f, t2);
  t.Scale(0.5f, 0.8f);

  Channel holes;
  holes.Conic();
  holes.ZoomOut(7.f, 2.f);
  t2.Fade(0.f);
  t2.ClampAndScale(0.6f, 0.61f);
  holes *= t2;
  holes.ClampAndScale(0.71f, 0.7f);

  queueTextureRGB(fluteOrnaments,
                  t, t, t,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  holes.Translate(0.f, 0.77f);
  t *= holes;

  queueTextureRGB(flute,
                  t, t, t,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  t.Fade(0.f);
  t.Scale(0.f, 30.f);
  t.Mod();
  t.GaussianBlur();

  t2.Random();
  t2.VerticalMotionBlur(0.1f);
  t2.GaussianBlur();
  t2.Scale(0.3f, 1.f);
  t *= t2;

  buildAndQueueBumpMapFromHeightMap(souffletBump, t, true);
}
#endif
