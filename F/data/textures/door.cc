#if TEXTURE_EXPOSE == TEXTURE_NAMES

doorBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildDoor,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildDoor()
{
  Channel s;
  s.Square();

  float astop = 0.2f;
  float bstop = 0.25f;
  float cstop = 0.4f;
  float dstop = 0.45f;

  // Entre 0 et a, plat plein
  Channel frame = s;
  frame.ClampAndScale(astop, astop - 0.01f);

  // Entre a et b, meplat et arrondi
  Channel inner = s;
  inner.ClampAndScale(bstop, astop);
  inner.Pow(0.5f);
  inner.Scale(0.f, 0.8f);

  // Entre c et d, arrondi et nouveau meplat
  Channel outer = s;
  outer.ClampAndScale(cstop, dstop - 0.01f);
  outer.Pow(0.5f);
  outer.Scale(0.f, 0.55f);

  Channel up = s;
  up.Clamp(dstop - 0.01f, dstop);
  up.Scale(0.f, 0.2f);

  // Entre d et 1, pente douce
  Channel end = s;
  end.Clamp(dstop, 1.f);
  end.Scale(0.f, 0.25f);

  Channel t = frame;
  t += inner;
  t += outer;
  t += up;
  t += end;

  Channel grain;
  grain.Random();
  grain.VerticalMotionBlur(0.01f);
  grain.Scale(-0.1f, 0.1f);

  t += grain;

  buildAndQueueBumpMapFromHeightMap(doorBump, t, true);
}

#endif // TEXTURE_EXPOSE
