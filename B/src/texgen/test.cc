// ============================================================================
//
// Texture generator tests
//
// ============================================================================

#include		<iostream>
#include		<cstdio>
#include		"texture.hh"

static const char * file = "test.raw";

Texture Cells100;

Texture PerlinHarmonics[8];

Texture red_channel;
Texture green_channel;
Texture blue_channel;

Texture test;
Texture testbis;
Texture tex1;
Texture tex2;
Texture tex3;


static void		RAW(const Texture & r,
			    const Texture & g,
			    const Texture & b)
{
  const unsigned int size = 3 * r.Width() * r.Height();
  const unsigned char * render = RGBBuffer(r, g, b);

  FILE *fd = fopen(file, "ab");
  if (0 == fd)
    return;

  if (size != fwrite(render, 1, size, fd))
    std::cerr << "Failed to dump texture! T_T" << std::endl;

  fclose (fd);
}

// Methode un peu space...
static void		RAW(const Texture & a)
{
  const unsigned int size = 3 * a.Width() * a.Height();
  const unsigned char * render = RGBBuffer(a, a, a);

  FILE *fd = fopen(file, "ab");
  if (0 == fd)
    return;

  if (size != fwrite(render, 1, size, fd))
    std::cerr << "Failed to dump texture! T_T" << std::endl;

  fclose (fd);
}

// #warning "[1;33mFIXME : Faire du test unitaire de chaque fonction.[0;39m"

// static void		Perlin ()
// {
//   test.Perlin (8, 1, 1, 0);

//   RAW(test);
// }

// static void		Perlin_sum(bool detail = false)
// {
//   test.Perlin (128, 1, 1, 0);
//   for (int i = 64; i > 0; i = i / 2)
//     {
//       testbis.Perlin (i, 1, 1, 0);
//       test += testbis;
//     }
//   test.Scale(0, 1);

//   RAW(test);
// }

static void		Cloud1(bool detail = false)
{
  test.Clear();

  unsigned int factor = 1;
  for (unsigned int i = 0; i < 8; ++i)
  {
    testbis = PerlinHarmonics[i];
    testbis /= (float)factor;
    test += testbis;
    factor <<= 1;
  }

  if (detail)
    RAW(test);

  test.Scale(0.f, 1.f);

  if (detail)
    RAW(test);

  test.Scale(-0.5f, 1.f);

  RAW(test);
}

static void		Cloud2(bool detail = false)
{
  test.Perlin(1, 2, 0, 0);
  testbis.Perlin(2, 2, 0, 0);
  testbis /= 2.f;
  test += testbis;

  unsigned int factor = 4;
  for (unsigned int i = 2; i < 8; ++i)
  {
    testbis = PerlinHarmonics[i];
    testbis /= (float)factor;
    test += testbis;
    factor <<= 1;
  }

  if (detail)
    RAW(test);

  test.Scale(0.f, 1.f);

  if (detail)
    RAW(test);

  test.Scale(-0.5f, 1.f);

  RAW(test);
}

static void		Cloud3(bool detail = false)
{
  test.Clear();

  unsigned int factor = 1;
  for (unsigned int i = 0; i < 8; ++i)
  {
    testbis = PerlinHarmonics[i];
    testbis.Abs();
    testbis /= (float)factor;
    test += testbis;
    factor <<= 1;
  }

  if (detail)
    RAW(test);

  test.Scale(0.f, 1.f);

  if (detail)
    RAW(test);

  test.Scale(-0.5f, 1.f);

  RAW(test);
}

static void		Cloud4(bool detail = false)
{
  test.Perlin(1, 2, 0, 0);
  testbis.Perlin(2, 2, 0, 0);
  testbis /= 2.f;
  test += testbis;

  unsigned int factor = 4;
  for (unsigned int i = 2; i < 8; ++i)
  {
    testbis = PerlinHarmonics[i];
    testbis.Abs();
    testbis /= (float)factor;
    test += testbis;
    factor <<= 1;
  }

  if (detail)
    RAW(test);

  test.Scale(0, 1);

  if (detail)
    RAW(test);

  test.Scale(-0.5f, 1.f);

  RAW(test);
}

// static void		Cloud5 ()
// {
//   test.Perlin (128, 1, 1, 0);
//   for (int i = 64; i > 4; i = i / 2)
//   {
//     testbis.Perlin (i, i < 4 ? 2 : 1, 0, 0);
//     test += testbis;
//   }
//   test.Scale(0, 1);
//   RAW(test(), test(), test());
// }

static void		Lava(bool detail = false)
{
  testbis = PerlinHarmonics[7];
  testbis.Scale(-0.5f, 0.5f);
  testbis += PerlinHarmonics[6];
  testbis.Scale(-0.5f, 0.5f);
  testbis += PerlinHarmonics[5];
  testbis.Scale(-1.f, 1.f);

  if (detail)
    RAW(testbis);

  testbis.Abs();

  if (detail)
    RAW(testbis);

  test = PerlinHarmonics[4];
  test.Scale(0.f, 1.f);

  testbis *= test;

  if (detail)
    RAW(testbis);


  test = Cells100;

  if (detail)
    RAW(test);

  test *= test;

  if (detail)
    RAW(test);


  testbis *= test;
  testbis.Limit(0, 1);

  if (detail)
    RAW(testbis);

  testbis.Scale(0.f, 0.5f);

  if (detail)
    RAW(testbis);


  test += testbis;
  test.Scale(0.f, 1.f);

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(0.f, 1.64f);
  green_channel.Scale(-2.f, 1.f);
  blue_channel.Scale(-3.f, 0.75f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		Cave(bool detail = false)
{
  testbis = PerlinHarmonics[7];
  testbis.Scale(-0.5f, 0.5f);
  testbis += PerlinHarmonics[6];
  testbis.Scale(-0.5f, 0.5f);
  testbis += PerlinHarmonics[5];
  testbis.Scale(-1.f, 1.f);

  if (detail)
    RAW(testbis);

  testbis.Abs();

  if (detail)
    RAW(testbis);

  testbis.Scale(1.f, -4.f);

  if (detail)
    RAW(testbis);


  test = Cells100;

  if (detail)
    RAW(test);

  test *= test;

  if (detail)
    RAW(test);


  test *= testbis;
  test.Limit(0, 1);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(0.f, 0.5f);
  green_channel.Scale(0.f, 0.8f);
  blue_channel.Scale(0.f, 1.f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		Wood(bool detail = false)
{
  test.Perlin(4, 10, 0, 0);
  test.Scale(0.f, 1.f);

  if (detail)
    RAW(test);

  test.Cut();
  test.Cut();
  test.Cut();

  if (detail)
    RAW(test);

  testbis = PerlinHarmonics[1];
  testbis += PerlinHarmonics[6];
  testbis += PerlinHarmonics[7];
  testbis.Scale(0.95f, 1.f);

  test *= testbis;

  if (detail)
    RAW(test);

  test.Blur();

  if (detail)
    RAW(test);

  testbis.Random();

  if (detail)
    RAW(testbis);

  testbis.HorizontalMotionBlur(10);

  if (detail)
    RAW(testbis);

  testbis.Blur ();

  if (detail)
    RAW(testbis);

  testbis.Scale(0.f, 0.2f);

  if (detail)
    RAW(testbis);

  test += testbis;

  test.Scale(0.f, 1.f);
  test.Blur();

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(0.25f, 1.f);
  green_channel.Scale(0.125f, 0.75f);
  blue_channel.Scale(0.f, 0.5f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		TestGradient(bool detail = false)
{
  red_channel.Gradient(1.f, 0, 0, 1.f);
  green_channel.Gradient(1.f, 0, 1.f , 0);
  blue_channel.Gradient(0, 1.f, 0, 0);

  if (detail)
    RAW(red_channel);
  if (detail)
    RAW(green_channel);
  if (detail)
    RAW(blue_channel);

  RAW(red_channel, green_channel, blue_channel);
}

static void		Fireball(bool detail = false)
{
  test.Clear();
  for (unsigned int i = 0; i < 7; ++i)
  {
    test += PerlinHarmonics[7 - i];
    test.Scale(0.f, 1.f);
  }
  test.Scale(0.1f, 1.f);

  if (detail)
    RAW(test);

  testbis.Conic();//128, 0);
  testbis.Scale(-0.5f, 3.f);

  if (detail)
    RAW(testbis);

  test *= testbis;

  if (detail)
    RAW(test);

  test.Limit(0, 1);
  test *= test;

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(-0.05f, 1.64f);
  green_channel.Scale(-2.f, 2.f);
  blue_channel.Scale(-1.75f, 1.f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		PurplePlasmaball(bool detail = false)
{
  test.Clear();
  for (unsigned int i = 0; i < 4; ++i)
  {
    test += PerlinHarmonics[i];
    test.Scale(0.f, 1.f);
  }
  test.Scale(0.1f, 1.f);

  if (detail)
    RAW(test);

  testbis.Conic();//128, 0);
  testbis.Scale(-0.5f, 3.f);

  if (detail)
    RAW(testbis);

  test *= testbis;

  if (detail)
    RAW(test);

  test.Limit(0, 1);
  test *= test;

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(-0.12f, 1.81f);
  green_channel.Scale(-0.72f, 1.f);
  blue_channel.Scale(0.f, 1.417f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		GreenPlasmaball(bool detail = false)
{
  test.Clear();
  for (unsigned int i = 0; i < 5; ++i)
  {
    test += PerlinHarmonics[i];
    test.Scale(0.f, 1.f);
  }
  test.Scale(0.1f, 1.f);

  if (detail)
    RAW(test);

  testbis.Conic();//128, 0);
  testbis.Scale(-0.5f, 3.f);

  if (detail)
    RAW(testbis);

  test *= testbis;

  if (detail)
    RAW(test);

  test.Limit(0, 1);
  test *= test;

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(-0.158f, 1.33f);
  green_channel.Scale(0.f, 1.9f);
  blue_channel.Scale(-0.41f, 1.f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		WhitePlasmaball(bool detail = false)
{
  test.Clear();
  for (unsigned int i = 0; i < 6; ++i)
  {
    test += PerlinHarmonics[i];
    test.Scale(0.f, 1.f);
  }
  test.Scale(0.1f, 1.f);

  if (detail)
    RAW(test);

  testbis.Conic();//128, 0);
  testbis.Scale(-0.5f, 3.f);

  if (detail)
    RAW(testbis);

  test *= testbis;

  if (detail)
    RAW(test);

  test.Limit(0, 1);
  test *= test;

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  green_channel.Scale(0.f, 1.149f);
  blue_channel.Scale(0.f, 1.226f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		Grass(bool detail = false)
{
  test.Clear();
  testbis.Clear();
  test.AddCuts(1500, 10);
  test.Blur();

  if (detail)
    RAW(test);

  for (unsigned char length = 11; length < 14; ++length)
  {
    testbis.AddCuts(1500, length);
    testbis.Blur();
    test += testbis;
    test.Scale(0.f, 1.f);
    if (detail)
      RAW(test);
  }

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale(0.f, 0.5f);
  green_channel.Scale(0.f, 0.75f);
  blue_channel.Scale(0.f, 0.25f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		Water(bool detail = false)
{
  test.Perlin(10, 1, 1, 0);
  test.Abs();

  if (detail)
    RAW(test);

  testbis.Perlin(5, 1, 1, 0);
  testbis.Abs();
  test += testbis;

  if (detail)
    RAW(test);

  testbis = PerlinHarmonics[2];
  testbis.Abs();
  test += testbis;

  if (detail)
    RAW(test);

  testbis.Perlin(3, 1, 1, 0);
  testbis.Abs();
  test += testbis;

  if (detail)
    RAW(test);

  testbis = PerlinHarmonics[1];
  testbis.Abs();
  test += testbis;

  if (detail)
    RAW(test);

  testbis = PerlinHarmonics[0];
  testbis.Abs();
  test += testbis;

  if (detail)
    RAW(test);

  test.Scale(1.f, -2.f);

  if (detail)
    RAW(test);

  red_channel = test;
  green_channel = test;
  blue_channel = test;

  red_channel.Scale (-0.25f, 1.f);
  green_channel.Scale (0.25f, 1.25f);
  blue_channel.Scale (0.5f, 1.f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		BrickWall(bool detail = false)
{
  //
  // FIXME : rendre le nombre de briques paramétrable
  //

  // Emplacement des briques
  tex1.Sinus(2, 4);
  tex1.Abs();

  if (detail)
    RAW(tex1);

  // Filtre pour décaller une ligne sur deux
  tex2.Cosinus(0, 4);
  tex2.Limit(0, 1.f);
  tex2.Scale(0, 1000.f);
  tex2.Limit(0, 1.f);

  if (detail)
    RAW(tex2);

  tex2.VerticalShift(tex2.Height() / 16);

  if (detail)
    RAW(tex2);

  // Lignes A
  tex3 = tex2;
  tex3.MaximizeTo(tex1);
  tex3.HorizontalShift(tex3.Width() / 6);

  if (detail)
    RAW(tex3);

  // Lignes B
  tex2.Scale(1.f, 0);
  tex1.MaximizeTo(tex2);

  if (detail)
    RAW(tex1);

  // Fusion
  tex1.MinimizeTo(tex3);

  if (detail)
    RAW(tex1);

  // Dégradé
  tex2.VerticalFade();
  tex3.VerticalFade();
  tex3.Scale(1.f, 0);
  tex2 *= tex3;
  tex2.Scale(0, 1.f);

  if (detail)
    RAW(tex2);

  // Bruit
  tex3 = PerlinHarmonics[5];
  tex3 += PerlinHarmonics[6];
  tex3 += PerlinHarmonics[7];
  tex3.Scale(-0.1f, 0.1f);
  tex1 += tex3;

  if (detail)
    RAW(tex1);

  // Joints
  tex1.Limit(0, 0.1f);
  tex1.Scale(0, 1.f);

  if (detail)
    RAW(tex1);

  // Bump map
  tex3.Scale(-0.3f, 0.3f);
  tex3 += tex1;
  tex3.Bump();
  tex3.Blur();
  tex3.Scale(0, 1.f);

  if (detail)
    RAW(tex3);

  tex1 *= tex2;
  tex1.Scale(0.2f, 1.f);
  tex1 *= tex3;

  if (detail)
    RAW(tex1);

  // Couleur
  red_channel = tex1;
  green_channel = tex1;
  blue_channel = tex1;

  red_channel.Scale(0.2f, 0.83f);
  green_channel.Scale(0.14f, 0.77f);
  blue_channel.Scale(0.1f, 0.53f);

  RAW(red_channel, green_channel, blue_channel);
}

static void		IronGrid(bool detail = false)
{
  test.Sinus(4, 1);
  RAW(test);
}

static void		IronGround(bool detail = false)
{
  test.Sinus(8, 4);

  if (detail)
    RAW(test);

  test.Scale(-7.f, 1.f);

  if (detail)
    RAW(test);

  testbis = test;
  testbis.Rotate90();

  if (detail)
    RAW(testbis);

  test.MinimizeTo(testbis);

  if (detail)
    RAW(test);

  test.Limit(0, 1);

  test.Bump();

  test.Scale(0.f, 1.f);

  if (detail)
    RAW(test);

  testbis = PerlinHarmonics[7];
  testbis.Scale(-0.2f, 0.2f);
  testbis += PerlinHarmonics[5];
  testbis += PerlinHarmonics[4];
  testbis += PerlinHarmonics[1];
  testbis += PerlinHarmonics[0];

  if (detail)
    RAW(testbis);

  testbis.Limit(0.5f, 1.f);
  testbis.Scale(1.f, 0.f);

  if (detail)
    RAW(testbis);


  Texture temp = testbis;
  temp.Bump();

  temp.Scale(1.f, -1.f);
  temp.Limit(0, 1);

  if (detail)
    RAW(temp);

  test += temp;

  if (detail)
    RAW(test);


  testbis.VerticalDribble();

  if (detail)
    RAW(testbis);

  test *= testbis;

  // Rouille :
//   red_channel.Scale(0.68f, 0);
//   green_channel.Scale(0.13f, 0);
//   blue_channel.Scale(0.01f, 0);

  RAW(test);
}

// void			Test_texture_class ()
// {
//   test.AddCuts(50, 50);
//   RAW(test);

//   test.Blur ();
//   RAW(test);

//   Texture * testbis = new Texture ();
//   testbis->AddCuts(200, 5);
//   RAW((*testbis)());

//   test += *testbis;
//   RAW(test);

//   testbis->HorizontalFade ();
//   RAW((*testbis)());

//   Texture * red = new Texture(*testbis);
//   RAW((*red)());

//   testbis->VerticalFlip ();
//   RAW((*testbis)());

//   Texture * green = new Texture(*testbis);
//   RAW((*green)());

//   (*red) += (*green);
//   delete green;
//   RAW((*red)());

//   test += (*red);
//   delete red;
//   RAW(test);

//   test.HorizontalFlip();
//   RAW(test);

//   test.Scale(0, 1);
//   RAW(test);

//   test.Random ();
//   RAW(test);

//   test.Perlin (10, 2, 0, 0);
//   RAW(test(), test(), test());

//   test.Perlin_abs (10, 2, 0, 0);
//   RAW(test(), test(), test());

//   test.Cells (10);
//   RAW(test);

//   testbis->Conic()://0, 10);
//   RAW((*testbis)());

//   test.MinimizeTo(*testbis);
//   RAW(test);

//   test.MaximizeTo(*testbis);
//   RAW(test);

//   test.Scale (0.2f, 0.8f);
//   RAW(test);

//   test.Scale (-1, 2);
//   RAW(test);

//   delete testbis;
// }

static void MixAndLoad(int foo,
		       Texture & r1,
		       Texture & g1,
		       Texture & b1,
		       const Texture & stencil,
		       const Texture & r2,
		       const Texture & g2,
		       const Texture & b2)
{
  r1.MinimizeTo(stencil);
  g1.MinimizeTo(stencil);
  b1.MinimizeTo(stencil);

  r1.MaximizeTo(r2);
  g1.MaximizeTo(g2);
  b1.MaximizeTo(b2);

  RAW(r1, g1, b1);
}

void	Blocks(bool detail)
{
  int bicolor, oneway, changekey, fastgame, french = 0;

  // Partie bloc
  // ===========

  tex1.Square();

  const float a = 0.1f;
  const float b = 0.25f;
  const float c = 0.3;

  tex2 = tex1;
  tex2.Limit(0, a);
  tex2.Scale(0, 1.f);

  tex1.Limit(b, c);

  Texture stencil = tex1;

  tex1.Scale(1.f, 0.5f);

  tex1.MaximizeTo(tex2);

  tex2 = PerlinHarmonics[5];
  tex2 += PerlinHarmonics[6];
  tex2 += PerlinHarmonics[7];

  tex2.Scale(-0.1f, 0.1f);

  tex1 += tex2;

  tex1.Bump();
  tex1.Blur();
  tex1.Scale(0, 1.f);


  // Pochoir
  // =======

  stencil.Scale(-10.f, 1.f);
  stencil.Limit(0, 1.f);


  // Couleur de la pierre
  // ====================

  Texture red_stone = tex1;
  Texture green_stone = tex1;
  Texture blue_stone = tex1;

  red_stone.Scale(0.2f, 0.83f);
  green_stone.Scale(0.14f, 0.77f);
  blue_stone.Scale(0.1f, 0.53f);

  red_stone.MinimizeTo(stencil);
  green_stone.MinimizeTo(stencil);
  blue_stone.MinimizeTo(stencil);


  // Bicolor
  // =======

  tex1.HorizontalFade();
  tex1.Limit(0.5f * c + (1.f - c) / 3.f, 0.5f * c + 2 * (1.f - c) / 3.f);
  tex1.Scale(0, 1.f);

  red_channel = tex1;
  red_channel.Scale(0.84f, 0);

  green_channel = tex1;
  green_channel.Scale(0, 0.27f);

  blue_channel = tex1;
  blue_channel.Scale(0.14f, 1.f);

  tex2.Square();
  tex2.Limit(c, c + (1.f - c) * 0.25f);
  tex2.Scale(0, 1.f);
  tex2.GaussianBlur();

  red_channel *= tex2;
  green_channel *= tex2;
  blue_channel *= tex2;

  stencil.Scale(1.f, 0);

  MixAndLoad(bicolor,
	     red_channel, green_channel, blue_channel,
	     stencil,
	     red_stone, green_stone, blue_stone);

  // Drapeau fr
  // ==========

  tex3 = tex1;  // tex1 contient encore un Hfade

  tex1.Limit(0, 0.05f);
  tex1.Scale(1.f, 0);
  red_channel = tex1;

  tex2 = tex1;

  tex3.Limit(0.95f, 1.f);
  tex3.Scale(0, 1.f);

  tex2 += tex3;
  tex2.Limit(0.99f, 1.f);
  tex2.Scale(1.f, 0);

  red_channel = tex3;
  red_channel.Scale(0, 0.957f);
  red_channel.MinimizeTo(tex2);

  green_channel = tex2;

  blue_channel = tex1;
  blue_channel.Scale(0, 0.56f);
  blue_channel.MinimizeTo(tex2);


  MixAndLoad(french,
	     red_channel, green_channel, blue_channel,
	     stencil,
	     red_stone, green_stone, blue_stone);


  // Sens interdit
  // =============

  tex2.Square();

  tex2.Limit(c + 0.26f * (1.f - c), c + 0.26f * (1.f - c) + 0.01f);
  tex2.Scale(0, 1.f);

  tex3.Cosinus(0, 2);

  tex3.Limit(0.45f, 0.46f);
  tex3.Scale(0, 1.f);

  tex2 *= tex3;

  tex3.Conic();
  tex3.Limit(c + 0.05f, c + 0.06f);
  tex3.Scale(0, 1.f);

  red_channel = tex3;
  red_channel.Scale(0, 0.9f);
  red_channel.MinimizeTo(tex2);

  green_channel.Clear();
  green_channel.MinimizeTo(tex2);

  blue_channel = green_channel;


  MixAndLoad(oneway,
	     red_channel, green_channel, blue_channel,
	     stencil,
	     red_stone, green_stone, blue_stone);


  // Mode lent
  // =========

  tex1.Conic();

  tex1.Limit(0.7f, 1.f);
  tex1.Scale(0, 10.f);
  tex1.Limit(0, 1.f);

  const unsigned char shift = (unsigned char)(0.12f * tex1.Width() / (1.f + c));

  tex1.HorizontalShift(-shift);

  tex2 = tex1;
  tex2.HorizontalShift(shift);
  tex2.Scale(0, 0.70f);
  tex1.MinimizeTo(tex2);

  tex2.HorizontalShift(shift);
  tex2.Scale(0, 0.4f);
  tex1.MinimizeTo(tex2);

  tex2.HorizontalShift(shift);
  tex2.Scale(0, 0.1f);
  tex1.MinimizeTo(tex2);

  red_channel = tex1;
  blue_channel = tex1;
  green_channel = tex1;

  green_channel.Scale(0.f, 1.149f);
  blue_channel.Scale(0.f, 1.226f);

  MixAndLoad(oneway,
	     red_channel, green_channel, blue_channel,
	     stencil,
	     red_stone, green_stone, blue_stone);


  // Mode rapide
  // ===========
  tex1.Conic();

  tex1.Limit(0.7f, 1.f);
  tex1.Scale(0, 10.f);
  tex1.Limit(0, 1.f);

  tex2 = tex1;

  const unsigned char trail = (unsigned char)(0.15f * tex1.Width() / (1.f + c));
  tex1.HorizontalMotionBlur(trail);

  tex2.HorizontalShift(-trail);
  tex1.MinimizeTo(tex2);

  red_channel = tex1;
  blue_channel = tex1;
  green_channel = tex1;

  green_channel.Scale(0.f, 1.149f);
  blue_channel.Scale(0.f, 1.226f);

  MixAndLoad(oneway,
	     red_channel, green_channel, blue_channel,
	     stencil,
	     red_stone, green_stone, blue_stone);
}

void			TestTexture ()
{
  std::cerr << "[" << std::endl;

  Cells100.Cells (100);
//   RAW(Cells100);

  unsigned int harmonic = 1;
  for (unsigned int i = 0; i < 8; ++i)
  {
    PerlinHarmonics[i].Perlin(harmonic, 1, 1, 0);
//     PerlinHarmonics[i].Random();
    std::cerr << "." << std::endl;
    harmonic <<= 1;
  }
  std::cerr << "-" << std::endl;

//   Test_texture_class ();
//   std::cerr << ";" << std::endl;
//   Perlin();
//   std::cerr << ";" << std::endl;
//   Perlin_sum (false);
//   std::cerr << ";" << std::endl;

  TestGradient(true);
  std::cerr << ";" << std::endl;

//   Fireball(true);
//   std::cerr << ";" << std::endl;

//   PurplePlasmaball(true);
//   std::cerr << ";" << std::endl;

//   GreenPlasmaball(true);
//   std::cerr << ";" << std::endl;

//   WhitePlasmaball(true);
//   std::cerr << ";" << std::endl;

//   Lava(true);
//   std::cerr << ";" << std::endl;

//   Cave(true);
//   std::cerr << ";" << std::endl;

//   Wood(true);
//   std::cerr << ";" << std::endl;

//   Grass(true);
//   std::cerr << ";" << std::endl;

//   Water(true);
//   std::cerr << ";" << std::endl;

//   Cloud1();
//   std::cerr << ";" << std::endl;
//   Cloud2();
//   std::cerr << ";" << std::endl;
//   Cloud3();
//   std::cerr << ";" << std::endl;
//   Cloud4();
//   std::cerr << ";" << std::endl;
//   Cloud5 ();

//   BrickWall();
//   IronGrid();
//   IronGround(true);

//   Blocks(true);

  std::cerr << "]" << std::endl;
}

// ============================================================================
