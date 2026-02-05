#define VISIBLE_COUNT 7  // сколько уровней влезает на экран
#define MAX_LEVEL_INDEX sizeof(level_array) / sizeof(level_array[0]) - 1

void menu_action(uint8_t curButton) {
  static uint16_t lastMenuIndex;
  static int16_t scrollOffset = 0;  // смещение прокрутки

  // Обработка кнопок
  if (curButton == downButt && menuIndex < MAX_LEVEL_INDEX) {
    menuIndex++;
    if (menuIndex >= scrollOffset + VISIBLE_COUNT) {
      scrollOffset++;
    }
  }

  if (curButton == rightButt && menuIndex < MAX_LEVEL_INDEX) {
    menuIndex += 50;
    if (menuIndex >= scrollOffset + VISIBLE_COUNT) {
      scrollOffset += 50;
    }
    if (MAX_LEVEL_INDEX < scrollOffset + VISIBLE_COUNT) {
      scrollOffset = MAX_LEVEL_INDEX - VISIBLE_COUNT + 1;
      menuIndex = MAX_LEVEL_INDEX;
    }
  }

  if (curButton == upButt && menuIndex > 0) {
    menuIndex--;
    if (menuIndex < scrollOffset) {
      scrollOffset--;
    }
  }

  if (curButton == leftButt && menuIndex >= 0) {
    menuIndex -= 50;
    if (menuIndex < scrollOffset) {
      scrollOffset -= 50;
    }
    if (0 > scrollOffset) {
      scrollOffset = 0;
      menuIndex = scrollOffset;
    }
  }

  // Запуск уровня
  if (curButton == playQuit) {
    mode = !mode;
    tft.fillScreen(BLACK);
    load_level(menuIndex);
    show_level(curButton);
    return;
  }

  // Обновление экрана только при изменении menuIndex или первом входе в меню
  if (lastMenuIndex != menuIndex || !flagMenu) {

    if (!flagMenu) {
      flagMenu = true;
      tft.setTextColor(BLUE);
      tft.setCursor(130, 10);
      tft.setTextSize(3);
      tft.print("MENU");
      tft.setTextSize(2);
    }

    // Выводим уровни, прокручивая их при необходимости
    for (uint16_t i = 0; i < VISIBLE_COUNT; i++) {
      uint16_t levelNumber = scrollOffset + i;
      if (levelNumber > MAX_LEVEL_INDEX) break;
      tft.setTextColor(levelNumber == menuIndex ? GREEN : BLUE, BLACK);
      tft.setCursor(40, i * 27 + 45);
      tft.print("Level ");
      tft.print((String)levelNumber + "  ");
    }

    // Запоминаем последний выбранный уровень
    lastMenuIndex = menuIndex;
  }
}