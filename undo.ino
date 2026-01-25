void save_move(CellChange* changes, uint8_t count) {
  MoveSnapshot& snap = undo_buffer[undo_pos];
  snap.count = count;

  for (uint8_t i = 0; i < count; i++) {
    snap.changes[i] = changes[i];
  }

  undo_pos = (undo_pos + 1) % UNDO_DEPTH;
  if (undo_size < UNDO_DEPTH) undo_size++;
}

bool undo_move() {
  if (undo_size == 0) return false;

  undo_pos = (undo_pos + UNDO_DEPTH - 1) % UNDO_DEPTH;
  MoveSnapshot& snap = undo_buffer[undo_pos];

  for (uint8_t i = 0; i < snap.count; i++) {
    uint8_t x = snap.changes[i].x;
    uint8_t y = snap.changes[i].y;
    level[x][y] = snap.changes[i].prev_value;
  }

  // ищем игрока
  for (uint8_t y = 0; y < size_y; y++) {
    for (uint8_t x = 0; x < size_x; x++) {
      if (level[x][y] == MAN || level[x][y] == MAN_PLACE) {
        player_x = x;
        player_y = y;
        break;
      }
    }
  }

  undo_size--;

  return true;
}

void clear_undo_buffer() {
  undo_size = 0;
}