#if TEXTURE_EXPOSE == TEXTURE_NAMES

tire,
tireBump,
tireSpecular,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildTire,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Pneu, à plaquer sur un tore
//
void buildTire()
{
  float angle = 0.f;
  float lines = 16.f;
  float fringes = 256.f;
  float zigzag = 64.f;
  float valley = 0.2f;
  float hill = 0.4f;

  Channel diag1;
  diag1.Fade(90.f);

  Channel lr;
  lr.Fade(angle); lr.Mult(zigzag); lr.Mod();
  lr.Scale(0, 0.02f);

  Channel pattern = diag1;
  pattern += lr;

  pattern.Mult(12.f * lines); pattern.Mod();
  pattern.ClampAndScale(valley, hill);

  Channel fringe;
  fringe.Fade(0); fringe.Mult(fringes); fringe.Mod();
  fringe.ClampAndScale(0.4f, 0.8f);

  Channel fringeOrBorder;
  fringeOrBorder.Fade(90); fringeOrBorder.Scale(0, 4.f); fringeOrBorder.Mod();
  fringeOrBorder.ClampAndScale(0.94f, 0.95f);

  fringe *= fringeOrBorder;

  Channel border;
  border.Fade(90); border.Mult(32.f); border.Mod();
  border.ClampAndScale(0.9f, 0.95f);
  fringe += border;

  fringe.Scale(0, 0.1f);

  Channel shapeOrBorder;
  shapeOrBorder.Fade(90.f); shapeOrBorder.Mult(2.f); shapeOrBorder.Mod();
  shapeOrBorder.ClampAndScale(0.75f, 0.74f);

  Channel shape = pattern;
  shape.Mask(shapeOrBorder, fringe);

  buildAndQueueBumpMapFromHeightMap(tireBump, shape, true);


  Channel noise;
  noise.Random();
  noise.Pow(2.f);
  noise.Scale(0, 0.2f);
  noise *= shapeOrBorder;

  queueTextureA(tireSpecular,
		noise,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT);


  shape.Scale(0.35f, 0.7f);

  queueTextureRGB(tire,
		  shape, shape, shape,
		  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		  true, GL_REPEAT);
}

#endif // TEXTURE_EXPOSE
