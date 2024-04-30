
#if TEXTURE_EXPOSE == TEXTURE_NAMES

testBump,
testBump2,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildBumpTest,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de bump
//
void buildBumpTest()
{
  Channel m;
  m.Conic(); m *= 2.f; m.Mod();
  m.ClampAndScale(0.9f, 0.8f);

  Channel c;
  c.Square();
  c.ClampAndScale(0.9f, 0.95f);
  c.Translate(-0.445f, -0.445f);

  Channel l = c;
  for (unsigned int i = 0; i < 6; ++i)
  {
    c.Translate(0.125f, 0);
    l += c;
  }
  c = l;
  l.Rotate90(); c += l;
  l.Rotate90(); c += l;
  l.Rotate90(); c += l;

  m += c;
  m.Scale(0, 1.);

  buildAndQueueBumpMapFromHeightMap(testBump, m);


  Channel h;
  h.Fade(0); h *= 20.f; h.Mod();
  h.ClampAndScale(0.1f, 0.2f);
  {
    Channel v = h;
    v.Rotate90();
    h *= v;
  }
  h.Scale(0, 1.f);

  buildAndQueueBumpMapFromHeightMap(testBump2, h);
}

#endif // TEXTURE_EXPOSE
