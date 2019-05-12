//
// TODO : nettoyer et factoriser le code
//

#include "interpolation.hh"
#include "node.hh"
#include "revolution.hh"
#include "shaders.hh"
#include "spline.h"
#include "textureid.hh"
#include "textures.hh"
#include "tweakval.h"
#include "timing.hh"
#include "vbos.hh"

#define START_DATE	worldStartDate
#define END_DATE	worldEndDate

namespace Chess
{
  // Pion
  static float pawnData[] = {
    0, 0, 0,
    0, 2, 55,
    0, 13, 53,
    0, 19, 55,
    0, 49, 40,
    0, 54, 40,
    0, 58, 35,
    0, 67, 35,
    0, 93, 22,
    0, 139, 13,
    0, 147, 27,
    0, 152, 30,
    0, 156, 43,
    0, 165, 15,
    0, 183, 35,
    0, 210, 41,
    0, 235, 25,
    0, 244, 0,
  };

  static float bishopData[] = {
    0, 0, 0,
    0, 2, 55,
    0, 13, 53,
    0, 19, 55,
    0, 49, 40,
    0, 54, 40,
    0, 58, 35,
    0, 67, 35,
    0, 93, 22,
    0, 150, 17,
    0, 159, 45,
    0, 166, 30,
    0, 170, 26,
    0, 192, 27,
    0, 234, 47,
    0, 276, 24,
    0, 223, 13,
    0, 283, 13,
    0, 287, 6,
    0, 296, 11,
    0, 300, 0,
  };

  static float rockData[] = {
    0, 0, 0,
    0, 2, 55,
    0, 13, 53,
    0, 19, 55,
    0, 49, 40,
    0, 54, 40,
    0, 58, 35,
    0, 67, 35,
    0, 93, 28,
    0, 217, 27,
    0, 218, 36,
    0, 219, 40,
    0, 224, 40,
    0, 256, 45,
    0, 260, 0,
  };

  static float queenData[] = { // sert aussi pour le roi
    0, 0, 0,
    0, 2, 55,
    0, 13, 53,
    0, 19, 55,
    0, 49, 40,
    0, 54, 40,
    0, 58, 35,
    0, 67, 35,
    0, 93, 22,
    0, 160, 17,
    0, 169, 45,
    0, 185, 40,
    0, 190, 26,
    0, 200, 40,
    0, 206, 27,
    0, 275, 47,
    0, 282, 6,
    0, 300, 10,
    0, 305, 0,
  };

  static float knightData[] = {
    0, 60,
    45, 84,
    65, 123,
    66, 169,
    74, 166,
    98, 176,
    115, 179,
    128, 132,
    138, 59,
    152, 63,
    158, 94,
    163, 97,
    165, 81,
    168, 84,
    173, 102,
    191, 96,
    217, 54,
    260, 39,
    273, 53,
    309, 64,
    320, 56,
    360, 60
  };

  static float knightSocleData[] = {
    0, 0, 0,
    0, 2, 55,
    0, 13, 53,
    0, 19, 55,
    0, 49, 40,
    0, 54, 35,
    0, 58, 0,
  };

  vector3f bishopHFunc(float t, float theta) {
    // déformation du haut du fou - à refaire ?
    bishopData[17*3 + 1] = (223.f - 22.f) + (223.f - 22.f) * 0.5f * cosf(theta);
    bishopData[16*3 + 1] = (234.f - 34.f) + (234.f - 34.f) * 0.5f * -cosf(theta);

    float ret[2];
    spline(bishopData, ARRAY_LEN(bishopData) / 3, 2, t, ret);
    return vector3f(0.f, ret[0] / 255.f + 0.001f, 0.f);
  }

  vector3f rockHFunc(float t, float theta) {
    int top = (int) (1.f + msys_sinf(theta * _TV(6.f))); // crête
    rockData[13*3 + 1] = 256.f + top * _TV(20.f);

    float ret[2];
    spline(rockData, ARRAY_LEN(rockData) / 3, 2, t, ret);
    return vector3f(0.f, ret[0] / 255.f + 0.001f, 0.f);
  }

  vector3f knightHFunc(float t, float theta) {
    return vector3f(0.f, t, 0.f);
  }
  float knightRFunc(float t, float theta) {
    float ret[2];
    spline(knightData, ARRAY_LEN(knightData) / 2, 1, theta / 3.1415f * 180.f, ret);
    float r = ret[0];
    r = mix(r, 0.f, t == 0.5f ? 0.f : msys_powf(fabsf(1.f - t * 2.f), 6.f));
    return r * 2.f / 255.f + 0.01f;
  }

  void _generateTextureCoordinatesForPawn(Array<vertex> & mesh)
  {
    // On choisit un plan aléatoire mais proche de l'horizontale
    vector3f u(msys_sfrand(), 0.25f * msys_sfrand(), msys_sfrand());
    vector3f v(0.25f * msys_sfrand(), 1.f, 0.25f * msys_sfrand());
    normalize(u);
    vector3f w = u ^ v;
    v = w ^ u;
    normalize(v);

    const float textureScale = _TV(0.2f);
    u *= textureScale;
    v *= textureScale;

    // Et on remappe la texture
    Mesh::reprojectTexture(mesh, u, v);
  }

  void generateMeshes()
  {
    VBO::generatePave(VBO::chessBoard, 24.f, _TV(2.f), 24.f);
    Mesh::clearPool();
    Array<vertex> & border = Mesh::getTemp();
    Mesh::generateCubicTorus(border, _TV(27.f), _TV(24.f), _TV(2.f));
    VBO::setupData(VBO::chessBorder, border DBGARG("chessBorder"));

    Mesh::Revolution pawn(NULL, NULL, pawnData, ARRAY_LEN(pawnData));
    //Mesh::Revolution pawn = Mesh::loadSplineFromFile("data/revolution.txt");
    Array<vertex> & pawnMesh = Mesh::getTemp();
    pawn.generateMesh(pawnMesh, _TV(1.2f), _TV(1.4f), _TV(20), _TV(8));

    Mesh::Revolution knight(knightHFunc, knightRFunc);
    Array<vertex> & knightMesh = Mesh::getTemp();
    knight.generateMesh(knightMesh, _TV(1.f), _TV(1.6f), _TV(4), _TV(39));
    glLoadIdentity();
    glTranslatef(_TV(0.25f), _TV(3.1f), _TV(-0.5f));
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    glTranslatef(_TV(0.f), _TV(0.f), _TV(0.f));
    Mesh::applyCurrentProjection(knightMesh);
    Mesh::Revolution socle(NULL, NULL, knightSocleData, ARRAY_LEN(knightSocleData));
    socle.generateMesh(knightMesh, _TV(1.7f), _TV(1.25f), _TV(10), _TV(14));
    Mesh::rotate(knightMesh, 90.f, 0, 1.f, 0);

    Mesh::Revolution bishop(bishopHFunc, NULL, bishopData, ARRAY_LEN(bishopData));
    Array<vertex> & bishopMesh = Mesh::getTemp();
    bishop.generateMesh(bishopMesh, _TV(3.5f), _TV(1.6f), _TV(20), _TV(12));

    Mesh::Revolution rock(rockHFunc, NULL, rockData, ARRAY_LEN(rockData));
    Array<vertex> & rockMesh = Mesh::getTemp();
    rock.generateMesh(rockMesh, _TV(3.f), _TV(1.6f), _TV(20), _TV(15));

    //Mesh::Revolution queen = Mesh::loadSplineFromFile("data/revolution.txt");
    Mesh::Revolution queen(NULL, NULL, queenData, ARRAY_LEN(queenData));
    Array<vertex> & queenMesh = Mesh::getTemp();
    Array<vertex> & kingMesh = Mesh::getTemp();
    queen.generateMesh(queenMesh, _TV(1.5f), _TV(1.6f), _TV(22), _TV(8));

    // croix du roi
    Mesh::generatePave(kingMesh, _TV(0.75f), _TV(0.25f), _TV(0.25f));
    Mesh::generatePave(kingMesh, _TV(0.25f), _TV(0.75f), _TV(0.25f));
    Mesh::translate(kingMesh, _TV(0.f), _TV(4.5f), _TV(0.f));
    queen.generateMesh(kingMesh, _TV(1.5f), _TV(1.6f), _TV(28), _TV(8));

    Array<vertex> chessPiecesW(32000);
    Array<vertex> chessPiecesB(32000);

    // temporaire ?
    Array<vertex>* pieces[8] = {
      &rockMesh,
      &knightMesh,
      &bishopMesh,
      &kingMesh,
      &queenMesh,
      &bishopMesh,
      &knightMesh,
      &rockMesh
    };
    for (int i = 0; i < 8; ++i)
    {
      Array<vertex> & m = *pieces[i];

      _generateTextureCoordinatesForPawn(m);
      _generateTextureCoordinatesForPawn(pawnMesh);

      glLoadIdentity();
      glTranslatef(_TV(1.5f) + 3.f * float(i - 4) + msys_sfrand() * _TV(0.3f),
		   _TV(1.f),
		   _TV(-10.5f) + msys_sfrand() * _TV(0.3f));
      Mesh::addVerticesUsingCurrentProjection(chessPiecesW, m);

      glLoadIdentity();
      glTranslatef(_TV(1.5f) + 3.f * float(i - 4) + msys_sfrand() * _TV(0.3f),
		   _TV(1.f),
		   _TV(-7.5f) + msys_sfrand() * _TV(0.3f));
      glRotatef(msys_sfrand() * _TV(3.f), 0, 1.f, 0);
      Mesh::addVerticesUsingCurrentProjection(chessPiecesW, pawnMesh);


      _generateTextureCoordinatesForPawn(m);
      _generateTextureCoordinatesForPawn(pawnMesh);


      glLoadIdentity();
      glTranslatef(_TV(1.5f) + 3.f * float(i - 4) + msys_sfrand() * _TV(0.3f),
		   _TV(1.f),
		   _TV(7.5f) + msys_sfrand() * _TV(0.3f));
      Mesh::addVerticesUsingCurrentProjection(chessPiecesB, pawnMesh);

      glLoadIdentity();
      glTranslatef(_TV(1.5f) + 3.f * float(i - 4) + msys_sfrand() * _TV(0.3f),
		   _TV(1.f),
		   _TV(10.5f) + msys_sfrand() * _TV(0.3f));
      glRotatef(180.f + msys_sfrand() * _TV(3.f), 0, 1.f, 0);
      Mesh::addVerticesUsingCurrentProjection(chessPiecesB, m);
    }
    Mesh::addTexCoordNoise(chessPiecesB, _TV(0.f));
    Mesh::addTexCoordNoise(chessPiecesW, _TV(0.f));
    VBO::setupData(VBO::chessPiecesW, chessPiecesW DBGARG("chessPiecesW"));
    VBO::setupData(VBO::chessPiecesB, chessPiecesB DBGARG("chessPiecesB"));
  }

  Node* create()
  {
    glPushMatrix();
    Node * chess = Node::New(START_DATE, END_DATE);
    Renderable chessPiecesW(Shader::parallax, VBO::chessPiecesW);
    Renderable chessPiecesB(Shader::parallax, VBO::chessPiecesB);
    chessPiecesW.setShininess(_TV(20));
    chessPiecesB.setShininess(_TV(60));
    chess->attachRenderable(chessPiecesW.setTextures(Texture::wood_TendreSec));
    chess->attachRenderable(chessPiecesB.setTextures(Texture::wood_Noyer));
    glPopMatrix();

    Renderable chessBoard(Shader::parallax, VBO::chessBoard);
    Renderable chessBorder(Shader::parallax, VBO::chessBorder);
    chessBoard.setShininess(_TV(40));
    chessBorder.setShininess(_TV(30));
    chess->attachRenderable(chessBoard.setTextures(Texture::chessBoard, Texture::chessBoardBump));
    chess->attachRenderable(chessBorder.setTextures(Texture::wood_Noyer));

    return chess;
  }
}
