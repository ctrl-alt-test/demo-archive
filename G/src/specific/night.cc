#include "algebra/vector3.hxx"
#include "array.hxx"
#include "interpolation.hxx"
#include "intro.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "night.hh"
#include "node.hxx"
#include "randomness.hh"
#include "renderable.hxx"
#include "shaders.hh"
#include "snd/midisync.hh"
#include "timing.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys.h"

namespace Night
{
  static Array<Sync::Note> notes[128];

  static char *pic[] = {
    "          L         ",
    "         MNM        ",
    "        N O N       ",
    "   HIQPO  G  O KJ   ",
    "   IH    GFG   JK   ",
    "   P    H E H   Q   ",
    "   O      D     P   ",
    "  N  H    C     O   ",
    " M  G   RRBS  H  N  ",
    "LN GFEDCBAAS   G  M ",
    " M  G   SAABCDEFG NL",
    "  N  H  SBRR   G  M ",
    "   O     C    H  N  ",
    "   P     D      O   ",
    "   Q   H E H    P   ",
    "   KJ   GFG    HI   ",
    "   JKPO  G  OPQIH   ",
    "       N   N        ",
    "        MNM         ",
    "         L          ",
  };

  static float computeTransition(int objid)
  {
    int time = (int) intro.now.youtube;

    int index = Sync::getLastNote(Instrument::MidiUntitled, time);
    if (index < 0) return 0.f;
    index %= _TV(19);

    int x = objid / 100;
    int y = objid % 100;

    int case_val = pic[x][y] - 'A';
    // Case vide
    if (pic[x][y] == ' ') return _TV(0.f);
    // Case pas encore allumee
    if (case_val > index) return _TV(0.f);
    // Case allumee depuis longtemps
    if (index - case_val > _TV(9)) return _TV(0.f);
    // Case allumee
    if (case_val < index) return _TV(0.01f);

    // Case qui s'allume
    const Array<Sync::Note> & sheet = Sync::sheet[Instrument::MidiUntitled];
    const Sync::Note & note = sheet[index];
    const int noteDuration = _TV(100); // en ms
    float col = 1.f - smoothStep(note.time, note.time + noteDuration, time);
    return mix(_TV(1.f), _TV(0.01f), col);
  }

  void generateCelestialSphere()
  {
    const float sphereRadius = _TV(500.f);
    const float minStarSize = _TV(0.25f);
    const float maxStarSize = _TV(1.0f);
    const int numberOfStars = _TV(4000);

    //
    //   0 4
    //  /| |\
    // 3 | | 5
    // 2 | | 6
    //  \| |/
    //   1 7
    //
    const float cos30 = 0.8660254f;
    const vector2f points[] = {
      vector2f(  0.0f,  1.0f),
      vector2f(  0.0f, -1.0f),
      vector2f(-cos30, -0.5f),
      vector2f(-cos30,  0.5f),

      vector2f(  0.0f,  1.0f),
      vector2f( cos30,  0.5f),
      vector2f( cos30, -0.5f),
      vector2f(  0.0f, -1.0f),
    };

    Mesh::MeshStruct & star = Mesh::getTempMesh();
    star.generate2DShape(points, 8, false, true);


    Mesh::MeshStruct & celestialSphere = Mesh::getTempMesh();

    Rand rand;
    int count = 0;
    while (count < numberOfStars)
    {
      // Répartition uniforme des étoiles
      vector3f p(rand.fgen(-sphereRadius, sphereRadius),
		 rand.fgen(-sphereRadius, sphereRadius),
		 rand.fgen(-sphereRadius, sphereRadius));
//       if (p.y < 0.f) continue;
      const float distance = norm(p);
      if (distance == 0.f || distance > sphereRadius) continue;

      p *= sphereRadius / distance;
      const float phi = msys_acosf(p.y / sphereRadius);
      const float theta = msys_atan2f(p.z, p.x);

      const float size = rand.fgen(minStarSize, maxStarSize);

      matrix4 transform = matrix4::rotation(DEG_TO_RAD * theta * 180.f / PI, vector3f::uy);
      transform.rotate(phi * 180.f / PI, vector3f::uz);
      transform.translate(vector3f(0.f, sphereRadius, 0.f));
      transform.scale(vector3f(size, size, size));

      star.setId(float(count % 128));
      celestialSphere.add(star, transform);

      ++count;
    }
    SETUP_VBO(celestialSphere, celestialSphere.vertices);
  }

  void generateMesh()
  {
    Mesh::clearPool();

    generateCelestialSphere();

    Mesh::MeshStruct & ground = Mesh::getTempMesh();
    Mesh::Pave(_TV(8.f), 0, _TV(8.f)).generateMesh(ground, Mesh::pave_top);
    ground.computeBaryCoord();
    SETUP_VBO(nightGround, ground);
  }

  Node * create(date startDate, date endDate)
  {
    Node * root = Node::New(startDate, endDate);

    IFDBG(if (!intro.initDone))
    Sync::getSheetForEachNote(Instrument::MidiUntitled, notes, 65);

    glLoadIdentity();
    glRotatef(-42.f, 1.f, 0.f, 0.f);
    Node * sphere = Node::New(startDate, endDate);
    Renderable celestialSphere(Material::star, VBO_(celestialSphere));
    celestialSphere.setTransitionFct(computeTransition);
    sphere->attachRenderable(celestialSphere);
    sphere->setAnimation(celestialSphereAnimation);
    sphere->attachToNode(root);

    for (int i = 0; i < _TV(20); i++)
    for (int j = 0; j < _TV(20); j++)
    {
      glLoadIdentity();
      glTranslatef(_TV(0.f) + (i - 10) * _TV(8.f),
		   _TV(0.f),
		   _TV(0.f) + (j - 10) * _TV(8.f));
      Node * node = Node::New(startDate, endDate);
      Renderable ground(Material::danceFloor, VBO_(nightGround));
      ground.setId(i * 100 + j);
      ground.setTransitionFct(computeTransition);
      node->attachRenderable(ground);
      node->attachToNode(root);
    }

    return root;
  }

  void celestialSphereAnimation(const Node & node, date d)
  {
    const int circleDuration = _TV(600000);
    const float progression = ((int)d % circleDuration) / float(circleDuration);

    glRotatef(-360.f * progression, 0, 0, 1.f);
  }
}
