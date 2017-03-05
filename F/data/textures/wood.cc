#if TEXTURE_EXPOSE == TEXTURE_NAMES

wood,
wood2,
chessBoard,
chessBoardBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildWood,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void queueWoodAlbedo(id textureId,
		     Channel * t,
		     float r1, float g1, float b1,
		     float r2, float g2, float b2)
{
  Channel r = *t; r.Scale(r1, r2);
  Channel g = *t; g.Scale(g1, g2);
  Channel b = *t; b.Scale(b1, b2);
  queueTextureRGB(textureId,
                  r, g, b,
                  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                  true, GL_REPEAT, false);
}

void buildWood()
{
  //setSize(512);
  Channel a;
  Channel t;
  Channel t2;
  Channel t3;

  t.Fade(0);
  t.Scale(0, 10.f);
  t.Sin();
  t.Pow(0.2f);

  Channel grain;
  grain.Random();
  grain.HorizontalMotionBlur(0.01f);
  /*
  t2.Random();
  t2.HorizontalMotionBlur(0.01f);
  t *= t2;
  */
  t *= grain;

  t2.Cells(5);
  t *= t2;

  t2.Fade(90);
  t2.Scale(0.f, 2.f);
  t2.Sin();
  t2.Pow(0.9f);

  t3.Perlin(5, 1, 1, 1);
  t2.Mix(0.05f, t3);

  t.Waves(90.f, 4.f, t2);

  // Bump
  t2 = t;
  t2.GaussianBlur();

  // Bois fin
  t3.Random();
  t3.VerticalMotionBlur(0.01f);
  t3 *= t2;
  t3.Scale(0, 0.2f);
  buildAndQueueBumpMapFromHeightMap(woodBump1, t3, true);

  // Bois grossier
  t3.Random();
  t3.Blur();
  t3.VerticalMotionBlur(0.01f);
  t3 *= t2;
  t3.Scale(0, 1.f);
  buildAndQueueBumpMapFromHeightMap(woodBump2, t3, true);

  // Moins marqué
  t3.Scale(0, 0.5f);
  buildAndQueueBumpMapFromHeightMap(woodBump3, t3, true);

  grain.ClampAndScale(0, 0.35f);
  grain.Waves(90.f, 4.f, t2);

  queueWoodAlbedo(wood, &t,
		  0.60f, 0.40f, 0.10f,
		  1.00f, 0.81f, 0.50f);

  // couleurs d'un noyer
  queueWoodAlbedo(wood_Noyer, &t,
		  0.17f, 0.10f, 0.07f,
		  0.40f, 0.27f, 0.11f);

  // Merisier clair
  queueWoodAlbedo(wood_MerisierClair, &t,
		  133.f/255.f,  72.f/255.f,  25.f/255.f,
		  195.f/255.f, 123.f/255.f,  65.f/255.f);

  // Merisier foncé
  queueWoodAlbedo(wood_MerisierFonce, &t,
		   95.f/255.f,  43.f/255.f,  22.f/255.f,
		  127.f/255.f,  74.f/255.f,  43.f/255.f);

  // Bois tendre non traité
  queueWoodAlbedo(wood_TendreFrais, &t,
		  191.f/255.f, 139.f/255.f,  88.f/255.f,
		  218.f/255.f, 174.f/255.f, 130.f/255.f);

  // Bois tendre sec
  queueWoodAlbedo(wood_TendreSec, &t,
		  191.f/255.f, 139.f/255.f,  88.f/255.f,
		  218.f/255.f, 174.f/255.f, 130.f/255.f);

  // Bois pauvre non traité
  queueWoodAlbedo(wood_Pauvre, &t,
		  164.f/255.f, 133.f/255.f, 107.f/255.f,
		  211.f/255.f, 178.f/255.f, 139.f/255.f);

  // Bois un peu sombre mais pas autant que le noyer, chêne ?
  queueWoodAlbedo(wood_Chene, &t,
		  141.f/255.f,  94.f/255.f,  52.f/255.f,
		  171.f/255.f, 124.f/255.f,  82.f/255.f);

  // Bois non traité, un peu riche, style lambris
  queueWoodAlbedo(wood_Lambris, &t,
		  162.f/255.f, 110.f/255.f,  63.f/255.f,
		  213.f/255.f, 166.f/255.f, 104.f/255.f);

  // ---------------------------------------------------------
  // Plateau de l'échiquier

  Channel r = t; r.Scale(0.17f, 0.4f);
  Channel g = t; g.Scale(0.1f, 0.27f);
  Channel b = t; b.Scale(0.07f, 0.11f);

  t.HorizontalFlip();
  t.Translate(0.1f, 0.1f);
  t2 = grain;
  t2.HorizontalFlip();
  t2.Translate(0.1f, 0.1f);
  Channel r2 = t; r2.Scale(0.87f, 0.91f);
  Channel g2 = t; g2.Scale(0.80f, 0.84f);
  Channel b2 = t; b2.Scale(0.72f, 0.78f);

  Channel board;
  board.Sinus(4, 4);
  board.ClampAndScale(-0.002f, 0.002f);
  /*
  t.Random();
  t.GaussianBlur();
  board.Waves(45, 0.01f, t);
  */

  r.Mask(board, r2);
  g.Mask(board, g2);
  b.Mask(board, b2);
  grain.Mask(board, t2);

  queueTextureRGB(chessBoard,
                  r, g, b,
                  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                  true, GL_CLAMP_TO_EDGE, true);

  buildAndQueueBumpMapFromHeightMap(chessBoardBump, grain, true);
}

#endif // TEXTURE_EXPOSE
