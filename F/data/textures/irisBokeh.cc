
#if TEXTURE_EXPOSE == TEXTURE_NAMES

irisBokeh,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildIrisBokeh,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Bokeh polygonal, en forme de diafragme
//
void buildIrisBokeh()
{
  int size = 256;
  int blades = 9;
  float shift = 15.f;
  float angle = 360.f / blades;
  float border = 0.1f;

  Channel t(size, size);
  t.Flat(1.f);

  Channel f(size, size);
  for (int i = 0; i < blades; ++i)
  {
    f.Fade(i * angle + shift);
    f.ClampAndScale(0.12f, 0.12f + border);
    f.Pow(2.f);

    t *= f;
  }

  Channel t2(size, size);
  for (int j = 0; j < blades; ++j)
  {
    f.Fade(j * angle + shift);
    f.ClampAndScale(1.f, 0.12f + border);
    f.Pow(10.f);
    t2.MinimizeTo(f);
  }
  t2.Scale(0.8f, 1.f);
  t2.HorizontalMotionBlur(0.02f);
  t2.VerticalMotionBlur(0.02f);
  t2.HorizontalMotionBlur(0.02f);
  t2.VerticalMotionBlur(0.02f);

  t *= t2;

  queueTextureA(irisBokeh, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_CLAMP, false);


  // Grain pour le post-processing
  Channel grain;
  grain.Random();
  grain.Blur();
  grain.Scale(0., 1.);

  queueTextureA(grainMap,
                  grain,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, false);
}

#endif // TEXTURE_EXPOSE
