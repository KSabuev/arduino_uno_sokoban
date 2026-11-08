void draw_bitmap(int16_t x, int16_t y, const uint16_t *bitmap, bool invert = false) {
  const uint8_t cell_end = CELL_SIZE - 1;

  tft.setAddrWindow(x, y, x + cell_end, y + cell_end);
  tft.pushColors((const uint8_t *)bitmap, CELL_SIZE * CELL_SIZE, 1, invert);
}

void draw_bitmap_rotated(int16_t x, int16_t y, const uint16_t *bitmap, uint8_t rot) {
  static uint8_t row_buf[CELL_SIZE * 2];
  const uint8_t n = CELL_SIZE;

  tft.setAddrWindow(x, y, x + n - 1, y + n - 1);

  for (uint8_t row = 0; row < n; row++) {
    for (uint8_t col = 0; col < n; col++) {
      uint16_t idx;
      switch (rot) {
        case 1: idx = (n - 1 - col) * n + row; break;
        case 2: idx = (n - 1 - col) + (n - 1 - row) * n; break;
        case 3: idx = col * n + (n - 1 - row); break;
        default: idx = col + row * n; break;
      }
      uint16_t p = pgm_read_word(bitmap + idx);
      row_buf[col * 2] = p & 0xFF;
      row_buf[col * 2 + 1] = p >> 8;
    }
    tft.pushColorsAt(x, y + row, row_buf, n * 2);
  }
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
    case MAN_PLACE: {
      uint8_t rot = 0;
      if (curButton == rightButt) rot = 1;
      else if (curButton == downButt) rot = 2;
      else if (curButton == leftButt) rot = 3;
      draw_bitmap_rotated(x_pos, y_pos, bulldozer, rot);
      break;
    }
    default:
      tft.fillRect(x_pos, y_pos, CELL_SIZE, CELL_SIZE, BLACK);
      break;
  }
}

bool is_level_won() {
  for (uint8_t y = 0; y < size_y; y++) {
    for (uint8_t x = 0; x < size_x; x++) {
      if (level[x][y] == BOX) return false;
    }
  }
  return true;
}

void show_victory() {
  tft.setTextSize(3);
  tft.setCursor(50, tft.height() / 2 - 10);
  tft.print("VICTORY! ");
  tft.print(menuIndex);
}

void show_level(uint8_t curButton) {
  for (uint8_t y = 0; y < size_y; y++) {
    for (uint8_t x = 0; x < size_x; x++) {
      uint8_t cell_type = level[x][y];
      draw_cell(offset_x + x * CELL_SIZE, offset_y + y * CELL_SIZE, cell_type, curButton);
    }
  }

  if (is_level_won()) show_victory();
}

void update_level(uint8_t curButton) {
  uint8_t last_pos = (undo_pos + UNDO_DEPTH - 1) % UNDO_DEPTH;
  MoveSnapshot &snap = undo_buffer[last_pos];

  for (uint8_t i = 0; i < snap.count; i++) {
    uint8_t x, y, prev_val;
    unpack_change(snap.changes[i], x, y, prev_val);
    draw_cell(offset_x + x * CELL_SIZE, offset_y + y * CELL_SIZE, level[x][y], curButton);
  }

  if (is_level_won()) show_victory();
}
