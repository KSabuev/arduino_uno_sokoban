#include "MCUFRIEND_kbv.h"
#include "bitmapRGB.h"
MCUFRIEND_kbv tft;

// menu
const bool menuMode = true;
const bool gameMode = false;

bool mode = menuMode;
uint8_t menuIndex = 0;

uint8_t scrollOffset = 0;    // смещение прокрутки
const uint8_t visibleCount = 4; // сколько уровней влезает на экран

// game
uint8_t level[25][25] = {};
uint8_t num_level = 0;
uint8_t player_x;
uint8_t player_y;
uint8_t curLevel = 0; //номер текущего уровня

uint8_t size_level, size_x, size_y, man_x, man_y;
volatile uint16_t big_bufer = 0;
volatile uint8_t byte_index = 0;
volatile uint8_t bit_counter = 0;

#define VOID 0   // пусто
#define WALL 1   // стена
#define BOX 2    // коробка
#define PLACE 3  // палет
#define MAN 4    // игрок
#define MAN_PLACE   5   // игрок на палете
#define BOX_PLACE   6   // ящик на палете

// undo
const uint8_t UNDO_DEPTH = 10;
const uint8_t MAX_CHANGES = 4;

struct CellChange {
  uint8_t x, y;
  uint8_t prev_value;
};

struct MoveSnapshot {
  CellChange changes[MAX_CHANGES];
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

bool buttonPressed = false;
int prevValue = 0;
unsigned long debounceTime = 0;  // Время последнего изменения состояния

#define adc_key_1 980
#define adc_key_2 780
#define adc_key_3 600
#define adc_key_4 410
#define adc_key_5 240
#define adc_key_6 130

#define noButt 0
#define playQuit 1
#define back 2
#define leftButt 3
#define rightButt 4
#define downButt 5
#define upButt 6

void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
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

