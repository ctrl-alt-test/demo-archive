#if TEXTURE_EXPOSE == TEXTURE_NAMES

mbCylinder,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildMusicBox,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildMusicBox()
{
  Channel t;
  Channel t2;

  t.Random();
  t2.Cells(20);
  t2.HorizontalMotionBlur(0.1f);
  t2.VerticalMotionBlur(0.1f);
  t.Mix(0.5f, t2);
  t2.AverageScale(4, 1.75f);
  t2.ClampAndScale(0.f, 0.75f);
  t.Mix(0.25f, t2);
  t.Scale(0.4f, 0.8f);
  t.Mix(0.25f, t2);
  t.Scale(0.8f, 1.f);
  Channel r; r.Flat(255.f/255.f); r *= t;
  Channel g; g.Flat(216.f/255.f); g *= t;
  Channel b; b.Flat( 91.f/255.f); b *= t;

  queueTextureRGB(mbCylinder,
		  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  t.Random();
  t.VerticalMotionBlur(0.04f);
  t.GaussianBlur();
  t.Scale(0.f, 0.5f);
  buildAndQueueBumpMapFromHeightMap(mbCylinderBump, t, true);
}

#endif // TEXTURE_EXPOSE
