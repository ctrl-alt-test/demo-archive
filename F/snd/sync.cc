#include "../src/sys/msys_debug.h"
#include "../src/interpolation.hh"
#include "sync.hh"

namespace Sync {

  // association channel -> instrument
  int chanToInstr[17];

  Array<Note> sheet[NUMBER_OF_INSTR];

  // hack parce que la boite à musique et le xylotrain se retrouvent avec le même id :/
  static bool xyloFlag = false;

  void playNote(int note, int vol, int channel, int time)
  {
    time += 180; // décalage de 180ms ? :/
    int instr = channel; // chanToInstr[channel];
    if (instr < NUMBER_OF_INSTR)
    {
      // bruit de la porte
      if (instr == Instrument::MusicBox && time < 15000) return;

      if (time > 150000) { // bidouille pour la partie finale (après le xylo)
        if (instr == Instrument::Orgalame) return;
        if (instr == Instrument::MusicBox) instr = Instrument::Lousianne;
        else if (instr == Instrument::Lousianne) instr = Instrument::Orgalame;
      }
      // on ignore les notes "echo" et "off" de la lousianne, et les accords
      if (instr == Instrument::Lousianne) {
        if (vol < 65) return;
        if (sheet[instr].size > 0 && sheet[instr].last().time > time - 5) return;
      }
      if (instr == Instrument::Xylo) {
        // on ignore les notes "echo" et "off" du xylotrain
        if (vol < 65) return;
        // on supprime les notes trop proches du xylotrain :)
        if (sheet[instr].size > 0 && sheet[instr].last().time == time) return;
      }
      // pas de note off pour le pas-content
      if (instr == Instrument::Angry && vol == 0) return;
      // pas de note off pour le tambour
      if (instr == Instrument::Boum && vol == 0) return;

      //DBG("%6d: note %3d, %3d, %2d, %2d", time, note, vol, channel, instr);
      Note n = {time, note, vol};
      sheet[instr].add(n);
    }
    else
      DBG("%6d: untracked instrument %3d, %3d, %2d", time, note, vol, instr);
  }

  void changeInstr(int ch, int instr)
  {
    // FIXME: pourquoi on a parfois ch >= 16 ?
    if (ch == 2 && instr == 1) xyloFlag = true;
    if (ch <= 16)
      chanToInstr[ch] = instr;
    else
      DBG("instru %2d, %2d", ch, instr);
  }

  void getSheetForEachNote(Instrument::Id instr, Array<Sync::Note> out[128], int volumeMin)
  {
    // Hypothèse : les notes sont comprises entre 0 et 128 et il y a au
    // maximum 200 fois la même note (fréquence, instrument) dans la musique
    for (int i = 0; i < 128; i++)
      out[i].init(200);

    Array<Sync::Note> & arr = Sync::sheet[instr];
    for (int i = 0; i < arr.size; i++)
    {
      assert(arr[i].note < 128);
      if (arr[i].vol >= volumeMin)
        out[arr[i].note].add(arr[i]);
    }
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

  // Renvoie à quel moment de l'animation on en est rendu
  // C'est une fonction continue entre 0 et 1
  // Plus on est proche de 0, plus on est près de la note 
  //
  float getProgress(Instrument::Id instr, int time, int before, int after, int length)
  {
    const int d = max(0, time);

    int idx = Sync::getLastNote(instr, d);
    const Array<Sync::Note> & sheet = Sync::sheet[instr];
    int last = idx == -1 ? 0 : sheet[idx].time;
    int next = idx == sheet.size - 1 ? d+1 : sheet[idx + 1].time;

    last = max(d - before, last);
    next = min(d + after, next);

    float res = smoothStepI(last, next, d);
    res = 1.f - msys_fabsf(0.5f - res) * 2.f;

    // on ne remonte pas entièrement jusqu'à 1 dans les passages rapides
    res *= (min(length, next - last)) / (float)length;
    return res;
  }

}
