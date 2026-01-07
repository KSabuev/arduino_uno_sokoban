void game_action(byte curButton) {

  if (curButton != noButt) {
    move_player(curButton);
    show_level();
  }
  if (curButton == playQuit) {
    tft.fillScreen(BLACK);
    mode = !mode;
    Serial.println("перешли в меню");
  }
  if (curButton == back) {
    undo_move();
    show_level();
  }
}

void move_player(byte curButton) {
  int8_t dx = 0, dy = 0;
  CellChange changes[4];
  byte change_count = 0;

  switch (curButton) {
    case upButt: dy = -1; break;
    case downButt: dy = 1; break;
    case leftButt: dx = -1; break;
    case rightButt: dx = 1; break;
    default: return;
  }

  byte target_x = player_x + dx;
  byte target_y = player_y + dy;

  if (target_x >= size_x || target_y >= size_y) return;

  byte target_cell = level[target_x][target_y];

  bool is_player_on_place = (level[player_x][player_y] == MAN_PLACE);

  // пусто или место под ящик
  if (target_cell == VOID || target_cell == PLACE) {
    // освобождаем текущую клетку
    changes[change_count++] = { player_x, player_y, level[player_x][player_y] };
    level[player_x][player_y] = is_player_on_place ? PLACE : VOID;

    // ставим игрока
    changes[change_count++] = { target_x, target_y, level[target_x][target_y] };
    level[target_x][target_y] = (target_cell == PLACE) ? MAN_PLACE : MAN;

    player_x = target_x;
    player_y = target_y;
  }

  // если ящик
  else if (target_cell == BOX || target_cell == BOX_PLACE) {
    byte beyond_x = target_x + dx;
    byte beyond_y = target_y + dy;

    if (beyond_x >= size_x || beyond_y >= size_y) return;

    byte beyond_cell = level[beyond_x][beyond_y];

    if (beyond_cell == VOID || beyond_cell == PLACE) {
      // двигаем ящик
      changes[change_count++] = { beyond_x, beyond_y, level[beyond_x][beyond_y] };
      level[beyond_x][beyond_y] = (beyond_cell == PLACE) ? BOX_PLACE : BOX;

      // освобождаем игрока
      changes[change_count++] = { player_x, player_y, level[player_x][player_y] };
      level[player_x][player_y] = is_player_on_place ? PLACE : VOID;

      // ставим игрока
      changes[change_count++] = { target_x, target_y, level[target_x][target_y] };
      level[target_x][target_y] = (target_cell == BOX_PLACE) ? MAN_PLACE : MAN;

      player_x = target_x;
      player_y = target_y;
    }
  }
  if (change_count > 0) {
    save_move(changes, change_count);
  }
}

void draw_cell(int16_t x, int16_t y, const uint16_t* bitmap, bool transparent = false) {
  const uint8_t cell_size = 15;
  const uint8_t cell_end = cell_size - 1;

  tft.setAddrWindow(x, y, x + cell_end, y + cell_end);
  tft.pushColors((const uint8_t*)bitmap, cell_size * cell_size, 1, transparent);
}

void show_level() {
  tft.fillScreen(BLACK);

  const uint8_t cell_size = 15;

  const uint16_t screen_w = tft.width();   // 240
  const uint16_t screen_h = tft.height();  // 320

  const int16_t center_x = screen_w / 2 - cell_size / 2;
  const int16_t center_y = screen_h / 2 - cell_size / 2;

  uint8_t box_count = 0;

  for (byte y = 0; y < size_y; y++) {
    for (byte x = 0; x < size_x; x++) {
      int16_t x_pos = (x - player_x) * cell_size + center_x;
      int16_t y_pos = (y - player_y) * cell_size + center_y;

      if (x_pos + cell_size < 0 || x_pos >= screen_w || y_pos + cell_size < 0 || y_pos >= screen_h) {
        continue;
      }

      uint8_t cell = level[x][y];

      switch (cell) {
        case WALL:
          draw_cell(x_pos, y_pos, wall);
          break;

        case PLACE:
          draw_cell(x_pos, y_pos, place);
          break;

        case BOX:
          box_count++;
          draw_cell(x_pos, y_pos, box);
          break;

        case BOX_PLACE:
          draw_cell(x_pos, y_pos, box, true);
          break;

        case MAN:
        case MAN_PLACE:
          draw_cell(x_pos, y_pos, bulldozer);
          break;

        default:
          tft.fillRect(x_pos, y_pos, cell_size, cell_size, BLACK);
          break;
      }
    }
  }

  if (box_count == 0) {
    tft.setTextSize(3);
    tft.setCursor(screen_w / 2 - 60, screen_h / 2 - 10);
    tft.print("VIKTORI!");
  }
}

byte load_bit(byte x) {
  byte bufer = 0;
  num_level = 0;
  // первое чтение архива
  if (big_bufer == 0) {
    big_bufer = (pgm_read_byte(&level_array[num_level][0]) << 8) | pgm_read_byte(&level_array[num_level][1]);
    byte_index = 1;
    bit_counter = 0;
  }

  // побитовое чтение из буфера
  for (byte i = 0; i < x; i++) {
    bufer = (bufer << 1) | ((big_bufer & 0x8000) ? 1 : 0);
    big_bufer <<= 1;
    bit_counter++;

    if (bit_counter == 8) {
      byte_index++;
      big_bufer |= pgm_read_byte(&level_array[num_level][byte_index]);
      bit_counter = 0;
    }
  }

  return bufer;
}

void load_level() {
  big_bufer = 0;
  byte_index = 0;
  bit_counter = 0;

  size_x = load_bit(8);
  size_y = load_bit(8);

  player_x = load_bit(8);
  player_y = load_bit(8);

  int total_cells = size_x * size_y;
  int m = 0;

  while (m < total_cells) {
    byte n_replay;

    // признак повтора
    if (load_bit(1)) {
      byte repeat_code = load_bit(3);  // 3 бита для количества повторений
      n_replay = repeat_code + 2;      // от 2 до 9
    } else {
      n_replay = 1;
    }

    // объект: 2 бита
    byte element = load_bit(2);

    for (byte i = 0; i < n_replay; i++) {
      byte x = m % size_x;
      byte y = m / size_x;
      if (y < size_y) {
        level[x][y] = element;
      }
      m++;
    }
  }

  // установка игрока
  level[player_x][player_y] = 4;
}
