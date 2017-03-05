#if TEXTURE_EXPOSE == TEXTURE_NAMES

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildClock,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildClock()
{
  int size = 256;
  Channel t(size, size);
  t.Radial();
  t.Mult(24.f);
  t.Mod();
  t.ClampAndScale(0.f, 0.05f);

  // pour effacer les rayons au centre et au bord
  Channel c(size, size);
  c.Conic(); c.ClampAndScale(0.2f, 0.4f); t.MinimizeTo(c);
  c.Conic(); c.ClampAndScale(0.2f, 0.1f); t.MinimizeTo(c);

  // les yeux du chat
  Channel eye(size, size);
  eye.Conic(); eye.ClampAndScale(0.9f, 0.88f);
  eye.Translate(-0.2f, -0.15f); t *= eye;
  eye.Translate( 0.4f,  0.f  ); t *= eye;

  c.Random(); c.GaussianBlur();
  t.Mix(0.25f, c);
  t.Translate(0.5f, 0.5f);

  queueTextureRGB(clock,
                  t, t, t,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);
}
#endif
