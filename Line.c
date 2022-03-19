#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <stdint.h>
#include <math.h>

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define SET_PIXEL(x,y,color)      screen_buf[(y)*SCREEN_WIDTH + (x)] = color
#define SIGN(x)                   (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))

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

#define degToRad(degree)    ((degree) * M_PI / 180.0)
#define RAD_15              (M_PI/12)
#define RAD_30              (M_PI/6)
#define RAD_45              (M_PI/4)
#define RAD_60              (M_PI/3)
#define RAD_90              (M_PI/2)
#define RAD_120             (M_PI/1.5)
#define RAD_240             (1.5*M_PI)
#define RAD_360             (2*M_PI)
 
#define X_AXIS              1
#define Y_AXIS              2

#define SCREEN_SIZE         64000
#define TILE_WIDTH          8
#define TILE_HEIGHT         8
#define CHARACTER_SIZE      72

int running = 1;
uint8_t alphabet [4240];

typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    int x, y;
} Vec2_int;

typedef struct
{
    Vec2 startpos;
    Vec2 endpos;
    uint8_t colour;
} Line;

typedef struct
{
    float angle;
    Vec2* vectors;
    int numVectors;
    float radius;
    uint8_t colour;
} Polygon;

Line line_array[5] = {
    {{40.0,  50.0}, {20.0, 40.0}, 41},
    {{100.0, 20.0}, {180.0, 30.0}, 55},
    {{220.0, 100.0}, {220.0, 180.0}, 41},
    {{60.0,  50.0}, {120.0, 50.0}, 55},
    {{240.0, 100.0}, {240.0, 105.0}, 41}
};

Polygon poly_array[10];
Polygon makePolygon(float angle, int numVectors, float radius, uint8_t colour);

struct Input
{
    uint8_t kb_array[KB_ARRAY_LENGTH];
    uint8_t kb_queue[KB_QUEUE_LENGTH];
    uint8_t kb_head;
    uint8_t kb_tail;
};

typedef struct Input Input_t;

uint8_t *VGA=(uint8_t *)0xA0000000L;        /* this points to video memory. */
uint8_t far screen_buf [64000];

Input_t Input = {0};
Input_t* g_Input = &Input;
uint8_t* g_Keyboard = Input.kb_array;

static void interrupt (far *old_keyhandler)(void);

void set_mode(uint8_t mode)
{
    union REGS regs;

    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    int86(VIDEO_INT, &regs, &regs);
}

Vec2 change_vec_angle(Vec2 vector, float angle)
{
    Vec2 x_vec;
    Vec2 y_vec;
    Vec2 newVector;
    
    x_vec.x = cos(angle) * vector.x;
    x_vec.y = sin(angle) * vector.x;
    y_vec.x = cos(angle) * vector.y;
    y_vec.y = sin(angle) * vector.y;
    
    newVector.x = x_vec.x - y_vec.y;
    newVector.y = x_vec.y + y_vec.x;
    
    return newVector;
}

Vec2 change_vec_scale(Vec2 vector, float scale)
{
    Vec2 newVector;
    
    newVector.x = vector.x * scale;
    newVector.y = vector.y * scale;
    
    return newVector;
}

void change_poly_angles(Polygon* poly, float angle)
{
    char i;
    
    for (i = 0; i < poly->numVectors; i++)
    {
        poly->vectors[i] = change_vec_angle(poly->vectors[i], angle);
    }
}

void change_poly_size(Polygon* poly, float scale)
{
    char i;
    
    for (i = 0; i < poly->numVectors; i++)
    {
        poly->vectors[i] = change_vec_scale(poly->vectors[i], scale);
    }
}

void interrupt far keyhandler()
{
    uint8_t key_scan = 0;
    static uint8_t key_scan_last = 0;
    static uint8_t key_special = 0;
    static uint8_t key_special_last = 0;
    // obtain scancode
    asm {
        cli // clear interrupt flag; prevent other external interrupts
        in  al, 0x60
        mov key_scan, al
        in  al, 0x61 // get keyboard status
        mov bl, al
        or  al, 0x80 // MSB set
        out 0x61, al // write (ACK)
        mov al, bl   // MSB clear
        out 0x61, al // write
    }
    // remember the last special key code
    if (key_scan_last == KEY_SPECIAL_CODE)
        key_special = key_scan;
    else if (key_scan != KEY_SPECIAL_CODE)
        key_special = 0;
    // place scancode on queue unless it's the same as the previous one
    if (key_scan != key_scan_last && (key_special != key_special_last || key_special_last == 0))
    {
        g_Input->kb_queue[g_Input->kb_tail] = key_scan;
        if (g_Input->kb_tail+1 != g_Input->kb_head)
            g_Input->kb_tail++;
    }
    key_scan_last = key_scan;
    key_special_last = key_special;
    // PIC 8259 reset
    asm {
        mov al, 0x20
        out 0x20, al
        sti // set interrupt flag; allow external interrupts again
    }
}

void init_keyboard()
{
    uint8_t far *bios_key_state;
    asm cli
    // save address of current keyhandler interrupt function
    old_keyhandler = _dos_getvect(9);
    // caps lock & num lock off
    bios_key_state = MK_FP(0x040, 0x017);
    *bios_key_state &= (~(32|64));
    old_keyhandler(); 
    // replace old keyhandler with new keyhandler function
    _dos_setvect(9, keyhandler);
    asm sti
}

void deinit_keyboard()
{
    // restore old keyhandler
    asm cli
    _dos_setvect(9, old_keyhandler);
    asm sti

    g_Input = NULL;
}

void get_keyboard()
{
    int i;
    uint8_t key_scan;
    uint8_t extended = 0;
    uint8_t released = 0;

    while (g_Input->kb_head != g_Input->kb_tail)
    {
        key_scan = g_Input->kb_queue[g_Input->kb_head++];
        /* handle special keys, but only if a second scancode follows*/
        if (key_scan == KEY_SPECIAL_CODE)
                extended = 128;
        /* remember the release flag before clearing it*/
        released = key_scan & KEY_RELEASED_FLAG;
        key_scan &= ~KEY_RELEASED_FLAG;
        if (released)
        {
            g_Input->kb_array[key_scan+extended] &= KEY_HIT_FLAG;
            g_Input->kb_array[key_scan+extended] |= KEY_RELEASED_FLAG;
        }
        else
            g_Input->kb_array[key_scan+extended] |= (KEY_HIT_FLAG|KEY_PRESSED_FLAG);

        extended = 0;
    }
}

void clear_keys()
{
    int i;
    for (i = 0; i < KB_ARRAY_LENGTH; i++)
        g_Input->kb_array[i] &= KEY_PRESSED_FLAG;
}

void control_ingame()
{
    if (KEY_IS_PRESSED(KEY_UP))
    {
        poly_array[1].vectors[2] = change_vec_angle(poly_array[1].vectors[2], RAD_15);
    }
    
    else if (KEY_IS_PRESSED(KEY_DOWN))
    {
        poly_array[1].vectors[2] = change_vec_angle(poly_array[1].vectors[2], -RAD_15);
    }
    
    else if (KEY_IS_PRESSED(KEY_LEFT))
    {
        change_poly_angles(&poly_array[1], -RAD_15);
    }
    
    else if (KEY_IS_PRESSED(KEY_RIGHT))
    {
        change_poly_angles(&poly_array[1], RAD_15);
    }
    
    else if (KEY_IS_PRESSED(KEY_W))
    {
        change_poly_size(&poly_array[1], 1.1);
    }
    
    else if (KEY_IS_PRESSED(KEY_Q))
    {
        change_poly_size(&poly_array[1], 0.9);
    }
}

void process_input()
{
    get_keyboard();

    control_ingame();
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        running = 0;

    clear_keys();
}

void load_font()
{
    FILE* file_ptr;
    file_ptr = fopen("FONT.7UP", "rb");
    fread(alphabet, 1, 4240, file_ptr);
    fclose(file_ptr);
}

void draw_text(int x, int y, int i, uint8_t color)
{
    uint8_t index_x = 0;
    uint8_t index_y = 0;
    i = i * CHARACTER_SIZE;

    for (index_y=0;index_y<TILE_HEIGHT;index_y++)
    {
        for (index_x=0;index_x<TILE_WIDTH;index_x++)
        {
            if (alphabet[i] != 13)
            {
                SET_PIXEL(x, y, alphabet[i] + color);
                i++;
                x++;
            }
            else
            {
                i++;
                x++;
            }
        }
        index_x = 0;
        x = x - TILE_WIDTH;
        y++;
    }
    index_y = 0;
    i= 0;
}

void render_text(int x, int y, char* string, uint8_t color)
{
    int i = 0;
    char c;
    
    while (string[i] != 0)
    {
        c = string[i];
        draw_text(x, y, c - 32, color);
        x = x + 10;
        i++;
    }
}

void draw_line_hor(int start_x, int start_y, int x_diff, uint8_t colour)
{
    int offset_x;
    
    if (x_diff < 0)
    {
        start_x += x_diff;
        x_diff = abs(x_diff);
    }
    
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(start_x + offset_x, start_y, colour);
    }
}

void draw_line_ver(int start_x, int start_y, int y_diff, uint8_t colour)
{
    int offset_y;
    
    if (y_diff < 0)
    {
        start_y += y_diff;
        y_diff = abs(y_diff);
    }
    
    for (offset_y = 0; offset_y < y_diff + 1; offset_y++)
    {
        SET_PIXEL(start_x, start_y + offset_y, colour);
    }
}

void draw_line(Vec2 v1, Vec2 v2, uint8_t colour)
{
    float offset_x;
    float offset_y;
    
    float x_diff = (v2.x - v1.x);
    float y_diff = (v2.y - v1.y);
    
    int x_sign = SIGN(x_diff);
    int y_sign = SIGN(y_diff);
    
    int x_pixel;
    int y_pixel;
    
    float slope = 0.0;
    
    if (fabs(y_diff) < 1.5)
        draw_line_hor(v1.x, v1.y, x_diff, colour);
    
    else if (fabs(x_diff) < 1.5)
        draw_line_ver(v1.x, v1.y, y_diff, colour);
    
    else if (y_diff < 0 && x_diff < 0)
    {
        if (y_diff < x_diff)
        {
            slope = y_diff / x_diff;
            
            for (offset_x = 0; offset_x > x_diff; offset_x += x_sign)
            {
                offset_y = offset_x * slope;
                offset_x += 0.75;
                offset_y += 0.75;
                SET_PIXEL((int)v2.x - (int)offset_x, (int)v2.y - (int)offset_y, colour);
            }
        }
        else
        {
            slope = x_diff / y_diff;
            
            for (offset_y = 0; offset_y > y_diff; offset_y += y_sign)
            {
                offset_x = offset_y * slope;
                offset_x -= 0.75;
                offset_y += 0.75;
                SET_PIXEL((int)v2.x - (int)offset_x, (int)v2.y - (int)offset_y, colour);
            }
        }
    }
    
    else if (y_diff < 0 || x_diff < 0)
    {
        if (y_diff < x_diff)
        {
            slope = y_diff / x_diff;
            
            for (offset_x = 0; offset_x < x_diff; offset_x += x_sign)
            {
                offset_y = offset_x * slope;
                offset_x -= 0.6;
                offset_y -= 0.6;
                SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, colour);
            }
        }

        else
        {
            slope = x_diff / y_diff;
            
            for (offset_y = 0; offset_y < y_diff; offset_y += y_sign)
            {
                offset_x = offset_y * slope;
                offset_x -= 0.6;
                offset_y -= 0.6;
                SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, colour);
            }
        }
    }
    
    else if (y_diff < x_diff)
    {
        slope = y_diff / x_diff;
        
        for (offset_x = 0; offset_x < x_diff; offset_x += x_sign)
        {
            offset_y = offset_x * slope;
            SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, colour);
        }
    }
    
    else
    {
        slope = x_diff / y_diff;
        
        for (offset_y = 0; offset_y < y_diff; offset_y += y_sign)
        {
            offset_x = offset_y * slope;
            SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, colour);
        }
    }
}

Polygon makeSquare(float angle, float side_length, uint8_t colour)
{
    Polygon newSquare;
    
    newSquare.numVectors = 4;
    newSquare.vectors = calloc(4, sizeof(Vec2));
    newSquare.angle = angle;
    newSquare.colour = colour;
    
    newSquare.vectors[0].x = -side_length/2.0;
    newSquare.vectors[0].y = -side_length/2.0;
    newSquare.vectors[1].x = side_length/2.0;
    newSquare.vectors[1].y = -side_length/2.0;
    newSquare.vectors[2].x = side_length/2.0;
    newSquare.vectors[2].y = side_length/2.0;
    newSquare.vectors[3].x = -side_length/2.0;
    newSquare.vectors[3].y = side_length/2.0;
    
    return newSquare;
}

Polygon makePolygon(float angle, int numVectors, float radius, uint8_t colour)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numVectors;
    
    newPolygon.numVectors = numVectors;
    newPolygon.vectors = malloc(numVectors * sizeof(Vec2));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.colour = colour;
    
    while (i < newPolygon.numVectors)
    {
        newPolygon.vectors[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.vectors[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    return newPolygon;
}

void draw_polygon(Polygon* poly, int center_x, int center_y)
{
    char i = 0;
    Vec2 start_loc;
    Vec2 end_loc;
    
    while (i < poly->numVectors - 1)
    {
        start_loc.x = center_x + poly->vectors[i].x;
        start_loc.y = center_y + poly->vectors[i].y;
        
        end_loc.x = center_x + poly->vectors[i + 1].x;
        end_loc.y = center_y + poly->vectors[i + 1].y;

        draw_line(start_loc, end_loc, poly->colour);
        
        i++;
    }
    
    start_loc.x = center_x + poly->vectors[i].x;
    start_loc.y = center_y + poly->vectors[i].y;
    
    end_loc.x = center_x + poly->vectors[0].x;
    end_loc.y = center_y + poly->vectors[0].y;
    
    draw_line(start_loc, end_loc, poly->colour);
}

void draw_lines()
{
    char i = 0;
    
    while (i < 5)
    {
        draw_line(line_array[i].startpos, line_array[i].endpos, line_array[i].colour);
        i++;
    }
}

void draw_polygons()
{
    char i = 0;
    int x = 100;
    int y = 100;
    
    while (i < 3)
    {
        draw_polygon(&poly_array[i], x, y);
        x += 45;
        i++;
    }
}

void draw_stuff()
{
    draw_lines();
    draw_polygons();
}

void render()
{     
    // copy off-screen buffer to VGA memory
    memcpy(VGA,screen_buf,SCREEN_SIZE);

    // clear off-screen buffer so the screen updates properly
    _fmemset(screen_buf, 0, SCREEN_SIZE);
}

void quit()
{   
    deinit_keyboard();
    set_mode(TEXT_MODE);
}

void main()
{   
    Polygon Square = makeSquare(0.0, 10.0, 44);
    Polygon Triangle = makePolygon(0.0, 3, 15.0, 47);
    Polygon Pentagon = makePolygon(0.0, 5, 25.0, 47);
    
    poly_array[0] = Square;
    poly_array[1] = Triangle;
    poly_array[2] = Pentagon;

    load_font();
    set_mode(VGA_256_COLOR_MODE);
    init_keyboard();
    
    while (running == 1)
    {
        process_input();
        draw_stuff();
        render();
        delay(50);
    }
    quit();
}