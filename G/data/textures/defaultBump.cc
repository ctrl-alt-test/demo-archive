
#if TEXTURE_EXPOSE == TEXTURE_NAMES

defaultBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildDefaultBump,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de speculaire
//
void buildDefaultBump()
{
  Texture::Channel t;
  t.Fade(0);
  t.Pow(1/6.f);

  Texture::Channel tmp = t;
  tmp.VerticalFlip();
  t += tmp;

  t.Scale(0, 1);
  tmp = t;
  tmp.Rotate90();

  t *= tmp;

  buildAndQueueBumpMapFromHeightMap(defaultBump, t);
}

#endif // TEXTURE_EXPOSE
