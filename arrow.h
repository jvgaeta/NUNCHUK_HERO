#include <stm32f30x.h>
#include "f3d_lcd_sd.h"

#define GOOD 1
#define GREAT 2
#define EXCELLENT 3
#define BAD 4
#define MISS 5
#define TARGET_X ST7735_width / 8
#define TARGET_WIDTH ST7735_width / 8
#define TARGET_X_CENTER TARGET_X + (TARGET_WIDTH / 2)
#define TARGET_Y ST7735_height / 2
#define TARGET_Y_CENTER TARGET_Y + (TARGET_WIDTH / 2)
#define GOOD_THR TARGET_WIDTH
#define GREAT_THR TARGET_WIDTH / 2
#define EXCELLENT_THR TARGET_WIDTH / 8

typedef struct arrow {
  uint8_t pos_x;
  uint8_t pos_y;
  uint8_t width;
  uint8_t depth;
  uint16_t color;
  uint8_t direction;
  char ch;
} arrow_t;
void initArrow(arrow_t *, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, int);
void drawArrow(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, int);
int moveArrow(arrow_t *, int8_t, int8_t, uint16_t);
void drawCharacter(uint8_t, uint8_t, uint16_t, uint8_t);
