#ifndef		MIDISYNC_HH
# define	MIDISYNC_HH
# include "../src/array.hh"

// nombre d'instruments dont on veut la partition
# define NUMBER_OF_INSTR 16

namespace Instrument {
  enum Id
  {
    MidiUntitled = 0,
    MidiGuitar = 1,
    MidiMuteGtr = 2,
    MidiClnGtr = 3,
    MidiSynth = 4,
    MidiSynBass = 5,
    MidiMainorg = 6,
    MidiDrum = 7,
    MidiNewOddMIDI = 5,
  };
}

namespace Sync {
  struct Note
  {
    int time;
    int pitch;
    int vol;
    int duration;
    int channel; // unused

  };
  bool operator >(const Note & lhs, const Note & rhs);

  bool init();

  // Pour faciliter la synchro
  int getLastNote(Instrument::Id instr, int d);
  int getLastNote(Instrument::Id instr, int d, int note);
  float getProgress(Instrument::Id intr, int time, int before, int after, int length);
  void getSheetForEachNote(Instrument::Id instr, Array<Note> out[128], int volumeMin);
  void exportSheet(const char * filename, const char * macroName);

  extern Array<Note> sheet[NUMBER_OF_INSTR];
}

#endif
