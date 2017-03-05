
#if TEXTURE_EXPOSE == TEXTURE_NAMES

emboutissage1Bump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildEmboutissage1,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de relief d'emboutissage 1
//
// http://upload.wikimedia.org/wikipedia/commons/a/a7/Kibo_PM_and_ELM-PS.jpg
//
void buildEmboutissage1()
{
  Channel h;
  h.Fade(0); h *= 2; h.Mod();
  h.Clamp(0.1f, 0.6f);

  h.Scale(0, 4.f); h.Mod();
  h.ClampAndScale(0.75f, 1.f);

  Channel tmp;
  tmp.Fade(90);
  tmp.Clamp(0.04f, 0.96f);
  tmp.Scale(0, 6.f); tmp.Mod();
  tmp.ClampAndScale(0.9f, 1.f);

  h.MinimizeTo(tmp);

  tmp.Square();
  tmp.ClampAndScale(0.05f, 0.1f);

  h.MaximizeTo(tmp);
  h.Pow(0.7f);
  h.GaussianBlur();


  Channel c;
  c.Conic();

  c.ClampAndScale(0.98f, 0.995f);
  c.Pow(0.5f);
  c.Scale(0, 0.2f);
  c.Translate(-0.445f, -0.445f);

  Channel cc = c;
  for (int i = 0; i < 18; ++i)
  {
    c.Translate(0.05f, 0);
    cc += c;
  }
  c = cc;
  cc.Rotate90(); c += cc;
  cc.Rotate90(); c += cc;
  cc.Rotate90(); c += cc;

  h.MinimizeTo(c);

  buildAndQueueBumpMapFromHeightMap(emboutissage1Bump, h, true);
}

#endif // TEXTURE_EXPOSE
