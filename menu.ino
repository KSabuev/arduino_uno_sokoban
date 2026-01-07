void menu_action(byte curButton) {
  const byte maxIndex = sizeof(level_array) / sizeof(level_array[0]) - 1;

  // Отображение экрана только при первом входе или изменении состояния
  static byte lastMenuIndex = 255;  // отслеживаем, был ли уже перерисован экран

  // Обработка кнопок
  if (curButton == downButt && menuIndex < maxIndex) {
    menuIndex++;
    if (menuIndex >= scrollOffset + visibleCount) {
      scrollOffset++;
    }
  }

  if (curButton == upButt && menuIndex > 0) {
    menuIndex--;
    if (menuIndex < scrollOffset) {
      scrollOffset--;
    }
  }

  // Запуск уровня
  if (curButton == playQuit) {
    tft.fillScreen(BLACK);
    load_level();
    show_level();
    mode = !mode;
    Serial.print("перешли в игру: уровень ");
    Serial.println(menuIndex);
    lastMenuIndex = 255;  // Обновляем, чтобы экран перерисовывался в следующий раз
    return;
  }

  // Обновление экрана только при изменении menuIndex или первом входе в меню
  if (lastMenuIndex != menuIndex || lastMenuIndex == 255) {
    tft.fillScreen(BLACK);  // Очищаем экран

    // Заголовок меню
    tft.setTextColor(BLUE);
    tft.setCursor(100, 20);
    tft.setTextSize(3);
    tft.print("MENU");
    tft.setTextSize(2);

    // Выводим уровни, прокручивая их при необходимости
    for (byte i = 0; i < visibleCount; i++) {
      byte levelNumber = scrollOffset + i;
      if (levelNumber > maxIndex) break;

      tft.setTextColor(levelNumber == menuIndex ? GREEN : BLUE);
      tft.setCursor(40, (i + 2) * 35);  // (i + 2) — смещение вниз
      tft.print("Level ");
      tft.print(levelNumber);
    }

    // Запоминаем последний выбранный уровень
    lastMenuIndex = menuIndex;
  }
}