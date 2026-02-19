void save_move(uint16_t* packed_changes, uint8_t count) {
  MoveSnapshot& snap = undo_buffer[undo_pos];
  snap.count = count;
  for (uint8_t i = 0; i < count; i++) {
    snap.changes[i] = packed_changes[i];
  }
  undo_pos = (undo_pos + 1) % UNDO_DEPTH;
}

void undo_move() {
  uint8_t prev_pos = (undo_pos + UNDO_DEPTH - 1) % UNDO_DEPTH;
  
  if (undo_buffer[prev_pos].count == 0) return;
  
  MoveSnapshot& snap = undo_buffer[prev_pos];
  
  for (uint8_t i = 0; i < snap.count; i++) {
    uint8_t x, y, prev_val;
    unpack_change(snap.changes[i], x, y, prev_val);
    level[x][y] = prev_val;
  }
  // Ищем игрока
  for (uint8_t y = 0; y < size_y; y++) {
    for (uint8_t x = 0; x < size_x; x++) {
      if (level[x][y] == MAN || level[x][y] == MAN_PLACE) {
        player_x = x;
        player_y = y;
        break;
      }
    }
  }
  
  undo_buffer[prev_pos].count = 0;
  undo_pos = prev_pos;
}

uint16_t pack_change(uint8_t x, uint8_t y, uint8_t value) {
  return ((x & 0x1F) << 8) |    // 5 бит X
         ((y & 0x1F) << 3) |    // 5 бит Y
         (value & 0x07);        // 3 бита значение
}

void unpack_change(uint16_t data, uint8_t &x, uint8_t &y, uint8_t &value) {
  x = (data >> 8) & 0x1F;
  y = (data >> 3) & 0x1F;
  value = data & 0x07; 
}


void clear_undo_buffer() {
  for (uint8_t i = 0; i < UNDO_DEPTH; i++) {
    undo_buffer[i].count = 0;
  }
}