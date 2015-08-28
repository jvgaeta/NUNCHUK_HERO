/* pong.c --- 
 * 
#include "rect.h"
#include "arrow.h"
#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <f3d_systick.h>
#include <ff.h>
#include <diskio.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <f3d_dac.h>

#define TIMER 20000
#define AUDIOBUFSIZE 15000
#define EVENT_LOOP_TIME 20
#define EXC_SCORE 50 
#define GREAT_SCORE 25
#define GOOD_SCORE 10
#define SCORE_X ST7735_width - 70  
#define SCORE_Y ST7735_width / 10 
#define LIVES 5

extern uint8_t Audiobuf[AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole; 

arrow_t current_char;
arrow_t current_arrow;
arrow_t arrows[4];
arrow_t chars[2]; 
rect_t stars[20];
FATFS Fatfs;    /* File system object */
FRESULT rc;     /* Result code */
UINT bw, br;
FIL Fil;    /* File object */
FIL fid;    /* File object */
BYTE Buff[3*128];   /* File read buffer */
int ret;
int arrow_vx = -3;
int arrow_vy = -1;
int in_game = 0;
int btn_down, btn_down_2, user_input, user_input_2, score, lives;
int high_score = 0;
int score_thr = 250;
char score_string[10];
nunchuk_t j;
unsigned char jx, jy, c, z;

struct ckhd {
  uint32_t ckID;
  uint32_t cksize;
};

struct fmtck {
  uint16_t wFormatTag;      
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

/* Game over screen */
void gameOver(void) {
  arrow_vx = -1;
  arrow_vy = -1;
  score_thr = 250;
  f3d_lcd_fillScreen(BLACK);
  f3d_lcd_drawString(ST7735_width / 4 + 10, TARGET_Y - 20, "GAME OVER", RED, BLACK);
  sprintf(score_string,"Score: %d", score);
  f3d_lcd_drawString(ST7735_width / 4, TARGET_Y, score_string, WHITE, BLACK);
  if(score > high_score) {
    high_score = score;
    f3d_lcd_drawString(ST7735_width / 4, TARGET_Y + 20, "NEW HIGH SCORE!", WHITE, BLACK);
  } else {
    /* Display old high score */
    sprintf(score_string,"High Score: %d", high_score);
    f3d_lcd_drawString(ST7735_width / 6, TARGET_Y + 20, score_string, WHITE, BLACK);
  }
   while(in_game == 2) {
    pollEvent();
    if(user_input_2 == 5) {
      in_game = 0;
      user_input_2 = 0;
    }
  }
}

void readckhd(FIL *fid, struct ckhd *hd, uint32_t ckID) {
  f_read(fid, hd, sizeof(struct ckhd), &ret);
  if (ret != sizeof(struct ckhd))
    exit(-1);
  if (ckID && (ckID != hd->ckID))
    exit(-1);
}

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}

void playAudio() {
  FRESULT rc;     /* Result code */
  UINT bw, br;
  unsigned int retval;
  int bytesread;
  
  f_mount(0, &Fatfs);/* Register volume work area */

  rc = f_open(&fid, "Jungle.wav", FA_READ);
  
  if (!rc) {
    struct ckhd hd;
    uint32_t  waveid;
    struct fmtck fck;
    
    readckhd(&fid, &hd, 'FFIR');
    
    f_read(&fid, &waveid, sizeof(waveid), &ret);
    if ((ret != sizeof(waveid)) || (waveid != 'EVAW'))
      return -1;
    
    readckhd(&fid, &hd, ' tmf');
    
    f_read(&fid, &fck, sizeof(fck), &ret);
    
    // skip over extra info
    
    if (hd.cksize != 16) {
      f_lseek(&fid, hd.cksize - 16);
    }
    
    // now skip all non-data chunks !
    
    while(1){
      readckhd(&fid, &hd, 0);
      if (hd.ckID == 'atad')
  break;
      f_lseek(&fid, hd.cksize);
    }
    
    f_read(&fid, Audiobuf, AUDIOBUFSIZE, &ret);
    hd.cksize -= ret;
    audioplayerStart();
    //audioplayerStop(); 
  }

  printf("\nClose the file.\n"); 
  rc = f_close(&fid);
  
  if (rc) die(rc);
}

/* Given an image name display image to LCD */
void displayImg(char *img) {
  rc = f_open(&Fil, img, FA_READ);
  if (rc) die(rc);

  char header[54];
  rc = f_read(&Fil, header, sizeof header, &br);

  int j, k;
  for (k = 0; k < 160; k++) {
    rc = f_read(&Fil, Buff, sizeof Buff, &br); /* Read a chunk of file */
    if (rc || !br) break;     /* Error or end of file */
    uint16_t pixels[128];

    for (j = 0; j < 128 * 3; j = j + 3) {
      uint16_t r, g, b; 
      r = (Buff[j+2] >> 3) << 11;
      g = (Buff[j+1] >> 2) << 5;
      b = Buff[j] >> 3;
      pixels[j/3] = r | g | b;
    } 

    f3d_lcd_setAddrWindow (0,k,127,k,MADCTLGRAPHICS);
    f3d_lcd_pushColor(pixels,128);
  }
  if (rc) die(rc);
  
  rc = f_close(&Fil);
  if (rc) die(rc);
}



/* Handle joystick input */
void pollEvent() {
  f3d_nunchuk_read(&j);
  jx = (unsigned char)j.jx;
  jy = (unsigned char)j.jy;
  c = (unsigned char)j.c;
  z = (unsigned char)j.z;
  float jxa = (float) (jx - 126) / 2;
  float jya = (float) (jy - 126) / 2;

  /* Handle joystick inputs */
  if(jxa > 60) { 
    user_input = 2;
  } else if(jxa < -60) {
    user_input = 4;
  } else if(jya > 60) {
    user_input = 1;
  } else if(jya < -60) {
    user_input = 3;
  } else {
    user_input = 0;
  }

  /* Handle Z & C inputs */
  if(z || c) {
    if(!btn_down_2) {
      if(c) {
        user_input_2 = 5;
      } else if(z) {
        user_input_2 = 6;
      }
      btn_down_2 = 1;
    }
  } else {
    btn_down_2 = 0;
  } 
} 


/* Main menu */
void menu(void) {
  /* Set up menu text */
  displayImg("title_17.bmp");
  lives = LIVES;
  score = 0;

  /* While not in game, wait for S to start game or Q to quit */
  while(!in_game) {
    pollEvent();
    if(user_input_2 == 5) {
      in_game = 1;
      user_input_2 = 0;

      /* Set up scores */
      f3d_lcd_fillScreen(BLACK);
      drawTarget(TARGET_X, TARGET_Y, TARGET_WIDTH, TARGET_WIDTH, CYAN);
      f3d_lcd_drawString(SCORE_X, SCORE_Y, "Score: ", WHITE, BLACK);
      initArrow(&arrows[0], ST7735_width - 20, TARGET_Y, 6, 6, WHITE, 100);
      initArrow(&arrows[1], ST7735_width - 20, TARGET_Y, 6, 6, WHITE, 1);
      initArrow(&arrows[2], ST7735_width - 20, TARGET_Y, 6, 6, WHITE, 100);
      initArrow(&arrows[3], ST7735_width - 20, TARGET_Y, 6, 6, WHITE, 100);
      initArrow(&chars[0], TARGET_X_CENTER, ST7735_height - 10, 6, 6, WHITE, 100);
      initArrow(&chars[1], TARGET_X_CENTER, ST7735_height - 10, 6, 6, WHITE, 5);
      int i;
      for(i = 0; i < 20; i++) {
        initRect(&stars[i], (int)rand() % ST7735_width, (int)rand() % ST7735_height, 1, 1, WHITE);
      }
      current_arrow = arrows[1];
      current_char = chars[1];
      playAudio();
    }
  }
}

/* Where the collisions are handled */
void game_loop(void) {
  int collision, charCollision;

  /* Start event loop for keyboard movement */
  pollEvent();
  drawTarget(TARGET_X, TARGET_Y, TARGET_WIDTH, TARGET_WIDTH, colors[(int)rand() % 8]);
  if(score >= score_thr) {
    arrow_vy -= !(score_thr % 500) ? 1 : 0; 
    score_thr += 250;
    arrow_vx -= 1;
  } 

  /* Initialize stars */
  int i;
  for(i = 0; i < 20; i++) {
    if(stars[i].pos_x < 3) {
      eraseRect(&stars[i], BLACK);
      initRect(&stars[i], (int)rand() % ST7735_width, (int)rand() % ST7735_height, 1, 1, WHITE);
    } else {
      moveRect(&stars[i], arrow_vx, 0, BLACK);    
    }
  }

  /* Update scores */
  sprintf(score_string,"Score: %d", score);
  f3d_lcd_drawString(SCORE_X, SCORE_Y, score_string, WHITE, BLACK);

  /* Update scores */
  sprintf(score_string,"Lives: %d", lives);
  f3d_lcd_drawString(SCORE_X, ST7735_height - SCORE_Y, score_string, WHITE, BLACK);

  /* Handle arrow collisions */
  if(current_arrow.direction  != 100) {
    collision = moveArrow(&current_arrow, arrow_vx, 0, BLACK);
    switch(collision) {
      case EXCELLENT:
        if(current_arrow.direction == user_input && current_arrow.color == WHITE) {
          score += EXC_SCORE;
          current_arrow.color = GREEN;
          user_input = 0;
        } 
        break;
      case GREAT:
        if(current_arrow.direction == user_input && current_arrow.color == WHITE) {
         score += GREAT_SCORE;
         current_arrow.color = YELLOW;
         user_input = 0;
        }
        break;
      case GOOD:
        if(current_arrow.direction == user_input && current_arrow.color == WHITE) {
          score += GOOD_SCORE;
          current_arrow.color = CYAN;
          user_input = 0;
        }
        break;
      case BAD:
        if(user_input != 0 && current_arrow.color == WHITE) {
          score -= (score >= GREAT_SCORE) ? GREAT_SCORE : 0;
          sprintf(score_string,"Score: %d", score);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, "                                           ", WHITE, BLACK);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, score_string, WHITE, BLACK);
          lives = lives - 1;
          current_arrow.color = RED;
          user_input = 0;
        }
        break;
      case MISS:
        if(current_arrow.color == WHITE) {
          score -= (score >= GREAT_SCORE) ? GREAT_SCORE : 0;
          sprintf(score_string,"Score: %d", score);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, "                                           ", WHITE, BLACK);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, score_string, WHITE, BLACK);
          lives = lives - 1;
          current_arrow.color = RED;
          user_input = 0;
        }
        break;
      default:
        user_input = 0;
        break;
    }
  } else {
      current_arrow = arrows[(int)rand() % 4 ];
      if(current_arrow.direction == 100) {
        current_arrow.direction = (int)rand() % 4 + 1;
        current_arrow.color = WHITE;
      }
  }

  /* Handle character collisions */
  if(current_char.direction  != 100) {
    charCollision = moveArrow(&current_char, 0, arrow_vy, BLACK);
    switch(charCollision) {
      case EXCELLENT:
        if(current_char.direction == user_input_2 && btn_down_2 && current_char.color == WHITE) {
          score += EXC_SCORE;
          current_char.color = GREEN;
          user_input_2 = 0;
        } 
        break;
      case GREAT:
        if(current_char.direction == user_input_2 && btn_down_2 && current_char.color == WHITE) {
         score += GREAT_SCORE;
         current_char.color = YELLOW;
         user_input_2 = 0;
        }
        break;
      case GOOD:
        if(current_char.direction == user_input_2 && btn_down_2 && current_char.color == WHITE) {
          score += GOOD_SCORE;
          current_char.color = CYAN;
          user_input_2 = 0;
        }
        break;
      case MISS:
        if(current_char.color == WHITE) {
          score -= (score >= GREAT_SCORE) ? GREAT_SCORE : 0;
          sprintf(score_string,"Score: %d", score);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, "                                           ", WHITE, BLACK);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, score_string, WHITE, BLACK);
          lives = lives - 1;
          current_char.color = RED;
          user_input_2 = 0;
        }
        break;
      case BAD:
        if(user_input_2 != 0 && btn_down_2 && current_char.color == WHITE) {
          score -= (score >= GREAT_SCORE) ? GREAT_SCORE : 0;
          sprintf(score_string,"Score: %d", score);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, "                                           ", WHITE, BLACK);
          f3d_lcd_drawString(SCORE_X, SCORE_Y, score_string, WHITE, BLACK);
          lives = lives - 1;
          current_char.color = RED;
          user_input_2 = 0;
        }
        break;
      default:
        break;
    }
  } else {
      current_char = chars[(int)rand() % 2];
      if(current_char.direction == 100) {
        current_char.direction = (int)rand() % 2 + 5;
        current_char.color = WHITE;
      }
  }

  /* Game over if lives = 0 */
  if(lives <= 0) {
    in_game = 2;
    audioplayerStop(); 
  }
}

/* Where the game_loop() is called the rectangles are initialized. */
int main(void) {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  f3d_uart_init();
  f3d_timer2_init();
  f3d_dac_init();
  f3d_delay_init();
  f3d_rtc_init();
  f3d_systick_init();
  f3d_lcd_init();
  f3d_i2c1_init();
  f3d_nunchuk_init();  

  f_mount(0, &Fatfs);   /* Register volume work area (never fails) */

  while (1) {
    if(in_game == 1) {
      game_loop();
      delay(15);
    } else if(in_game == 2) {
      gameOver();
      delay(15);
    } else {
      menu();
    }
  } 
} 

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while (1);
}
#endif
/* pong.c ends here */
