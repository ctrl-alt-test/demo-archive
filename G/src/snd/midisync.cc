#include "midisync.hh"

#include "array.hxx"
#include "interpolation.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"

#if DEBUG
#define Array MidiArray // avoid conflicts with the Array class
# include "midifile\MidiFile.h"
#undef Array
#endif // DEBUG

namespace Sync {
#if DEBUG
  static const char * midiFilename = "data/sound/hand - Level One.mid";
#endif

  Array<Note> sheet[NUMBER_OF_INSTR];

  // Pour trier les notes par dates
  bool operator >(const Note & lhs, const Note & rhs)
  {
	  return lhs.time > rhs.time;
  }


  int tickToMS(int t, int tpq)
  {
    // FIXME: les donnees dependent du fichier midi.
    // Il faudrait faire un truc plus generique.
    long long time = t;
    long long result = 0;
    long long delta = 0;
#define move(maxtime, usq) delta = msys_min(time, (long long)(maxtime)); result += delta * usq / tpq; time -= delta;
    move(345600, 444444);
    move(2361600-345600, 480000);
    move(3705600-2361600, 535714);
    move(4569600-3705600, 517241);
    move(4723200-4569600, 500000);
    move(4876800-4723200, 491803);
    move(10000000, 500000);
    return (int) (result / 1000);
//music microseconds/tick: 0 - 444444
//music microseconds/tick: 345600 - 480000
//music microseconds/tick: 2361600 - 535714
//music microseconds/tick: 3705600 - 517241
//music microseconds/tick: 4569600 - 500000
//music microseconds/tick: 4723200 - 491803
//music microseconds/tick: 4876800 - 500000
  }

#if DEBUG

  // adapted from: http://sig.sapp.org/doc/examples/museinfo/midi/midi2melody/midi2melody.html
  void convertToMelody(MidiFile& midifile, int track) {
    midifile.absoluteTime();
    int numEvents = midifile.getNumEvents(track);
    int tpq = midifile.getTicksPerQuarterNote();
    MidiArray<int> state(128);   // for keeping track of the note states

    int i;
    for (i=0; i<128; i++) {
      state[i] = -1;
    }

    Note note;
    int command;
    int pitch;
    int velocity;

    for (i=0; i<numEvents; i++) {
      MFEvent ev = midifile.getEvent(track, i);
      command = ev.data[0] & 0xf0;
      if (command == 0x90) {
         pitch = ev.data[1];
         velocity = ev.data[2];
         if (velocity == 0) {
            // note off
            goto noteoff;
         } else {
            // note on
            state[pitch] = ev.time;
         }
      } else if (command == 0x80) {
         // note off
         pitch = ev.data[1];
noteoff:
         if (state[pitch] == -1) {
            continue;
         }
         note.time = tickToMS(state[pitch], tpq);
         note.duration = tickToMS(ev.time - state[pitch], tpq);
         note.pitch = pitch;
         note.vol = velocity;
         int channel = ev.data[0] & 0xF;
         note.channel = channel;
         sheet[track].add(note);
         state[pitch] = -1;
      } else if (command == 0xC0) { // program change
         int channel = ev.data[0] & 0xF;
         //DBG("channel %d, program %d", channel, ev.data[1]);
      } else if (ev.isTempo()) {
        //DBG("music microseconds/tick: %d - %ld", ev.time, ev.getTempoMicroseconds());
      } else if (ev.isMeta()) {
        //char text[50] = {0};
        //char metaType = ev.data[1];
        //if (metaType < 8) {
        //  for (int i = 0; i < ev.data[2]; i++)
        //    text[i] = ev.data[3 + i];
        //  DBG("[%d] meta '%s' on track %d", metaType, text, track);
        //}
      }
    }

	for (int i = 0; i < NUMBER_OF_INSTR; ++i)
	{
		sheet[i]._quickSort(0, sheet[i].size - 1);
	}
  }

#else

  #include "../../data/exported-midi-data.hh"

  int loadNote(Sync::Note & note, const int * data)
  {
    note.time = data[0];
    note.pitch = data[1];
    note.vol = data[2];
    note.duration = data[3];
    note.channel = data[4];
    return 5;
  }

  int loadInstrument(Array<Sync::Note> & notes, const int * data)
  {
    int index = 0;
    const int size = data[index++];
    if (size > 0)
    {
      notes.init(size);
      for (int i = 0; i < size; ++i)
      {
	Sync::Note note;
	index += loadNote(note, data + index);
	notes.add(note);
      }
    }
    return index;
  }

  void loadMelody()
  {
    int index = 0;
    const int instruments = notesData[index++];
    assert(instruments == NUMBER_OF_INSTR);
    for (int i = 0; i < instruments; ++i)
    {
      index += loadInstrument(sheet[i], notesData + index);
    }
    assert(index == ARRAY_LEN(notesData));
  }

#endif // DEBUG

  void printMelody(Array<Note>& melody) {
    int i;

    if (melody.size == 0) {
      return;
    }

    Note temp;
    temp.time = melody[melody.size - 1].time +
                melody[melody.size - 1].duration;
    temp.pitch = 0;
    temp.duration = 0;
    melody.add(temp);

    for (i=0; i<melody.size - 1; i++) {
      int delta = melody[i+1].time - melody[i].time;
      if (delta == 0) {
         continue;
      }

      DBG("%d\t%d\t%d\t%d", melody[i].time, melody[i].pitch, melody[i].vol, melody[i].channel);
      if (delta > melody[i].duration) {
        DBG("%d\t--", melody[i].time + melody[i].duration);
      }
   }
   DBG("%d\t--", melody[melody.size - 1].time);
  }

  bool init()
  {
    START_TIME_EVAL;

#if DEBUG
    MidiFile midifile(midiFilename);
    if (midifile.getTimeState() == 0)
        return false;
    for (int i = 0; i < NUMBER_OF_INSTR; i++)
      sheet[i].init(10000); // number of notes per instr
    for (int i = 0; i < midifile.getNumTracks(); i++)
      convertToMelody(midifile, i);
    exportSheet("data/exported-midi-data.hh", "MIDI_EXPORTED_DATA");
#else
    loadMelody();
#endif
    //for (int i = 0; i < NUMBER_OF_INSTR; i++) {
    //  DBG("======================================== track %d", i);
    //  printMelody(sheet[i]);
    //}
    END_TIME_EVAL("Midi loading");
    return true;
  }

  // retourne l'indice de la dernière note jouée (-1 au début)
  int getLastNote(Instrument::Id instr, int d)
  {
    static int lastTimes[20] = {0};
    const Array<Sync::Note> & sheet = Sync::sheet[instr];

    int i = lastTimes[instr];
    if (i > 0 && sheet[i-1].time > d) i = 0; // retour dans le temps

    for (; i < sheet.size; i++)
      if (sheet[i].time > (int) d)
	break;

    lastTimes[instr] = i;
    return i - 1;
  }

  int getLastNote(Instrument::Id instr, int d, int note)
  {
    static int lastTimes[20] = {0};
    const Array<Sync::Note> & sheet = Sync::sheet[instr];

    int i = lastTimes[instr];
    if (i > 0 && sheet[i].time > d) i = 0; // retour dans le temps

    int lastNote = -1;
    for (; i < sheet.size; i++)
    {
      const Sync::Note& stroke = sheet[i];
      if (stroke.time > (int) d)
	break;
	  else if (stroke.pitch == note && stroke.vol > 0)
	lastNote = i;
    }

	if (lastNote >= 0)
	{
		const Sync::Note& found = sheet[lastNote];
		const Sync::Note& tooFar = sheet[i];
		lastTimes[instr] = lastNote;
	}
    return lastNote;
  }

  // Renvoie à quel moment de l'animation on en est rendu
  // C'est une fonction continue entre 0 et 1
  // Plus on est proche de 0, plus on est près de la note 
  //
  float getProgress(Instrument::Id instr, int time, int before, int after, int length)
  {
    const int d = msys_max(0, time);

    int idx = Sync::getLastNote(instr, d);
    if (idx < 0)
      return 0;
    Sync::Note note = Sync::sheet[instr][idx];
    if (time >= note.time + note.duration)
      return 0;
    return 1.f - interpolate(time, note.time, note.time + note.duration);
  }

  void getSheetForEachNote(Instrument::Id instr, Array<Note> out[128], int volumeMin)
  {
    // Hypothèse : les notes sont comprises entre 0 et 128 et il y a au
    // maximum 200 fois la même note (fréquence, instrument) dans la musique
    for (int i = 0; i < 128; i++)
      out[i].init(200);

    Array<Sync::Note> & arr = Sync::sheet[instr];
    for (int i = 0; i < arr.size; i++)
    {
      assert(arr[i].pitch < 128);
      if (arr[i].vol >= volumeMin)
        out[arr[i].pitch].add(arr[i]);
    }
  }

#if DEBUG

  void exportNote(FILE * fp, const Sync::Note & note)
  {
	  assert(fp != NULL);
	  fprintf(fp, "    %d, %d, %d, %d, %d,\n",
		  note.time, note.pitch, note.vol, note.duration, note.channel);
  }

  void exportNotes(FILE * fp, const Array<Sync::Note> & notes)
  {
	  assert(fp != NULL);
	  fprintf(fp, "    %d,\n", notes.size);
	  for (int i = 0; i < notes.size; ++i)
	  {
		  exportNote(fp, notes[i]);
	  }
	  fprintf(fp, "\n");
  }

  void exportSheet(const char * filename, const char * macroName)
  {
	  FILE * fp = fopen(filename, "w");
	  assert(fp != NULL);
	  fprintf(fp, "// Generated file\n\n#ifndef %s\n#define %s\n\n", macroName, macroName);
	  fprintf(fp, "const int notesData[] = {\n");
	  fprintf(fp, "    %d,\n", NUMBER_OF_INSTR);
	  for (int i = 0; i < NUMBER_OF_INSTR; ++i)
	  {
		  exportNotes(fp, Sync::sheet[i]);
	  }
	  fprintf(fp, "};\n\n#endif\n");
	  fclose(fp);
  }

#endif // DEBUG

}
