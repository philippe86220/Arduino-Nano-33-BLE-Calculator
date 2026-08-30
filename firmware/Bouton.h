#ifndef BOUTON_H
#define BOUTON_H

#include <Arduino.h>

class Bouton {
 public:
  void begin(uint8_t pin, unsigned long delaiAntiRebond = 20);
  bool nouvelAppui();

 private:
  uint8_t _pin = 0;
  bool _derniereLecture = HIGH;
  bool _etatStable = HIGH;
  unsigned long _instantChangement = 0;
  unsigned long _delaiAntiRebond = 20;
};

#endif
