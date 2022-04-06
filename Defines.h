#ifndef DEFINES_H
#define DEFINES_H

/* Graphics defines */
#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define PALETTE_WRITE       0x03C8
#define PALETTE_DATA        0x03C9

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define SCREEN_SIZE         64000
#define TRANSPARENT_COLOR   251

#define TILE_WIDTH          8
#define TILE_HEIGHT         8
#define CHARACTER_SIZE      72

/* Keyboard and control defines */
#define KB_ARRAY_LENGTH     256
#define KB_QUEUE_LENGTH     256

#define KEY_HIT_FLAG        1
#define KEY_PRESSED_FLAG    2
#define KEY_RELEASED_FLAG   0x80 // 128, MSB
#define KEY_SPECIAL_CODE    0xE0 // 224
#define KEY_IS_PRESSED(k)   (g_Keyboard[k] & KEY_PRESSED_FLAG)
#define KEY_WAS_HIT(k)      (g_Keyboard[k] & KEY_HIT_FLAG)
#define KEY_WAS_RELEASED(k) (g_Keyboard[k] & KEY_RELEASED_FLAG)
#define KEY_WAS_TOGGLED(k)  (g_Keyboard[k] & (KEY_HIT_FLAG|KEY_RELEASED_FLAG))

#define KEY_LEFT            75
#define KEY_RIGHT           77
#define KEY_UP              72 
#define KEY_DOWN            80
#define KEY_F10             68
#define KEY_ADD             78
#define KEY_SUB             74
#define KEY_Q               16
#define KEY_W               17
#define KEY_E               18
#define KEY_R               19
#define KEY_1               2
#define KEY_2               3
#define KEY_3               4
#define KEY_4               5
#define KEY_5               6
#define KEY_6               7

#define X_AXIS              1
#define Y_AXIS              2

#define TURN_RATE           (M_PI/50)

/* Math defines */
#define degToRad(degree)    ((degree) * M_PI / 180.0)
#define RAD_15              (M_PI/12)
#define RAD_30              (M_PI/6)
#define RAD_45              (M_PI/4)
#define RAD_60              (M_PI/3)
#define RAD_90              (M_PI/2)
#define RAD_135             (M_PI/1.5)
#define RAD_180             (M_PI)
#define RAD_240             (1.5*M_PI)
#define RAD_360             (2*M_PI)

/* Macros */
#define SIGN(x)                   (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define SET_PIXEL(x,y,color)      screen_buf[(y)*SCREEN_WIDTH + (x)] = color
#define RGB(color)                (color.r, color.g, color.b)

/* Misc / general defines */
#define TRUE 1
#define FALSE 0

#define modObject           mesh_array[0]

#endif
