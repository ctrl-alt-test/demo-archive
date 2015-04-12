#include		"texgen\texture.hh"

void fakeTexturesFunctions(void)
{
  Texture::Channel a;
  Texture::Channel b;

  a.Clear();
  a.Gradient(1.1f, 0.3f, 0.2f, 0.6f);
  a.AddCuts(100, 10);
  a.Cells(42);
  a.Conic();//unsigned char, char);
  a.Square();
  a.HorizontalFade();
  a.VerticalFade();
  a.Perlin(10, 1, 1, 1.2f);
  a.Sinus(10, 5);
  a.Cosinus(10, 7);
  a.Random();
  a.Cut(4);
  a.Abs();
  a.Clamp(0.2f, 0.5f);
  a.Scale(0.5f, 0.8f);
  a.NegativeNormalized();
  a.Segment(0.3f);
  a.Bump();
  a.Blur();
  a.GaussianBlur();
  a.HorizontalMotionBlur(2);
  a.VerticalMotionBlur(3);
  a.VerticalDribble();
  a.HorizontalShift(5);
  a.VerticalShift(8);
  a.HorizontalFlip();
  a.VerticalFlip();
  a.Transpose();
  a.Rotate90();
  a.MinimizeTo(b);
  a.MaximizeTo(b);
  a += b;
  b -= a;
  a *= b;
  a *= 0.7f;
  a /= 2.2f;
}

void fakeTextures(void)
{
}
/*
  Cloud1();
  Cloud2();
  Cloud3();
  Cloud4();
  Lava();
  Cave();
  Wood();
  Fireball();
  PurplePlasmaball();
  GreenPlasmaball();
  WhitePlasmaball();
  Grass();
  Water();
  BrickWall();
  IronGrid();
  IronGround();
  Blocks(false);
}
*/
