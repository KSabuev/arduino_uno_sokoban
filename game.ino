void game_action(uint8_t curButton) {

  if (curButton != noButt) {
    move_player(curButton);
    if (undo_pos > 0) {
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
  uint16_t changes[3];
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
