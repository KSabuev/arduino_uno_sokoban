byte ReadKey() {
  int currentValue = analogRead(BUTTON_PIN);

  if (abs(currentValue - prevValue) > 10) {
    debounceTime = millis();
    prevValue = currentValue;
  }

  if (millis() - debounceTime >= 10) {
    if (!buttonPressed) {
      if (currentValue > adc_key_6) {
        buttonPressed = true;
        if (currentValue > adc_key_1) return playQuit;
        if (currentValue > adc_key_2) return back;
        if (currentValue > adc_key_3) return leftButt;
        if (currentValue > adc_key_4) return rightButt;
        if (currentValue > adc_key_5) return downButt;
        return upButt;
      }
    } else {
      if (currentValue < adc_key_6) {
        buttonPressed = false;
      }
    }
  }
  return noButt;
}