
#if TEXTURE_EXPOSE == TEXTURE_NAMES

emboutissage2Bump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildEmboutissage2,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de relief d'emboutissage 2
//
// http://upload.wikimedia.org/wikipedia/commons/0/0c/ISS_Quest_airlock.jpg
//
void buildEmboutissage2()
{
  Channel t;
  t.Fade(0);
  t.Clamp(0.1f, 0.9f);
  t.Scale(0, 2.f); t.Mod();
  t.ClampAndScale(0.1f, 0.2f);

  Channel tmp;
  tmp.Fade(90);
  tmp.Clamp(0.125f, 0.875f);
  tmp.Scale(0, 4.f); tmp.Mod();
  tmp.ClampAndScale(0.05f, 0.25f);
  t.MaximizeTo(tmp);
  motionMotion(t, 0.01f, 0.01f);
  t.ClampAndScale(0.3f, 0.7f);

  tmp.Square();
  tmp.ClampAndScale(0.2f, 0.25f);
  motionMotion(tmp, 0.01f, 0.01f);
  tmp.ClampAndScale(0.3f, 0.7f);
  t += tmp;

  tmp.Square();
  tmp.ClampAndScale(0.1f, 0.15f);
  motionMotion(tmp, 0.01f, 0.01f);
  tmp.ClampAndScale(0.3f, 0.7f);
  t += tmp;

  t.Scale(0, 1.f);

  buildAndQueueBumpMapFromHeightMap(emboutissage2Bump, t, true);
}

#endif // TEXTURE_EXPOSE
