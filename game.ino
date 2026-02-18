void game_action(uint8_t curButton) {

  if (curButton != noButt) {
    move_player(curButton);
    if (undo_size > 0) {
      update_level(curButton);
    } else {
      show_level(curButton);
    }
  }
  if (curButton == playQuit) {
    tft.fillScreen(BLACK);
    flagMenu = false;
    mode = !mode;
  }
  if (curButton == back) {
    undo_move();
    show_level(curButton);
  }
}

void move_player(uint8_t curButton) {
  int8_t dx = 0, dy = 0;
  int16_t changes[4];
  uint8_t change_count = 0;

  switch (curButton) {
    case upButt: dy = -1; break;
    case downButt: dy = 1; break;
    case leftButt: dx = -1; break;
    case rightButt: dx = 1; break;
    default: return;
  }

  uint8_t target_x = player_x + dx;
  uint8_t target_y = player_y + dy;

  if (target_x >= size_x || target_y >= size_y) return;

  uint8_t target_cell = level[target_x][target_y];

  bool is_player_on_place = (level[player_x][player_y] == MAN_PLACE);

  // пусто или место под ящик
  if (target_cell == VOID || target_cell == PLACE) {
    // освобождаем текущую клетку
    changes[change_count++] = pack_change(player_x, player_y, level[player_x][player_y]);
    level[player_x][player_y] = is_player_on_place ? PLACE : VOID;

    // ставим игрока
    changes[change_count++] = pack_change(target_x, target_y, level[target_x][target_y]);
    level[target_x][target_y] = (target_cell == PLACE) ? MAN_PLACE : MAN;

    player_x = target_x;
    player_y = target_y;
  }

  // если ящик
  else if (target_cell == BOX || target_cell == BOX_PLACE) {
    uint8_t beyond_x = target_x + dx;
    uint8_t beyond_y = target_y + dy;

    if (beyond_x >= size_x || beyond_y >= size_y) return;

    uint8_t beyond_cell = level[beyond_x][beyond_y];

    if (beyond_cell == VOID || beyond_cell == PLACE) {
      // двигаем ящик
      changes[change_count++] = pack_change(beyond_x, beyond_y, level[beyond_x][beyond_y]);
      level[beyond_x][beyond_y] = (beyond_cell == PLACE) ? BOX_PLACE : BOX;

      // освобождаем игрока
      changes[change_count++] = pack_change(player_x, player_y, level[player_x][player_y]);
      level[player_x][player_y] = is_player_on_place ? PLACE : VOID;

      // ставим игрока
      changes[change_count++] = pack_change(target_x, target_y, level[target_x][target_y]);
      level[target_x][target_y] = (target_cell == BOX_PLACE) ? MAN_PLACE : MAN;

      player_x = target_x;
      player_y = target_y;
    }
  }
  if (change_count > 0) {
    save_move(changes, change_count);
  }
}

void draw_bitmap(int16_t x, int16_t y, const uint16_t *bitmap, bool transparent = false) {
  const uint8_t cell_end = CELL_SIZE - 1;

  tft.setAddrWindow(x, y, x + cell_end, y + cell_end);
  tft.pushColors((const uint8_t *)bitmap, CELL_SIZE * CELL_SIZE, 1);
}

void draw_cell(uint16_t x_pos, uint16_t y_pos, uint8_t cell_type, uint8_t curButton) {
  switch (cell_type) {
    case WALL:
      draw_bitmap(x_pos, y_pos, wall);
      break;
    case PLACE:
      draw_bitmap(x_pos, y_pos, place);
      break;
    case BOX:
      draw_bitmap(x_pos, y_pos, box);
      break;
    case BOX_PLACE:
      draw_bitmap(x_pos, y_pos, box, true);
      break;
    case MAN:
    case MAN_PLACE:
      if (curButton == rightButt) draw_bitmap(x_pos, y_pos, bulldozer_90);
      else if (curButton == downButt) draw_bitmap(x_pos, y_pos, bulldozer_180);
      else if (curButton == leftButt) draw_bitmap(x_pos, y_pos, bulldozer_270);
      else draw_bitmap(x_pos, y_pos, bulldozer);
      break;
    default:
      tft.fillRect(x_pos, y_pos, CELL_SIZE, CELL_SIZE, BLACK);
      break;
  }
}

void show_level(uint8_t curButton) {
  uint8_t box_count = 0;

  for (uint8_t y = 0; y < size_y; y++) {
    for (uint8_t x = 0; x < size_x; x++) {
      uint8_t cell_type = level[x][y];
      draw_cell(x * CELL_SIZE, y * CELL_SIZE, cell_type, curButton);
      if (cell_type == BOX) box_count++;
    }
  }

  if (box_count == 0) {
    tft.setTextSize(3);
    tft.setCursor(tft.width() / 2 - 60, tft.height() / 2 - 10);
    tft.print("VICTORY! " + (String)menuIndex);
  }
}

void update_level(uint8_t curButton) {
  uint8_t last_pos = (undo_pos + UNDO_DEPTH - 1) % UNDO_DEPTH;
  MoveSnapshot &snap = undo_buffer[last_pos];

  for (uint8_t i = 0; i < snap.count; i++) {
    uint8_t x, y, prev_val;
    unpack_change(snap.changes[i], x, y, prev_val);
    draw_cell(x * CELL_SIZE, y * CELL_SIZE, level[x][y], curButton);
  }
}

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

    size_x = decoder.read_bits(8);
    size_y = decoder.read_bits(8);
    player_x = decoder.read_bits(8);
    player_y = decoder.read_bits(8);

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