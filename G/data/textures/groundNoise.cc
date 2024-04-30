
#if TEXTURE_EXPOSE == TEXTURE_NAMES

groundNoise,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildGroundNoise,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de speculaire
//
void buildGroundNoise()
{
  Texture::Channel t;
  t.Cells(15);

  Texture::Channel t2;
  t2.Random();
  t2.VerticalMotionBlur(0.01f);
  t2.GaussianBlur();
  t2.Scale(0.f, 0.5f);

  t += t2;
  t.AverageScale(5, 1.5f);

  t.Scale(0.f, 1.f);
  t.ClampAndScale(0.2f, 0.7f);
  queueTextureA(groundNoise, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT);
}

#endif // TEXTURE_EXPOSE
