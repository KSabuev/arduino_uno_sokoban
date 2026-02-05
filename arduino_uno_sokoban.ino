#include "MCUFRIEND_kbv.h"
#include "bitmapRGB.h"
MCUFRIEND_kbv tft;

// menu
#define menuMode true
#define gameMode false

bool mode = menuMode;
volatile int16_t menuIndex = 0;
bool flagMenu = false;

// game
#define MAX_LEVEL_SIZE 20
uint8_t level[MAX_LEVEL_SIZE][MAX_LEVEL_SIZE] = {};
uint8_t player_x;
uint8_t player_y;

uint8_t size_x, size_y, man_x, man_y;

#define VOID 0       // пусто
#define WALL 1       // стена
#define BOX 2        // коробка
#define PLACE 3      // палет
#define MAN 4        // игрок
#define MAN_PLACE 5  // игрок на палете
#define BOX_PLACE 6  // ящик на палете

// undo
const uint8_t UNDO_DEPTH = 20;

struct MoveSnapshot {
  uint16_t changes[4];
  uint8_t count;
};

MoveSnapshot undo_buffer[UNDO_DEPTH];
uint8_t undo_pos = 0;
uint8_t undo_size = 0;

// collor
#define BLACK 0x0000
#define BLUE 0x001F
#define GREEN 0x07E0

// button
#define BUTTON_PIN A5

#define noButt 0
#define playQuit 1
#define back 2
#define leftButt 3
#define rightButt 4
#define downButt 5
#define upButt 6

void setup() {
  // Serial.begin(9600);
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  tft.setTextColor(BLUE);
}

void loop() {
  uint8_t curButton = ReadKey();

  switch (mode) {
    case (menuMode):
      menu_action(curButton);
      break;
    case (gameMode):
      game_action(curButton);
      break;
  }
}
