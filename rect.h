/* rect.h --- 
 * 
 * Filename: rect.h
 * Description: 
#include <stm32f30x.h>
#include "f3d_lcd_sd.h"

#define COLLISION_TOP 1
#define COLLISION_BOTTOM 2
#define COLLISION_LEFT 3
#define COLLISION_RIGHT 4

typedef struct rect {
  uint8_t pos_x;
  uint8_t pos_y;
  uint8_t width;
  uint8_t depth;
  uint16_t color;
} rect_t;

const int colors[8];

void initRect(rect_t *, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t);
int eraseRect(rect_t *, uint16_t);
int redrawRect(rect_t *);
int moveRect(rect_t *, int8_t, int8_t, uint16_t);



/* rect.h ends here */
