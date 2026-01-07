#include "MCUFRIEND_kbv.h"
#include "bitmapRGB.h"
MCUFRIEND_kbv tft;

// menu
const bool menuMode = true;
const bool gameMode = false;

bool mode = menuMode;
byte menuIndex = 0;

byte scrollOffset = 0;    // смещение прокрутки
const byte visibleCount = 4; // сколько уровней влезает на экран



// game
byte level[25][25] = {};
byte num_level = 0;
int player_x;
int player_y;
byte curLevel = 0; //номер текущего уровня

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
const byte UNDO_DEPTH = 10;
const byte MAX_CHANGES = 4;

struct CellChange {
  byte x, y;
  byte prev_value;
};

struct MoveSnapshot {
  CellChange changes[MAX_CHANGES];
  byte count;
};

MoveSnapshot undo_buffer[UNDO_DEPTH];
byte undo_pos = 0;
byte undo_size = 0;

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
  byte curButton = ReadKey();

  switch (mode) {
    case (menuMode):
      menu_action(curButton);
      break;
    case (gameMode):
      game_action(curButton);
      break;
  }
  }

