#if TEXTURE_EXPOSE == TEXTURE_NAMES

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildTrain,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildTrain()
{
  Channel t;
  // Bump des rails
  t.Fade(0);
  t.ClampAndScale(0.1f, 0.2f);
  t.Scale(3., 0.);
  buildAndQueueBumpMapFromHeightMap(railBump, t, true);


  // Panneau
  Channel t2;

  t.Flat(0.f);
  float width = 0.05f;
  float height = 0.25f;
  t.Line(0.f, 0.f, 0.f, height);
  t.Line(width, 0.f, width, height);
  t.Line(0.f, 0.f, width/2.f, -0.05f);
  t.Line(width/2.f, -0.05f, width, 0.f);
  t.Line(0.f, height, width, height);
  t.Translate(0.3f, 0.5f);

  Channel shape;
  int i;
  for (i = 0; i < 5; i++) {
    t.Translate(0.075f, 0.f);
    shape += t;
  }
  shape.FloodFill(1, 1, 1.f);

  t.Flat(0.f);
  t.Line(0.3f, 0.55f, 0.3f, 0.6f);
  t.Line(0.3f, 0.6f, 0.7f, 0.6f);
  t.Line(0.7f, 0.6f, 0.7f, 0.55f);
  t.Line(0.7f, 0.55f, 0.3f, 0.55f);
  t.FloodFill(1, 1, 1.f);
  t.Translate(0.f, 0.05f);
  shape *= t;
  t.Translate(0.f, -0.15f);
  shape *= t;

  Channel tr;
  tr.Flat(0.f);
  tr.Line(0.f, 1.f, 0.5f, 0.1f);
  tr.Line(0.5f, 0.1f, 1.f, 1.f);
  tr.Line(0.1f, 0.8f, 0.9f, 0.8f);
  tr.FloodFill(1, 1, 1.f);
  tr.Scale(0.f, 0.9f);

  t = shape;
  Channel b = shape; b -= tr;
  Channel g = shape; g -= tr;
  Channel r = tr; r += shape;
  queueTextureRGB(trainSign,
		  r, g, b,
		  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		  true, GL_REPEAT, true);
}

#endif // TEXTURE_EXPOSE
