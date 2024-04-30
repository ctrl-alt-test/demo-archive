//
// Définition des temps absolus en ms pour le déroulement
//

#include "timing.hh"
#include "tweakval.hh"
#include "sys/msys_libc.h"

const date worldStartDate = 0;
const date worldEndDate = worldStartDate + ((5*60 + 1) * 1000 + 700);

const date demoEndDate = worldEndDate;

namespace Demo
{
  // Compute the time in the demo world.
  Clock computeClock(date youtubeTime)
  {
    Clock targetTime;
    targetTime.story = youtubeTime;

    youtubeTime += _TV(2000); // à cause du hack temporel :/

    date timeMapping[] = {
      // Change le temps de la caméra pendant 'durée' ms.
      // youtube      durée      camera
               0,          0,           0,
      _TV(258000), _TV(400), _TV(303000),
      _TV(262000), _TV(400), _TV(307000),
      _TV(266000), _TV(400), _TV(308100),
      _TV(270000), _TV(400), _TV(309100),
      _TV(274000), _TV(0), _TV(313400),
      _TV(275000), _TV(3100),_TV(320100),
      _TV(278000), _TV(0), _TV(310100),
      _TV(278500), _TV(200), _TV(310600),
      _TV(279000), _TV(200), _TV(311100),
      _TV(279500), _TV(200), _TV(311600),
      _TV(280000), _TV(200), _TV(312100),
      _TV(280500), _TV(200), _TV(312600),
      _TV(281000), _TV(200), _TV(313100),
      _TV(284500), _TV(1500), _TV(318100),
      _TV(289200), _TV(1600), _TV(337400),
      // lignes en rab
      _TV(990000), _TV(200), _TV(300000),
      _TV(990000), _TV(200), _TV(300000),
      _TV(990000), _TV(200), _TV(300000),
      _TV(990000), _TV(200), _TV(300000),
      _TV(990000), _TV(200), _TV(300000),
    };

    int i;
    for (i = 0; i < ARRAY_LEN(timeMapping); i += 3)
    {
      if (timeMapping[i + 3] > youtubeTime) break;
    }
    if (youtubeTime > timeMapping[i] + timeMapping[i+1])
      targetTime.camera = youtubeTime;
    else
      targetTime.camera = youtubeTime - timeMapping[i] + timeMapping[i + 2];

    targetTime.camera += _TV(-2000); // à cause du hack temporel :/
    return targetTime;
  }
}
