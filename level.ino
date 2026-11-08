struct LevelDecoder {
  const uint8_t* data;
  uint16_t buffer;
  uint8_t bit_pos;

  LevelDecoder(uint16_t level_num) {
    data = (const uint8_t*)pgm_read_ptr(&level_array[level_num]);
    buffer = (pgm_read_byte(data) << 8) | pgm_read_byte(data + 1);
    data += 2;
    bit_pos = 0;
  }

  uint8_t read_bits(uint8_t n) {
    uint8_t result = 0;

    for (uint8_t i = 0; i < n; i++) {
      result = (result << 1) | ((buffer & 0x8000) ? 1 : 0);
      buffer <<= 1;
      bit_pos++;

      if (bit_pos == 8) {
        buffer |= pgm_read_byte(data++);
        bit_pos = 0;
      }
    }

    return result;
  }
};

void load_level(uint8_t num_level) {
  clear_undo_buffer();
  clearLevel();

  // Создаем декодер для уровня
  LevelDecoder decoder(num_level);

  size_x = decoder.read_bits(5);
  size_y = decoder.read_bits(5);
  player_x = decoder.read_bits(5);
  player_y = decoder.read_bits(5);

  if (size_x > MAX_LEVEL_SIZE || size_y > MAX_LEVEL_SIZE) return;

  offset_x = (tft.width() - size_x * CELL_SIZE) / 2;
  offset_y = (tft.height() - size_y * CELL_SIZE) / 2;
  if (offset_x < 0) offset_x = 0;
  if (offset_y < 0) offset_y = 0;

  int total_cells = size_x * size_y;
  int m = 0;

  while (m < total_cells) {
    uint8_t n_replay;

    // признак повтора
    if (decoder.read_bits(1)) {
      uint8_t repeat_code = decoder.read_bits(3);  // 3 бита для количества повторений
      n_replay = repeat_code + 2;                  // от 2 до 9
    } else {
      n_replay = 1;
    }

    // объект: 2 бита
    uint8_t element = decoder.read_bits(2);

    for (uint8_t i = 0; i < n_replay; i++) {
      uint8_t x = m % size_x;
      uint8_t y = m / size_x;
      if (y < size_y) {
        level[x][y] = element;
      }
      m++;
    }
  }

  // установка игрока
  level[player_x][player_y] = MAN;
}

// void printLevel() {
//   Serial.println("=== Level Array ===");
//   for (int y = 0; y < 20; y++) {
//     for (int x = 0; x < 20; x++) {
//       Serial.print(level[x][y]);
//       Serial.print(" ");
//     }
//     Serial.println();
//   }
//   Serial.println("===================");
// }

void clearLevel() {
  memset(level, 0, MAX_LEVEL_SIZE * MAX_LEVEL_SIZE);  // Установит все байты в 0
}
