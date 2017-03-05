
#include "bookshelf.hh"

#include "sys/msys.h"

#include "mesh.hh"
#include "node.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "spline.h"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace BookShelf
{
  const int dominoFallLength = 300; // ms
  const int lastDominoFallLength = 400; // ms

  void bookDominoFall(const Node & node, date d)
  {
    float progress = (float) d / dominoFallLength;
    glLoadIdentity();
    glRotatef(progress * _TV(30.f), 0.f, 0.f, 1.f);
  }

  void lastBookDominoFall(const Node & node, date d)
  {
    float progress = (float) d / lastDominoFallLength;
    glLoadIdentity();
    glRotatef(progress * _TV(81.f), 0.f, 0.f, 1.f);
  }

  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    Renderable bookShelf(Shader::parallax, VBO::bookShelf);
    bookShelf.setTextures(Texture::wood_Noyer, Texture::woodBump1);
    bookShelf.setShininess(_TV(50));
    root->attachRenderable(bookShelf);

    // Livres avec les greetings
    Renderable greetingsBooks(Shader::parallax, VBO::greetingsBooks);
    greetingsBooks.setTextures(Texture::greetingsBooks, Texture::greetingsBooksBump);
    greetingsBooks.setShininess(_TV(20));
    root->attachRenderable(greetingsBooks);

    // Dominos de livres
    glLoadIdentity();
    glTranslatef(_TV(42.f), _TV(70.f), _TV(30.f));
    msys_srand(0);
    Renderable book(Shader::parallax, VBO::book);
	int numberOfBooks = _TV(15);
    for (int i = 0; i < numberOfBooks; i++)
    {
      glTranslatef(_TV(-4.2f), _TV(0.f), _TV(0.f));
      glPushMatrix();
      glTranslatef(_TV(0.f), _TV(0.f), msys_sfrand() * _TV(2.f));
      Node * domino = Node::New(worldStartDate, worldEndDate);
      glPopMatrix();

      domino->attachRenderable(book.setTextures(Texture::book, Texture::bookBump));
      domino->setAnimation(i == numberOfBooks - 1 ? Anim::lastBookDominoFall : Anim::bookDominoFall);
      int startDate = _TV(90000) + _TV(20) * i;
      domino->setAnimationStartDate(startDate);
      int length = dominoFallLength;
      if (i == numberOfBooks - 1) length = lastDominoFallLength;
      if (i == numberOfBooks - 2) length = _TV(450);
      domino->setAnimationStopDate(startDate + length);
      domino->attachToNode(root);
    }

    return root;
  }

  // Mêmes données que le couvercle de la boite à musique
  static float drawerData[] = {
    0, 0, 100,
    0, 40, 90,
    0, 70, 60,
    0, 80, 21,
    0, 81, 20,
    0, 95, 10,
    0, 100, 4,
    0, 100, 1,
    0, 100, 0,
  };

  static float handleData[] = {
    0, 0, 45,
    0, 25, 45,
    0, 25, 45,
    0, 35, 35,
    0, 50, 35,
    0, 60, 65,
    0, 67, 100,
    0, 85, 90,
    0, 90, 80,
    0, 97, 60,
    0, 100, 0,
  };

  static float montantData[] = {
    0, 0, 20,
    0, 1, 40,
    0, 5, 40,
    0, 8, 5,
    0, 10, 40,

    0, 65, 40,

    0, 89, 30,
    0, 90, 10,
    0, 91, 40,
    0, 94, 10,
    0, 95, 30,
    0, 98, 40,
    0, 99, 20,
    0, 99, 40,
    0, 100, 40,
    0, 100, 0,
  };

  void createBook(Array<vertex> & book,
		  float epaisseur, float hauteur, float largeur, float arrondi)
  {
    const float hMin = _TV(1.f);

    assert(epaisseur >= 2.f * arrondi &&
	   largeur >= 2.f * arrondi &&
	   hauteur >= hMin);

    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    const int rFaces = _TV(8);
    // Comme on n'a pas un cercle parfait, il manque quelques mm pour atteindre "largeur"
    wheel.generateMesh(book, hMin, arrondi + _TV(0.2f), _TV(5), rFaces);
    Mesh::rotate(book, 180.f / float(rFaces), 0, 1.f, 0);
    Mesh::expandPave(book,
		     0.5f * epaisseur - arrondi,
		     0.5f * (hauteur - hMin),
		     0.5f * largeur - arrondi);
    Mesh::translate(book, _TV(0.5f) * epaisseur, _TV(0.f) * hauteur, _TV(0.f));
  }

  enum color {
    black = 0,
    green,
    red,
    blue,
    brown,
  };

  static const bookDesc books[] = {
    // Cas à part
//     { black,   impact, 21, "Conspiracy      Chaos Theory" },

    // Livres dans la bibliothèque
    { black,  georgia, 26, "UF  -  DD     Wir Sind Einstein" },

    { green,    roman, 21, "ASD                         Chameleon" },
    { green,    roman, 21, "ASD                         Lifeforce" },

    { brown,  georgia, 21, "Fairlight  -  CNCD           Numb Res" },
    { brown,  georgia, 21, "Fairlight                        Panic Room" },

    { green,  georgia, 24, "RGBA                    Elevated" },
    { green,  georgia, 24, "RGBA                    Paradize" },

    {   red,  verdana, 14, "Razor1911    We Have Accidently Borrowed Your Votedisk" },
    {  blue,  georgia, 21, "Traction                       Onward" },

    {   red,  verdana, 21, "Frequency               Ergon" },
    {   red,  georgia, 21, "Approximate            Ephemera" },
    {   red,  georgia, 21, "Mercury                Epsilon" },
    { black,    roman, 21, "SQNY                 Chromosphere" },

    { brown, palatino, 24, "Farbrausch         .the .product" },
    { brown, palatino, 24, "Farbrausch             Debris" },
    { brown, palatino, 24, "Farbrausch           Magellan" },

    { black,  georgia, 21, "Exceed               Heaven Seven" },
    {  blue,  verdana, 21, "Andromeda  -  Orb        Stargazer" },
    {   red,  verdana, 21, "Plastic                  Into the Pink" },
    { black,  georgia, 24, "Kewlers  -  MFX               1995" },

    // Still et Pixtur
    { black,  verdana, 21, "Haujobb              You Should" },
    { green,    serif, 24, "Still                  Ferner" },
    { green,    serif, 24, "Still             Finally Inside" },
    { green,    serif, 24, "Still                The Seeker" },

    // Amiga
    { black,  georgia, 21, "Elude                        Dust" },
    { brown,  georgia, 21, "Nature & Traktor      Jesus Christ Motocross" },
  };

  const bookDesc & greetingsBook(int i)
  {
    return books[i];
  }

  int numberOfGreetingsBooks()
  {
    return ARRAY_LEN(books);
  }

  void createBookSeries(Array<vertex> & bookSeries)
  {
    msys_srand(-3008769);
    Array<vertex> & book = Mesh::getTemp();

    float epaisseur;
    float hauteur;
    float largeur;
    float arrondi;

    glLoadIdentity();
    const int numberOfBooks = ARRAY_LEN(books);
    for (int i = 0; i < numberOfBooks; ++i)
    {
      const char * title = books[i].title;
      if (0 == i ||
	  // strcmp ultra optimisé :)
	  title[0] != books[i - 1].title[0] ||
	  title[1] != books[i - 1].title[1])
      {
	epaisseur = _TV(4.f);// + _TV(0.f) * msys_frand();
	hauteur = _TV(20.f) + _TV(5.f) * msys_frand();
	largeur = hauteur * (_TV(0.7f) + _TV(0.1f) * msys_sfrand());
	arrondi = _TV(1.25f) + _TV(0.5f) * msys_sfrand();
      }

      book.empty();
      createBook(book, epaisseur, hauteur, largeur, arrondi);

      glPushMatrix();
      glTranslatef(0, 0.5f * hauteur, 0.5f * largeur);
      Mesh::applyCurrentProjection(book);

      const float scale = 0.5f / numberOfBooks;
      const vector3f u(-scale, 0, 0);
      const vector3f v(0, -scale, 0);
      Mesh::reprojectTexture(book, u, v);
      Mesh::translateTexture(book,
			     _TV(0.f) + ((2*i) / numberOfBooks) * _TV(0.f),
			     _TV(0.f) + ((2*i) / numberOfBooks) * _TV(0.5f));

      glLoadIdentity();
      Mesh::addVerticesUsingCurrentProjection(bookSeries, book);

      glPopMatrix();
      glTranslatef(epaisseur, 0, 0);
    }
  }

  void generateMeshes()
  {
    float LONGUEUR_BIBLIOTHEQUE = _TV(140.f);
    float LARGEUR_BIBLIOTHEQUE  = _TV(50.f);
    float HAUTEUR_BIBLIOTHEQUE  = _TV(190.f);

    float BORD_PIED             = _TV(1.5f);
    float HAUTEUR_PIED          = _TV(10.f);

    float BORD_PLAFOND          = _TV(1.5f);
    float HAUTEUR_PLAFOND       = _TV(3.f);

    float HAUTEUR_COFFRE        = _TV(60.f);

    float LARGEUR_MONTANT       = _TV(6.f);
    float EPAISSEUR_MONTANT     = _TV(4.5f);
    float HAUTEUR_MONTANT       = HAUTEUR_BIBLIOTHEQUE - HAUTEUR_PIED - HAUTEUR_COFFRE - HAUTEUR_PLAFOND;

    float LARGEUR_PLAQUE        = LARGEUR_BIBLIOTHEQUE - EPAISSEUR_MONTANT;
    float EPAISSEUR_PLAQUE      = _TV(1.5f);
    float HAUTEUR_PLAQUE        = HAUTEUR_MONTANT;

    float LARGEUR_FOND          = LONGUEUR_BIBLIOTHEQUE - 2.f * EPAISSEUR_PLAQUE;
    float EPAISSEUR_FOND        = _TV(1.5f);
    float HAUTEUR_FOND          = HAUTEUR_MONTANT;

    float LARGEUR_TIROIR        = _TV(20.f);
    float HAUTEUR_TIROIR        = _TV(18.f);
    float EPAISSEUR_TIROIR      = _TV(1.5f);
    float REBORD_TIROIR         = _TV(1.5f);

    float LARGEUR_PORTE         = _TV(40.f);
    float HAUTEUR_PORTE         = _TV(58.f);
    float EPAISSEUR_PORTE       = _TV(1.5f);
    float REBORD_PORTE          = _TV(1.5f);

    float LONGUEUR_PLATEAU      = LONGUEUR_BIBLIOTHEQUE - _TV(3.f);
    float LARGEUR_PLATEAU       = LARGEUR_BIBLIOTHEQUE - _TV(5.f);
    float EPAISSEUR_PLATEAU     = _TV(1.5f);

    float DIAMETRE_POIGNEE      = _TV(3.f);
    float HAUTEUR_POIGNEE       = _TV(2.f);

    const float woodTextureScale = _TV(0.012f);

    Mesh::clearPool();
    Array<vertex> & pied= Mesh::getTemp();
    Mesh::generatePave(pied, HAUTEUR_PIED, LONGUEUR_BIBLIOTHEQUE + 2.f * BORD_PIED, LARGEUR_BIBLIOTHEQUE + BORD_PIED);

    Array<vertex> & plafond = Mesh::getTemp();
    Mesh::generatePave(plafond, LARGEUR_BIBLIOTHEQUE + BORD_PLAFOND, HAUTEUR_PLAFOND, LONGUEUR_BIBLIOTHEQUE + 2.f * BORD_PLAFOND);

    Array<vertex> & coffre = Mesh::getTemp();
    Mesh::generatePave(coffre, LARGEUR_BIBLIOTHEQUE, HAUTEUR_COFFRE, LONGUEUR_BIBLIOTHEQUE);

    Array<vertex> & montant = Mesh::getTemp();
    Mesh::Revolution revMontant(NULL, NULL, montantData, ARRAY_LEN(montantData));
    revMontant.generateMesh(montant, HAUTEUR_MONTANT, _TV(3.f), _TV(18), _TV(6));
    Mesh::expandPave(montant, _TV(2.5f), _TV(0.f), _TV(1.75f));
    Mesh::reprojectTextureXYPlane(montant, woodTextureScale);

    Array<vertex> & plaque = Mesh::getTemp();
    Mesh::generatePave(plaque, EPAISSEUR_PLAQUE, HAUTEUR_PLAQUE, LARGEUR_PLAQUE);

    Array<vertex> & fond = Mesh::getTemp();
    Mesh::generatePave(fond, LARGEUR_FOND, HAUTEUR_FOND, EPAISSEUR_FOND);

    Mesh::Revolution drawer(NULL, NULL, drawerData, ARRAY_LEN(drawerData));
    Array<vertex> & tiroir = Mesh::getTemp();
    drawer.generateMesh(tiroir, EPAISSEUR_TIROIR, REBORD_TIROIR, _TV(8), _TV(-4));
    glLoadIdentity();
    glRotatef(45.f, 0, 1.f, 0);
    Mesh::applyCurrentProjection(tiroir);
    Mesh::expandPave(tiroir, 0.5f * LARGEUR_TIROIR - REBORD_TIROIR, 0, 0.5f * HAUTEUR_TIROIR - REBORD_TIROIR);
    Mesh::reprojectTextureXZPlane(tiroir, woodTextureScale);
    glLoadIdentity();
    glRotatef(90.f, 1.f, 0, 0);
    Mesh::applyCurrentProjection(tiroir);

    Array<vertex> & porte = Mesh::getTemp();
    drawer.generateMesh(porte, EPAISSEUR_PORTE, REBORD_PORTE, _TV(8), _TV(-4));
    glLoadIdentity();
    glRotatef(45.f, 0, 1.f, 0);
    Mesh::applyCurrentProjection(porte);
    Mesh::expandPave(porte, 0.5f * LARGEUR_PORTE - REBORD_PORTE, 0, 0.5f * HAUTEUR_PORTE - REBORD_PORTE);
    Mesh::reprojectTextureXZPlane(porte, woodTextureScale);
    glLoadIdentity();
    glRotatef(90.f, 1.f, 0, 0);
    Mesh::applyCurrentProjection(porte);

    Array<vertex> & plateau = Mesh::getTemp();
    Mesh::generatePave(plateau, LARGEUR_PLATEAU, EPAISSEUR_PLATEAU, LONGUEUR_PLATEAU);

    Mesh::Revolution handle(NULL, NULL, handleData, ARRAY_LEN(handleData));
    Array<vertex> & handleMesh = Mesh::getTemp();
    handle.generateMesh(handleMesh, HAUTEUR_POIGNEE, 0.5f * DIAMETRE_POIGNEE, 8, 9);


    Array<vertex> & bookShelf = Mesh::getTemp();

    glLoadIdentity();
    glTranslatef(_TV(0.f), 0.5f * HAUTEUR_PIED, 0.5f * (LARGEUR_BIBLIOTHEQUE + BORD_PIED));
    glRotatef(90.f, _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(bookShelf, pied);

    glLoadIdentity();
    glTranslatef(_TV(0.f), HAUTEUR_BIBLIOTHEQUE - 0.5f * HAUTEUR_PLAFOND, 0.5f * (LARGEUR_BIBLIOTHEQUE + BORD_PLAFOND));
    glRotatef(90.f, 0, 1.f, 0); // Pour avoir la texture dans le sens de la longueur
    Mesh::addVerticesUsingCurrentProjection(bookShelf, plafond);

    glLoadIdentity();
    glTranslatef(_TV(0.f), HAUTEUR_PIED + 0.5f * HAUTEUR_COFFRE, 0.5f * LARGEUR_BIBLIOTHEQUE);
    glRotatef(90.f, 0, 1.f, 0); // Pour avoir la texture dans le sens de la longueur
    Mesh::addVerticesUsingCurrentProjection(bookShelf, coffre);

    glLoadIdentity();
    glTranslatef(_TV(0.f), HAUTEUR_PIED + HAUTEUR_COFFRE + 0.5f * HAUTEUR_FOND, 0.5f * EPAISSEUR_FOND);
    Mesh::addVerticesUsingCurrentProjection(bookShelf, fond);

    for (unsigned int i = 0; i < 6; ++i)
    {
      glLoadIdentity();
      const float x = (i % 2 ? -1.f : 1.f) * (LARGEUR_PORTE + 0.5f * LARGEUR_TIROIR + 4.f);
      const float y = HAUTEUR_PIED + (1 + 2 * (i / 2)) * (1.f + 0.5f * HAUTEUR_TIROIR);
      glTranslatef(x, y, 0.5f * EPAISSEUR_TIROIR + LARGEUR_BIBLIOTHEQUE);
      Mesh::translateTexture(tiroir, msys_frand(), msys_frand());
      Mesh::addVerticesUsingCurrentProjection(bookShelf, tiroir);

      glTranslatef(0, 0, 0.5f * EPAISSEUR_TIROIR);
      glRotatef(90.f, 1.f, 0, 0);
      Mesh::addVerticesUsingCurrentProjection(bookShelf, handleMesh);
    }

    {
      glLoadIdentity();
      glTranslatef(-(1.f + 0.5f * LARGEUR_PORTE), HAUTEUR_PIED + 0.5f * HAUTEUR_COFFRE, (0.5f * EPAISSEUR_PORTE + LARGEUR_BIBLIOTHEQUE));
      Mesh::translateTexture(porte, msys_frand(), msys_frand());
      Mesh::addVerticesUsingCurrentProjection(bookShelf, porte);
      glTranslatef(0.5f * LARGEUR_PORTE - 5.f, 0, 0.5f * EPAISSEUR_PORTE);
      glRotatef(90.f, 1.f, 0, 0);
      Mesh::addVerticesUsingCurrentProjection(bookShelf, handleMesh);


      glLoadIdentity();
      glTranslatef((1.f + 0.5f * LARGEUR_PORTE), HAUTEUR_PIED + 0.5f * HAUTEUR_COFFRE, (0.5f * EPAISSEUR_PORTE + LARGEUR_BIBLIOTHEQUE));
      Mesh::translateTexture(porte, msys_frand(), msys_frand());
      Mesh::addVerticesUsingCurrentProjection(bookShelf, porte);
      glTranslatef(-0.5f * LARGEUR_PORTE + 5.f, 0, 0.5f * EPAISSEUR_PORTE);
      glRotatef(90.f, 1.f, 0, 0);
      Mesh::addVerticesUsingCurrentProjection(bookShelf, handleMesh);
    }

    {
      glLoadIdentity();
      glTranslatef(-(0.5f * LONGUEUR_BIBLIOTHEQUE - 0.5f * LARGEUR_MONTANT), HAUTEUR_COFFRE + HAUTEUR_PIED, (LARGEUR_BIBLIOTHEQUE - 0.5f * EPAISSEUR_MONTANT));
      Mesh::addVerticesUsingCurrentProjection(bookShelf, montant);

      glLoadIdentity();
      glTranslatef((0.5f * LONGUEUR_BIBLIOTHEQUE - 0.5f * LARGEUR_MONTANT), HAUTEUR_COFFRE + HAUTEUR_PIED, (LARGEUR_BIBLIOTHEQUE - 0.5f * EPAISSEUR_MONTANT));
      Mesh::translateTexture(montant, msys_frand(), msys_frand());
      Mesh::addVerticesUsingCurrentProjection(bookShelf, montant);
    }

    {
      glLoadIdentity();
      glTranslatef(-(0.5f * LONGUEUR_BIBLIOTHEQUE - 0.5f * EPAISSEUR_PLAQUE), (HAUTEUR_PIED + HAUTEUR_COFFRE + 0.5f * HAUTEUR_PLAQUE), (LARGEUR_BIBLIOTHEQUE - 0.5f * LARGEUR_PLAQUE - EPAISSEUR_MONTANT));
      Mesh::addVerticesUsingCurrentProjection(bookShelf, plaque);

      glLoadIdentity();
      glTranslatef((0.5f * LONGUEUR_BIBLIOTHEQUE - 0.5f * EPAISSEUR_PLAQUE), (HAUTEUR_PIED + HAUTEUR_COFFRE + 0.5f * HAUTEUR_PLAQUE), (LARGEUR_BIBLIOTHEQUE - 0.5f * LARGEUR_PLAQUE) - EPAISSEUR_MONTANT);
      Mesh::addVerticesUsingCurrentProjection(bookShelf, plaque);
    }

    for (unsigned int i = 0; i < 2; ++i)
    {
      glLoadIdentity();
      glTranslatef(_TV(0.f), HAUTEUR_PIED + HAUTEUR_COFFRE + (i + 1) * _TV(40.f), (1.5f + 0.5f * LARGEUR_PLATEAU));
      glRotatef(90.f, 0, 1.f, 0); // Pour avoir la texture dans le sens de la longueur
      Mesh::addVerticesUsingCurrentProjection(bookShelf, plateau);
    }

    Array<vertex> & bookSeries = Mesh::getTemp();
    createBookSeries(bookSeries);
    Mesh::translate(bookSeries, _TV(-35.f), _TV(111.f), _TV(10.f));
    VBO::setupData(VBO::greetingsBooks, bookSeries DBGARG("greetingsBooks"));


    VBO::setupData(VBO::bookShelf, bookShelf DBGARG("bookShelf"));


    Array<vertex> & book = Mesh::getTemp();
    createBook(book, _TV(3.75f), _TV(20.f), _TV(16.f), _TV(1.25f));
    Mesh::translate(book, 0, _TV(10.f), 0);
    VBO::setupData(VBO::book, book DBGARG("book"));
  }
}
