void menu_action(uint8_t curButton) {
  const uint8_t maxIndex = sizeof(level_array) / sizeof(level_array[0]) - 1;
  static uint8_t lastMenuIndex;

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
    mode = !mode;
    tft.fillScreen(BLACK);
    load_level(0);
    show_level(curButton);
    return;
  }

  // Обновление экрана только при изменении menuIndex или первом входе в меню
  if (lastMenuIndex != menuIndex || !flagMenu) {
    tft.fillScreen(BLACK);  // Очищаем экран
    flagMenu = true;
    // Заголовок меню
    tft.setTextColor(BLUE);
    tft.setCursor(100, 20);
    tft.setTextSize(3);
    tft.print("MENU");
    tft.setTextSize(2);

    // Выводим уровни, прокручивая их при необходимости
    for (uint8_t i = 0; i < visibleCount; i++) {
      uint8_t levelNumber = scrollOffset + i;
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