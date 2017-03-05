#ifndef		SYNC_HH
# define	SYNC_HH
# include "../src/array.hh"

// nombre d'instruments dont on veut la partition
# define NUMBER_OF_INSTR 24

namespace Instrument {
/*
  enum Id
  {
    Tictac = 0,
    MusicBox = 1,
    Angry = 2,
    Harp = 3,
    Lousianne = 4, // commence avec le cyclogratte
    Angry2 = 9, // on peut l'ignorer
    Hihat = 10, // cymbale
    AmbiantNoise = 11,
    Boum = 12, // inclut aussi la chute de livres au début
    FX1 = 13, // ouverture boite à musique
    Clap = 14,

    Carousel = 9, // inutilisé
    Xylo = 16,
    Orgalame = 17,
  };
*/
  enum Id
  {
    Tictac = 0,
    MusicBox = 1,
    Carousel = 2,
    Angry = 3,
    Harp = 4,
    Lousianne = 5,
    Angry2 = 6,
    Xylo = 8,
    Orgalame = 9,
    Hihat = 10,
    AmbiantNoise = 11,
    Boum = 12,
    FX1 = 13,
    Clap = 14,
  };
}

namespace Sync {
  struct Note
  {
    int time;
    int note;
    int vol;
  };

  // Utilisés par v2, ne pas utiliser ailleurs
  void playNote(int note, int vol, int channel, int time);
  void changeInstr(int ch, int instr);

  // Pour faciliter la synchro
  void getSheetForEachNote(Instrument::Id instr, Array<Note> out[128], int volumeMin);
  int getLastNote(Instrument::Id instr, int d);
  float getProgress(Instrument::Id intr, int time, int before, int after, int length);

  extern Array<Note> sheet[NUMBER_OF_INSTR];
}

#endif
