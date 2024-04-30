#if TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME
buildHillsHM,

#elif TEXTURE_EXPOSE == TEXTURE_FILE
__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

Channel * hillsHMTexture = NULL;

void buildHillsHM()
{
  hillsHMTexture = new Channel();
  Channel & t = *hillsHMTexture;
  t.Cells(10);
  t.AverageScale(6, 2.f);
  t.GaussianBlur();
  t.GaussianBlur();
  t.GaussianBlur();
  t.GaussianBlur();
  t.Scale(0.f, 1.f);

  queueTextureA(hillsHM, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT, false
		DBGARG(""));
}

#endif // TEXTURE_EXPOSE
