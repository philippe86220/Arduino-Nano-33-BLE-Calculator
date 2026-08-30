#include "Bouton.h"

void Bouton::begin(uint8_t pin, unsigned long delaiAntiRebond) {
  _pin = pin;
  _delaiAntiRebond = delaiAntiRebond;

  pinMode(_pin, INPUT_PULLUP);
  _derniereLecture = digitalRead(_pin);
  _etatStable = _derniereLecture;
  _instantChangement = millis();
}

bool Bouton::nouvelAppui() {
  const bool lecture = digitalRead(_pin);
  const unsigned long maintenant = millis();

  if (lecture != _derniereLecture) {
    _derniereLecture = lecture;
    _instantChangement = maintenant;
  }

  if ((maintenant - _instantChangement >= _delaiAntiRebond) &&
      (lecture != _etatStable)) {
    _etatStable = lecture;
    return _etatStable == LOW;
  }

  return false;
}
