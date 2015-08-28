/*draws an arrow with specified coordiantes
direction specifies: 1 up, 2 right, 3 down, 4 left*/
#include "arrow.h"
// TO-DO LIST:
// 1. Port to STM
// 2. Make it a rhythm game
/*draws an outline of a rectangle with specified coordinates*/
void drawTarget(uint8_t x, uint8_t y, uint8_t width, uint8_t depth, uint16_t color) {
  int i;
  uint16_t buf[width*depth+1];
  for (i = 0; i < width*depth; i++) {
    buf[i] = color;
  }
  f3d_lcd_setAddrWindow (x, y, x+width-1, y+1, MADCTLGRAPHICS);
  f3d_lcd_pushColor(buf,width*depth);
  f3d_lcd_setAddrWindow (x, y+depth-1, x+width-1, y+depth, MADCTLGRAPHICS);
  f3d_lcd_pushColor(buf,width*depth);
  f3d_lcd_setAddrWindow (x, y, x+1, y+depth-1, MADCTLGRAPHICS);
  f3d_lcd_pushColor(buf,width*depth);
  f3d_lcd_setAddrWindow (x+width-1, y, x+width, y+depth, MADCTLGRAPHICS);
  f3d_lcd_pushColor(buf,width*depth);
}

void redrawTarget(uint8_t x, uint8_t y, uint8_t width, uint8_t depth, uint16_t color) {
  int i;
  uint16_t buf[width*depth+1];
  for (i = 0; i < width*depth; i++) {
    buf[i] = color;
  }
  f3d_lcd_setAddrWindow (x, y, x+1, y+depth-1, MADCTLGRAPHICS);
  f3d_lcd_pushColor(buf,width*depth);
  f3d_lcd_setAddrWindow (x+width-1, y, x+width, y+depth, MADCTLGRAPHICS);
  f3d_lcd_pushColor(buf,width*depth);
}

void drawCharacter(uint8_t x, uint8_t y, uint16_t color, uint8_t direction) {
  if(direction == 5) {
    f3d_lcd_setAddrWindow (x-3, y-3, x+2, y+2, MADCTLGRAPHICS);
    uint16_t char_buf[36] = {color, color, color, color, color, color,
                            color, 0, 0, 0, 0, 0,
                            color, 0, 0, 0, 0, 0,
                            color, 0, 0, 0, 0, 0,
                            color, 0, 0, 0, 0, 0,
                            color, color, color, color, color, color};
    f3d_lcd_pushColor(char_buf,36);
  } else if(direction == 6) {
    f3d_lcd_setAddrWindow (x-3, y-3, x+2, y+2, MADCTLGRAPHICS);
    uint16_t char_buf[36] = {color, color, color, color, color, color,
                                             0, 0, 0, 0, color, 0,
                                      0, 0, 0, color, 0, 0,
                                   0, 0, color, 0, 0, 0,
                            0, color, 0, 0, 0, 0,
                            color, color, color, color, color, color};
    f3d_lcd_pushColor(char_buf,36);
  }
}

void initArrow(arrow_t *arrow, uint8_t x, uint8_t y, uint8_t width, uint8_t depth, uint16_t color, int direction) {
  arrow->pos_x = x;
  arrow->pos_y = y;
  arrow->width = width;
  arrow->depth = depth;
  arrow->color = color;
  arrow->direction = direction;
  if(direction < 5) { 
    drawArrow(arrow->pos_x, arrow->pos_y, arrow->width, arrow->depth, arrow->color, arrow->direction);
  } else {
    drawCharacter(arrow->pos_x, arrow->pos_y, arrow->color, arrow->direction);
  }
}

void drawArrow(uint8_t x, uint8_t y, uint8_t width, uint8_t depth, uint16_t color, int direction) {
  int i;
  uint16_t buf[depth+1];
  for (i = 0; i < depth; i++) {
    buf[i] = color;
  }
  if(direction % 2) {
    y = y + 4;
    f3d_lcd_setAddrWindow (x, y, x, y+depth-1, MADCTLGRAPHICS);
    f3d_lcd_pushColor(buf,depth);

    if(direction == 1) {
      // Up arrow
      f3d_lcd_setAddrWindow (x-2, y-2, x+2, y, MADCTLGRAPHICS);
      uint16_t arrow_buf[15] = {0, 0, color, 0, 0,
                                0, color, color, color, 0,
                                color, color, color, color, color};
      f3d_lcd_pushColor(arrow_buf,15);
    } else if(direction == 3) {
      // Down arrow
      f3d_lcd_setAddrWindow (x-2, y+depth, x+2, y+depth+2, MADCTLGRAPHICS);
      uint16_t arrow_buf[15] = {color, color, color, color, color,
                                0, color, color, color, 0,
                                0, 0, color, 0, 0};
      f3d_lcd_pushColor(arrow_buf,15);
    }
  } else {
    y = y + 8;
    f3d_lcd_setAddrWindow (x, y, x+width-1, y, MADCTLGRAPHICS);
    f3d_lcd_pushColor(buf,depth);
    if(direction == 2) {
      // Right arrow
      f3d_lcd_setAddrWindow (x+width, y-2, x+width+2, y+2, MADCTLGRAPHICS);
      uint16_t arrow_buf[15] = {color, 0, 0,
                                color, color, 0,
                                color, color, color,
                                color, color, 0,
                                color, 0, 0}; 
      f3d_lcd_pushColor(arrow_buf,15);
    } else if(direction == 4) {
      // Left arrow
      f3d_lcd_setAddrWindow (x-2, y-2, x, y+2, MADCTLGRAPHICS);
      uint16_t arrow_buf[15] = {0, 0, color,
                                0, color, color,
                                color, color, color,
                                0, color, color,
                                0, 0, color}; 
      f3d_lcd_pushColor(arrow_buf,15);
    }
  }
}

/*moves an arrow given an arrow and the velocity in both the x and y direction (as well as a background color)*/
int moveArrow(arrow_t *arrow, int8_t delta_x, int8_t delta_y, uint16_t background_color) {
  int xtemp;
  int ytemp;
  int collision = 0;

  // erase current rectangle 
  if(arrow->direction < 5) {
    drawArrow(arrow->pos_x, arrow->pos_y, arrow->width, arrow->depth, background_color, arrow->direction);
  } else {
    drawCharacter(arrow->pos_x, arrow->pos_y, background_color, arrow->direction);
  }
  // update x,y postion based on deltas, 
  xtemp = (int) (arrow->pos_x + delta_x);   // cast as int to gain benefit of sign and larger size 
  ytemp = (int) (arrow->pos_y + delta_y);
  // Check for arrow collisions
  if(arrow->direction < 5) {
    if (xtemp < TARGET_X_CENTER + EXCELLENT_THR && xtemp > TARGET_X_CENTER - EXCELLENT_THR) {
      collision = EXCELLENT;
    } else if (xtemp < TARGET_X_CENTER + GREAT_THR && xtemp > TARGET_X_CENTER - GREAT_THR) {
      collision = GREAT;
    } else if (xtemp < TARGET_X_CENTER + GOOD_THR && xtemp > TARGET_X_CENTER - GOOD_THR) {
      collision = GOOD;
    } else if (xtemp < TARGET_X_CENTER - GOOD_THR - 5) {
      collision = MISS;
    } else {
      collision = BAD;
    }
  } else {
    if (ytemp < TARGET_Y_CENTER + EXCELLENT_THR && ytemp > TARGET_Y_CENTER - EXCELLENT_THR) {
      collision = EXCELLENT;
    } else if (ytemp < TARGET_Y_CENTER + GREAT_THR && ytemp > TARGET_Y_CENTER - GREAT_THR) {
      collision = GREAT;
    } else if (ytemp < TARGET_Y_CENTER + GOOD_THR && ytemp > TARGET_Y_CENTER - GOOD_THR) {
      collision = GOOD;
    } else if (ytemp < TARGET_Y_CENTER - GOOD_THR - 5) {
      collision = MISS;
    } else {
      collision = BAD;
    }
  }
  arrow->pos_x = (uint8_t) xtemp;
  arrow->pos_y = (uint8_t) ytemp;

  if(xtemp < 0 || ytemp < 0) { 
    arrow->direction = 100;
  } else {
    if(arrow->direction < 5) {
      drawArrow(arrow->pos_x, arrow->pos_y, arrow->width, arrow->depth, arrow->color, arrow->direction);
    } else {
      drawCharacter(arrow->pos_x, arrow->pos_y, arrow->color, arrow->direction);
    }
  }
  return collision;
}

