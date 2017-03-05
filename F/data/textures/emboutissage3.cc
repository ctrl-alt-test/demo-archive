
#if TEXTURE_EXPOSE == TEXTURE_NAMES

emboutissage3Bump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildEmboutissage3,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de relief d'emboutissage 3
//
// http://upload.wikimedia.org/wikipedia/commons/0/0c/ISS_Quest_airlock.jpg
//
void buildEmboutissage3()
{
  Channel t;
  t.Square();
  t.ClampAndScale(1.f, 0.2f);

  Channel tmp;
  tmp.Fade(45);
  tmp.Clamp(0.5f, 1.f);
  tmp.Scale(0, 2.f); tmp.Mod();
  tmp.Scale(0.28f, 1.f);

  t.MinimizeTo(tmp);
  tmp.HorizontalFlip();
  t.MinimizeTo(tmp);

  t.ClampAndScale(0.85f, 0.95f);

  tmp.Square();
  tmp.ClampAndScale(0.1f, 0.2f);
  t.MaximizeTo(tmp);

  motionMotion(t, 0.02f, 0.02f);

  t.ClampAndScale(0.3f, 0.7f);

  buildAndQueueBumpMapFromHeightMap(emboutissage3Bump, t, true);
}

#endif // TEXTURE_EXPOSE
