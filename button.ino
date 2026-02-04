uint8_t ReadKey() {
  static int prevValue = 0;
  static unsigned long debounceTime = 0;
  static unsigned long nextPressTime = 0;
  static uint8_t heldButton = noButt;
  
  uint16_t currentValue = analogRead(BUTTON_PIN);
  unsigned long now = millis();

  if (abs(currentValue - prevValue) > 10) {
    debounceTime = now;
    prevValue = currentValue;
  }
  if (now - debounceTime < 10) return noButt;
  uint8_t currentButton = noButt;
  if (currentValue > 130) {
    if (currentValue > 980) currentButton = playQuit;
    else if (currentValue > 780) currentButton = back;
    else if (currentValue > 600) currentButton = leftButt;
    else if (currentValue > 410) currentButton = rightButt;
    else if (currentValue > 240) currentButton = downButt;
    else currentButton = upButt;
  }
  if (currentButton != noButt) {
    if (heldButton == noButt) {
      heldButton = currentButton;
      nextPressTime = now + 500;
      return currentButton;
    } else if (currentButton == heldButton && now >= nextPressTime) {
      nextPressTime = now + 500;
      return currentButton;
    }
  } else {
    heldButton = noButt;
  }

  return noButt;
}