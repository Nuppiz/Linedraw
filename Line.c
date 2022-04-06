#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <stdint.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */
#define TRANSPARENT_COLOR   251

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
#define KEY_E               18
#define KEY_R               19
#define KEY_1               2
#define KEY_2               3
#define KEY_3               4
#define KEY_4               5
#define KEY_5               6
#define KEY_6               7

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

#define TURN_RATE           (M_PI/50)
 
#define X_AXIS              1
#define Y_AXIS              2
#define modObject           mesh_array[0]

#define SCREEN_SIZE         64000
#define TILE_WIDTH          8
#define TILE_HEIGHT         8
#define CHARACTER_SIZE      72
#define RGB(color)          (color.r, color.g, color.b)
#define PALETTE_WRITE       0x03C8
#define PALETTE_DATA        0x03C9

uint8_t *VGA=(uint8_t *)0xA0000000L;        /* this points to video memory. */
uint8_t far screen_buf [64000];

int running = 1;
uint8_t alphabet [4240];
uint8_t tile [10000];
uint8_t car [2048];

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
    uint8_t color;
} Line;

typedef struct
{
    Vec2_int startpos;
    Vec2_int endpos;
    uint8_t color;
} Line_int;

typedef struct
{
    float angle;
    Vec2* points;
    Vec2* transformedV;
    int numPoints;
    float radius;
    float scale;
    uint8_t color;
    uint8_t* vertexColors;
    Vec2* UVCoords;
    Vec2_int center;
} Polygon;

typedef struct
{
    int pointID;
    Vec2 UV;
} Vertex;

typedef struct
{
    uint8_t* pixels;
    int height;
    int width;
    uint8_t transparent;
} Texture;

typedef struct
{
    float angle;
    float scale;
    int numPoints;
    Vec2* points;
    Vec2* transformedP;
    int numTriangles;
    Vertex* triangleVertices;
    Vec2_int center;
    Texture* texture;
} Mesh2D;

typedef struct
{
    uint8_t r,g,b;
} Color;

typedef struct
{
    Color colors[256];
} Palette;

Line line_array[5] = {
    {{40.0,  50.0}, {20.0, 40.0}, 41},
    {{100.0, 20.0}, {180.0, 30.0}, 55},
    {{220.0, 100.0}, {220.0, 180.0}, 41},
    {{60.0,  50.0}, {120.0, 50.0}, 55},
    {{240.0, 100.0}, {240.0, 105.0}, 41}
};

Line_int intline_array[2] = {
    {{210, 10}, {260, 20}, 55},
    {{220, 10}, {230, 60}, 41}
};

Polygon poly_array[10];
Mesh2D mesh_array[10];

typedef struct
{
    int offset[SCREEN_HEIGHT];
    uint8_t color[SCREEN_HEIGHT];
    Vec2 texture[SCREEN_HEIGHT];
} Span;

Span LeftEdge;
Span RightEdge;

Palette NewPalette;

Texture CarTex;
Mesh2D CarMesh;

struct Input
{
    uint8_t kb_array[KB_ARRAY_LENGTH];
    uint8_t kb_queue[KB_QUEUE_LENGTH];
    uint8_t kb_head;
    uint8_t kb_tail;
};

typedef struct Input Input_t;

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

// loads the palette from a 256-colour bitmap file
void loadPalette(char* filename, Palette* pal)
{
    FILE *fp;
    int i, x;
    
    // Open the file
    fp = fopen(filename, "rb");
    
    // skip forward and read the number of colours
    fseek(fp, 0x0036, SEEK_SET);
    
    // load palette
    for (i = 0; i < 256; i++)
    {
        uint8_t r,g,b;
        // VGA palette values only go up to 6 bits, (2 ^ 6 == 64 different shades)
        // while bitmap palettes go up to 8 bits, (2 ^ 8 == 256 different shades)
        // right shift (>>) the bits by two places so an 8-bit value becomes a 6-bit
        // this divides the shade value (0-255) by four, giving a value between 0-63
  
        pal->colors[i].b = fgetc(fp);
        pal->colors[i].g = fgetc(fp);
        pal->colors[i].r = fgetc(fp);

        x = fgetc(fp); // discarded  value
    }

    fclose(fp);
}

void setPalette_VGA(Palette* pal)
{
    unsigned i;
    outportb(PALETTE_WRITE, 0);

    for (i = 0; i < 256; i++)
    {
       outportb(PALETTE_DATA, (pal->colors[i].r>>2));
       outportb(PALETTE_DATA, (pal->colors[i].g>>2));
       outportb(PALETTE_DATA, (pal->colors[i].b>>2));
    }
}

void load_gfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void load_texture(char* filename, Texture* texture, int width, int height, int transparent)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    texture->pixels = malloc(width * height);
    fread(texture->pixels, 1, width * height, file_ptr);
    texture->height = height;
    texture->width = width;
    texture->transparent = transparent;
    fclose(file_ptr);
}

void draw_sprite(int x, int y, Texture* texture)
{
    int pix_x = x;
    int pix_y = y;
    int index_x = 0;
    int index_y = 0;
    int i = 0;

    if (texture->transparent == TRUE)
    {
        for (index_y=0;index_y<texture->height;index_y++)
        {
            for (index_x=0;index_x<texture->width;index_x++)
            {
                if (texture->pixels[i] != TRANSPARENT_COLOR)
                {
                    SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                    i++;
                    pix_x++;
                }
                else
                {
                    i++;
                    pix_x++;
                }
            }
            index_x = 0;
            pix_x = x;
            pix_y++;
        }
    }
    else
    {
        for (index_y=0;index_y<texture->height;index_y++)
        {
            for (index_x=0;index_x<texture->width;index_x++)
            {
                SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                i++;
                pix_x++;
            }
            index_x = 0;
            pix_x = x;
            pix_y++;
        }
    }
}

void MakeCarMesh(Mesh2D* carmesh)
{
    carmesh->angle = 0.0;
    carmesh->scale = 1.0;
    carmesh->center.x = 159;
    carmesh->center.y = 99;

    carmesh->numPoints = 4;
    carmesh->points = malloc(carmesh->numPoints * sizeof(Vec2));
    carmesh->points[0].x = -32;
    carmesh->points[0].y = -64;
    carmesh->points[1].x = 32;
    carmesh->points[1].y = -64;
    carmesh->points[2].x = 32;
    carmesh->points[2].y = 64;
    carmesh->points[3].x = -32;
    carmesh->points[3].y = 64;

    carmesh->transformedP = malloc(carmesh->numPoints * sizeof(Vec2));
    memcpy(carmesh->transformedP, carmesh->points, sizeof(Vec2) * carmesh->numPoints);

    carmesh->texture = &CarTex;
    carmesh->numTriangles = 2;

    carmesh->triangleVertices = malloc((carmesh->numTriangles * 3) * sizeof(Vertex));
    carmesh->triangleVertices[0].pointID = 0;
    carmesh->triangleVertices[0].UV.x = 0;
    carmesh->triangleVertices[0].UV.y = 0;
    carmesh->triangleVertices[1].pointID = 1;
    carmesh->triangleVertices[1].UV.x = 32;
    carmesh->triangleVertices[1].UV.y = 0;
    carmesh->triangleVertices[2].pointID = 3;
    carmesh->triangleVertices[2].UV.x = 0;
    carmesh->triangleVertices[2].UV.y = 64;
    carmesh->triangleVertices[3].pointID = 1;
    carmesh->triangleVertices[3].UV.x = 32;
    carmesh->triangleVertices[3].UV.y = 0;
    carmesh->triangleVertices[4].pointID = 2;
    carmesh->triangleVertices[4].UV.x = 32;
    carmesh->triangleVertices[4].UV.y = 64;
    carmesh->triangleVertices[5].pointID = 3;
    carmesh->triangleVertices[5].UV.x = 0;
    carmesh->triangleVertices[5].UV.y = 64;
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

/*Vec2 change_vec_scale(Vec2 vector, float scale)
{
    Vec2 newVector;
    
    newVector.x = vector.x * scale;
    newVector.y = vector.y * scale;
    
    return newVector;
}
void change_poly_angles(Polygon* poly, float angle)
{
    char i;
    
    for (i = 0; i < poly->numPoints; i++)
    {
        poly->transformedV[i] = change_vec_angle(poly->transformedV[i], angle);
    }
    poly->angle = angle;
}
void change_poly_size(Polygon* poly, float scale)
{
    char i;
    
    for (i = 0; i < poly->numPoints; i++)
    {
        poly->transformedV[i] = change_vec_scale(poly->transformedV[i], scale);
    }
}*/

void updatePoly(Polygon* poly)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_y;
    float new_x;
    float new_y;
    
    for (i = 0; i < poly->numPoints; i++)
    {
        old_x = poly->points[i].x;
        old_y = poly->points[i].y;
        cos_angle = cos(poly->angle);
        sin_angle = sin(poly->angle);
        new_x = (old_x * cos_angle - old_y * sin_angle) * poly->scale;
        new_y = (old_x * sin_angle + old_y * cos_angle) * poly->scale;
        poly->transformedV[i].x = new_x;
        poly->transformedV[i].y = new_y;
    }
}

void updateMesh(Mesh2D* mesh)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_y;
    float new_x;
    float new_y;
    
    for (i = 0; i < mesh->numPoints; i++)
    {
        old_x = mesh->points[i].x;
        old_y = mesh->points[i].y;
        cos_angle = cos(mesh->angle);
        sin_angle = sin(mesh->angle);
        new_x = (old_x * cos_angle - old_y * sin_angle) * mesh->scale;
        new_y = (old_x * sin_angle + old_y * cos_angle) * mesh->scale;
        mesh->transformedP[i].x = new_x;
        mesh->transformedP[i].y = new_y;
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
        poly_array[4].transformedV[2] = change_vec_angle(poly_array[1].transformedV[2], RAD_15);
    }
    
    else if (KEY_IS_PRESSED(KEY_DOWN))
    {
        poly_array[4].transformedV[2] = change_vec_angle(poly_array[1].transformedV[2], -RAD_15);
    }
    
    else if (KEY_IS_PRESSED(KEY_LEFT))
    {
        modObject.angle -= TURN_RATE;
        //poly_array[7].angle -= RAD_15;
        updateMesh(&modObject);
        //updatePoly(&poly_array[7]);
    }
    
    else if (KEY_IS_PRESSED(KEY_RIGHT))
    {
        modObject.angle += TURN_RATE;
        //poly_array[7].angle += RAD_15;
        updateMesh(&modObject);
        //updatePoly(&poly_array[7]);
    }
    
    else if (KEY_IS_PRESSED(KEY_ADD))
    {
        modObject.scale *= 1.05;
        poly_array[5].scale *= 1.05;
        updateMesh(&modObject);
        //updatePoly(&poly_array[7]);
    }
    
    else if (KEY_IS_PRESSED(KEY_SUB))
    {
        modObject.scale /= 1.05;
        poly_array[5].scale /= 1.05;
        updateMesh(&modObject);
        //updatePoly(&poly_array[7]);
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

void draw_line_hor(int start_x, int start_y, int x_diff, uint8_t color)
{
    int offset_x;
    
    if (x_diff < 0)
    {
        start_x += x_diff;
        x_diff = abs(x_diff);
    }
    
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(start_x + offset_x, start_y, color);
    }
}

void draw_line_ver(int start_x, int start_y, int y_diff, uint8_t color)
{
    int offset_y;
    
    if (y_diff < 0)
    {
        start_y += y_diff;
        y_diff = abs(y_diff);
    }
    
    for (offset_y = 0; offset_y < y_diff + 1; offset_y++)
    {
        SET_PIXEL(start_x, start_y + offset_y, color);
    }
}

void draw_line(Vec2 v1, Vec2 v2, uint8_t color)
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
        draw_line_hor(v1.x, v1.y, x_diff, color);
    
    else if (fabs(x_diff) < 1.5)
        draw_line_ver(v1.x, v1.y, y_diff, color);
    
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
                SET_PIXEL((int)v2.x - (int)offset_x, (int)v2.y - (int)offset_y, color);
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
                SET_PIXEL((int)v2.x - (int)offset_x, (int)v2.y - (int)offset_y, color);
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
                SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
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
                SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
            }
        }
    }
    
    else if (y_diff < x_diff)
    {
        slope = y_diff / x_diff;
        
        for (offset_x = 0; offset_x < x_diff; offset_x += x_sign)
        {
            offset_y = offset_x * slope;
            SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
        }
    }
    
    else
    {
        slope = x_diff / y_diff;
        
        for (offset_y = 0; offset_y < y_diff; offset_y += y_sign)
        {
            offset_x = offset_y * slope;
            SET_PIXEL((int)v1.x + (int)offset_x, (int)v1.y + (int)offset_y, color);
        }
    }
}

void draw_line_int(Vec2_int p0, Vec2_int p1, uint8_t color)
{
    int offset_x = 0;
    int offset_y = 0;
    
    int x_diff = (p1.x - p0.x);
    int y_diff = (p1.y - p0.y);
    int counter;
    
    int x_diff_true = x_diff;
    int y_diff_true = y_diff;
    
    int x_sign = SIGN(x_diff);
    int y_sign = SIGN(y_diff);
    
    x_diff = abs(x_diff);
    y_diff = abs(y_diff);
    
    if (y_diff == 0)
        draw_line_hor(p0.x, p0.y, x_diff_true, color);
    
    else if (x_diff == 0)
        draw_line_ver(p0.x, p0.y, y_diff_true, color);
    
    else if (y_diff < x_diff)
    {
        counter = 0;
        while (offset_x < x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, color);
            offset_x++;
            counter += y_diff;
            if (counter >= x_diff)
            {
                offset_y++;
                counter -= x_diff;
            }
        }
    }
    
    else if (x_diff < y_diff)
    {
        counter = 0;
        while (offset_y < y_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, color);
            offset_y++;
            counter += x_diff;
            if (counter >= y_diff)
            {
                offset_x++;
                counter -= y_diff;
            }
        }
    }
    
    else
    {        
        while (offset_y <= y_diff && offset_x <= x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, color);
            offset_y++;
            offset_x++;
        }
    }
}

void plotTriangleLineColorBlendedInt(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge, uint8_t start_color, uint8_t end_color)
{
	int offset;
	int counter;
    int x = (int)(p1.x + 0.5) + center.x;
	int y = (int)(p1.y + 0.5) + center.y;
    int x_diff = (int)(p2.x - p1.x + 0.5);
    int y_diff = (int)(p2.y - p1.y + 0.5);
    int x_sign = SIGN(x_diff);
	int y_sign = SIGN(y_diff);
	
    int color = start_color;
    int color_diff = end_color - start_color;
	int color_sign = SIGN(color_diff);
	int color_counter;
	
	x_diff = abs(x_diff);
	y_diff = abs(y_diff);
	color_diff = abs(color_diff);
	
    if (y_diff > x_diff)
    {
		color_counter = y_diff/2;
		// beginning at y_diff/2 instead of 0 fixes a minor inaccuracy
        for (offset = 0; offset < y_diff; offset++)
        {
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
			// move along the y coordinate
			y += y_sign;
			// check whether to move on the y coordinate
			counter += x_diff;
			while (counter > y_diff)
			{
				counter -= y_diff;
				x += x_sign;
			}
			// check whether to step the color value forwards
			color_counter += color_diff;
			while (color_counter > y_diff)
			{
				color_counter -= y_diff;
				color += color_sign;
			}
        }
    }
    else if (x_diff > y_diff)
    {
		color_counter = x_diff/2;
		// beginning at x_diff/2 instead of 0 fixes a minor inaccuracy	
        for (offset = 0; offset < x_diff; offset++)
        {
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
			// move along the x coordinate
			x += x_sign;
			// check whether to move on the y coordinate
			counter += y_diff;
			while (counter > x_diff)
			{
				counter -= x_diff;
				y += y_sign;
			}
			// check whether to step the color value forwards
			color_counter += color_diff;
			while (color_counter > x_diff)
			{
				color_counter -= x_diff;
				color += color_sign;
			}
        }
    }
}

void drawLineHorzColorBlendedInt(int start_x, int end_x, int y, uint8_t start_color, uint8_t end_color)
{    
	int x = start_x;
	int offset_x;
    int x_diff = end_x - start_x;
    int x_sign = SIGN(x_diff);

    int color = start_color;
    int color_diff = end_color - start_color;
	int color_sign = SIGN(color_diff);
	int color_counter;
	// beginning at x_diff/2 instead of 0 fixes a minor inaccuracy
	
	x_diff = abs(x_diff);
	color_diff = abs(color_diff);
	color_counter = x_diff/2;
	
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(x, y, (uint8_t)color);
		x += x_sign;
		// check whether to step the color value forwards
        color_counter += color_diff;
		while (color_counter > x_diff)
		{
			color_counter -= x_diff;
			color += color_sign;
		}
    }
}

void fillSpansColorBlendedInt(int top, int bottom)
{
    int line, start_x, end_x;
    uint8_t start_color, end_color;
    
    for (line = top; line < bottom; line++)
    {
        start_x 	= (int)LeftEdge.offset[line];
        end_x 		= (int)RightEdge.offset[line];
        start_color = LeftEdge.color[line];
        end_color 	= RightEdge.color[line];
        
        drawLineHorzColorBlendedInt(start_x, end_x, line,
									start_color, end_color);
    }
}

void drawLineHorzColorBlended(int start_x, int end_x, int start_y, uint8_t start_color, uint8_t end_color)
{
    float color = start_color;
    int color_diff = end_color - start_color;
    int offset_x;
    float x_diff = end_x - start_x;
    float color_ratio = color_diff / fabs(x_diff);
    
    for (offset_x = 0; offset_x < x_diff; offset_x++)
    {
        SET_PIXEL(start_x + offset_x, start_y, (uint8_t)color);
        color += color_ratio;
    }
}

void drawLineHorzTextured(int start_x, int end_x, int start_y, Vec2 sprite_start, Vec2 sprite_end, uint8_t* sprite)
{
    Vec2 uv;
    int offset_x;
    float u_ratio;
    float v_ratio;
    float x_diff = end_x - start_x;
    float u_diff = sprite_end.x - sprite_start.x;
    float v_diff = sprite_end.y - sprite_start.y;
    int x, y, pixel;
    uv.x = sprite_start.x;
    uv.y = sprite_start.y;
    
    x_diff = fabs(x_diff);
    
	if (x_diff != 0.0)
	{
		u_ratio = u_diff / x_diff;
		v_ratio = v_diff / x_diff;

		for (offset_x = 0; offset_x < x_diff; offset_x++)
		{
			pixel = (int)uv.y * 100 + (int)uv.x;
			SET_PIXEL(start_x + offset_x, start_y, sprite[pixel]);
			uv.x += u_ratio;
			uv.y += v_ratio;
		}
	}
}

void drawLineHorzMeshTextured(int start_x, int end_x, int start_y, Vec2 tex_start, Vec2 tex_end, Texture* texture)
{
    Vec2 uv;
    int offset_x;
    float u_ratio;
    float v_ratio;
    float x_diff = end_x - start_x;
    float u_diff = tex_end.x - tex_start.x;
    float v_diff = tex_end.y - tex_start.y;
	int tex_x_mask = texture->width-1; // Caution: This assumes texture dimensions
	int tex_y_mask = texture->height-1; // are powers of 2! Shit breaks otherwise
    int x, y, pixel;
    uint8_t color;
    uv.x = tex_start.x;
    uv.y = tex_start.y;
	start_x++;
    
    x_diff = fabs(x_diff);
    
	if (x_diff != 0)
	{
	    u_ratio = u_diff / x_diff;
	    v_ratio = v_diff / x_diff;
	
	    
	    if (texture->transparent == TRUE)
	    {
	        for (offset_x = 0; offset_x < x_diff; offset_x++)
	        {
	            //pixel = (((int)uv.y) % texture->height) * texture->width
				//		+ (((int)uv.x) % texture->width);
	            pixel = (((int)uv.y) & tex_y_mask) * texture->width
						+ (((int)uv.x) & tex_x_mask);
	            color = texture->pixels[pixel];
	            if (color != TRANSPARENT_COLOR)
	                SET_PIXEL(start_x + offset_x, start_y, color);
	            uv.x += u_ratio;
	            uv.y += v_ratio;
	        }
	    }
	    else
	    {
	        for (offset_x = 0; offset_x < x_diff; offset_x++)
	        {
	            //pixel = (((int)uv.y) % texture->height) * texture->width
				//		+ (((int)uv.x) % texture->width);
	            pixel = (((int)uv.y) & tex_y_mask) * texture->width
						+ (((int)uv.x) & tex_x_mask);
	            color = texture->pixels[pixel];
	            SET_PIXEL(start_x + offset_x, start_y, color);
	            uv.x += u_ratio;
	            uv.y += v_ratio;
	        }
	    }
	}
}

void drawLineColorBlended(Vec2 p0, Vec2 p1, uint8_t st_col, uint8_t end_col)
{
    int offset_x = 0;
    int offset_y = 0;
    
    int x_diff = (p1.x - p0.x);
    int y_diff = (p1.y - p0.y);
    
    int length = sqrt((abs(x_diff) * abs(x_diff)) + (abs(y_diff) * abs(y_diff)));
	// why the fuck do you take square root here lmao wtffbbq XDXDSDDD
    uint8_t col_add = 0;
    uint8_t col_step = length / abs(end_col - st_col);
    uint8_t col_counter = 0;
    
    int counter;
    
    int x_diff_true = x_diff;
    int y_diff_true = y_diff;
    
    int x_sign = SIGN(x_diff);
    int y_sign = SIGN(y_diff);
    
    x_diff = abs(x_diff);
    y_diff = abs(y_diff);
    
    if (y_diff == 0)
    {
        if (x_diff_true < 0)
        {
            p0.x += x_diff_true;
        }
        
        for (offset_x = 0; offset_x < x_diff; offset_x++)
        {
            SET_PIXEL(p0.x + offset_x, p0.y, st_col + col_add);
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
        }
    }
        
    else if (x_diff == 0)
    {
        if (y_diff_true < 0)
        {
            p0.y += y_diff_true;
        }
        
        for (offset_y = 0; offset_y < y_diff; offset_y++)
        {
            SET_PIXEL(p0.x, p0.y + offset_y, st_col + col_add);
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
        }
    }
    
    else if (y_diff < x_diff)
    {
        counter = 0;
        while (offset_x < x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, st_col + col_add);
            offset_x++;
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
            counter += y_diff;
            if (counter >= x_diff)
            {
                offset_y++;
                counter -= x_diff;
            }
        }
    }
    
    else if (x_diff < y_diff)
    {
        counter = 0;
        while (offset_y < y_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, st_col + col_add);
            offset_y++;
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
            counter += x_diff;
            if (counter >= y_diff)
            {
                offset_x++;
                counter -= y_diff;
            }
        }
    }
    
    else
    {        
        while (offset_y <= y_diff && offset_x <= x_diff)
        {
            SET_PIXEL(p0.x + offset_x * x_sign, p0.y + offset_y * y_sign, st_col + col_add);
            offset_y++;
            offset_x++;
            col_counter++;
            if (col_counter == col_step && (st_col + col_add) < end_col)
            {
                col_add++;
                col_counter = 0;
            }
        }
    }
}

void test_draw()
{
    char i = 0;
    float angle = 12;
    int radius = 90;
    Vec2 v1;
    Vec2 v2;
    Vec2 v3;
    
    while (i < 12)
    {
        v1.x = cos(angle) * radius + 160.0;
        v1.y = sin(angle) * radius + 100.0;
        v2.x = cos(angle + RAD_30) * radius + 160.0;
        v2.y = sin(angle + RAD_30) * radius + 100.0;
        v3.x = 160.0;
        v3.y = 100.0;
        drawLineColorBlended(v1, v2, 64, 79);
        drawLineColorBlended(v3, v1, 64, 79);
        //SET_PIXEL(v1.x, v1.y, 14);
        //SET_PIXEL(v2.x, v2.y, 14);
        angle += RAD_30;
        i++;
    }
}

Polygon makeSquare(float angle, float side_length, float scale, uint8_t color)
{
    Polygon newSquare;
    
    newSquare.numPoints = 4;
    newSquare.points = malloc(newSquare.numPoints * sizeof(Vec2));
    newSquare.transformedV = malloc(newSquare.numPoints * sizeof(Vec2));
    newSquare.angle = angle;
    newSquare.scale = scale;
    newSquare.color = color;
    
    newSquare.points[0].x = -side_length/2.0;
    newSquare.points[0].y = -side_length/2.0;
    newSquare.points[1].x = side_length/2.0;
    newSquare.points[1].y = -side_length/2.0;
    newSquare.points[2].x = side_length/2.0;
    newSquare.points[2].y = side_length/2.0;
    newSquare.points[3].x = -side_length/2.0;
    newSquare.points[3].y = side_length/2.0;
    
    memcpy(newSquare.transformedV, newSquare.points, sizeof(Vec2) * newSquare.numPoints);
    
    return newSquare;
}

Polygon makePolygon(float angle, int numPoints, float radius, float scale, uint8_t color)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numPoints;
    
    newPolygon.numPoints = numPoints;
    newPolygon.points = malloc(numPoints * sizeof(Vec2));
    newPolygon.transformedV = malloc(numPoints * sizeof(Vec2));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.scale = scale;
    newPolygon.color = color;
    
    while (i < newPolygon.numPoints)
    {
        newPolygon.points[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.points[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    memcpy(newPolygon.transformedV, newPolygon.points, sizeof(Vec2) * newPolygon.numPoints);
    
    return newPolygon;
}

Polygon makeShadedTriangle(float angle, int numPoints, float radius, float scale, uint8_t col_1, uint8_t col_2, uint8_t col_3)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numPoints;
    
    newPolygon.numPoints = numPoints;
    newPolygon.points = malloc(numPoints * sizeof(Vec2));
    newPolygon.transformedV = malloc(numPoints * sizeof(Vec2));
    newPolygon.vertexColors = malloc(3 * sizeof(uint8_t));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.scale = scale;
    
    while (i < newPolygon.numPoints)
    {
        newPolygon.points[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.points[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    newPolygon.vertexColors[0] = col_1;
    newPolygon.vertexColors[1] = col_2;
    newPolygon.vertexColors[2] = col_3;
    
    memcpy(newPolygon.transformedV, newPolygon.points, sizeof(Vec2) * newPolygon.numPoints);
    
    return newPolygon;
}

Polygon makeTexturedTriangle(float angle, int numPoints, float radius, float scale, Vec2 uv_1, Vec2 uv_2, Vec2 uv_3, Vec2_int center)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numPoints;
    
    newPolygon.numPoints = numPoints;
    newPolygon.points = malloc(numPoints * sizeof(Vec2));
    newPolygon.transformedV = malloc(numPoints * sizeof(Vec2));
    newPolygon.UVCoords = malloc(3 * sizeof(Vec2));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.scale = scale;
    newPolygon.center.x = center.x;
    newPolygon.center.y = center.y;
    
    while (i < newPolygon.numPoints)
    {
        newPolygon.points[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.points[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    newPolygon.UVCoords[0] = uv_1;
    newPolygon.UVCoords[1] = uv_2;
    newPolygon.UVCoords[2] = uv_3;
    
    memcpy(newPolygon.transformedV, newPolygon.points, sizeof(Vec2) * newPolygon.numPoints);
    
    return newPolygon;
}

void plotTriangleLine(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge)
{
    uint8_t color;
    
    float offset_y;
    float offset_x;
    
    float x_diff = (p2.x - p1.x);
    float y_diff = (p2.y - p1.y);
   
    float slope = 0.0;
    
    int y_sign = SIGN(y_diff);
    int x, y;
    
    y_diff = fabs(y_diff);
    
    slope = x_diff / y_diff;
        
    for (offset_y = 0; offset_y < y_diff; offset_y++)
    {
        offset_x = offset_y * slope;
        x = (int)(center.x + p1.x + offset_x);
        y = (int)(center.y + p1.y + offset_y) * y_sign;
        edge->offset[y] = x;
        //SET_PIXEL(edge->offset[y], y, color);
    }
}

void plotTriangleLineColorBlended(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge, uint8_t color_1, uint8_t color_2)
{
    float color = color_1;
    int color_diff = color_2 - color_1;
    uint8_t test_color;
    
    float offset_y;
    float offset_x;
    
    float x_diff = (p2.x - p1.x);
    float y_diff = (p2.y - p1.y);
    
    float color_ratio;
   
    float slope = 0.0;
    
    int y_sign = SIGN(y_diff);
    int x, y;
    
    if (edge == &LeftEdge)
        test_color = 32;
    
    else if (edge == & RightEdge)
        test_color = 40;
    
    if (y_diff > x_diff)
    {
        y_diff = fabs(y_diff);
        color_ratio = color_diff / y_diff;
        slope = x_diff / y_diff;
            
        for (offset_y = 0; offset_y < y_diff; offset_y++)
        {
            offset_x = offset_y * slope;
            x = (int)(center.x + p1.x + offset_x);
            y = (int)(center.y + p1.y + offset_y) * y_sign;
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
            color += color_ratio;
            if (KEY_IS_PRESSED (KEY_Q))
                SET_PIXEL(edge->offset[y], y, test_color);
            else if (KEY_IS_PRESSED (KEY_R))
                SET_PIXEL(edge->offset[y], y, edge->color[y]);
        }
    }
    
    else if (x_diff > y_diff)
    {
        x_diff = fabs(x_diff);
        color_ratio = color_diff / x_diff; //Potential divide by zero
        slope = y_diff / x_diff;//Potential divide by zero
            
        for (offset_x = 0; offset_x < x_diff; offset_x++)
        {
            offset_y = offset_x * slope;
            x = (int)(center.x + p1.x + offset_x);
            y = (int)(center.y + p1.y + offset_y) * y_sign;
            edge->offset[y] = x;
            edge->color[y] = (uint8_t)color;
            color += color_ratio;
            if (KEY_IS_PRESSED (KEY_Q))
                SET_PIXEL(edge->offset[y], y, test_color);
            else if (KEY_IS_PRESSED (KEY_R))
                SET_PIXEL(edge->offset[y], y, edge->color[y]);
        }
    }
}

void plotTriangleLineTexture(Vec2 p1, Vec2 p2, Vec2_int center, Span* edge, Vec2 tex_start, Vec2 tex_end)
{
    Vec2 uv;
    
    float offset_y;
    float offset_x;
    
    float x_diff = (p2.x - p1.x);
    float y_diff = (p2.y - p1.y);
	
    float u_diff = (tex_end.x - tex_start.x);
    float v_diff = (tex_end.y - tex_start.y);
    
    float u_ratio;
    float v_ratio;
    
    float slope = 0.0;
    
    int y_sign = SIGN(y_diff);
	int x_sign = SIGN(x_diff);
	
    int x, y;
	
	uint8_t test_color;
    
	uv.x = tex_start.x;
	uv.y = tex_start.y;
    y_diff = fabs(y_diff)+1;
	x_diff = fabs(x_diff)+1;
	
    if (y_diff > x_diff)
    {
	    slope = (x_diff / y_diff) * x_sign;
	    u_ratio = u_diff / y_diff;
	    v_ratio = v_diff / y_diff;

        for (offset_y = 0; offset_y < y_diff; offset_y++)
        {
            offset_x = offset_y * slope;
            x = (int)(center.x + p1.x + offset_x);
            y = (int)(center.y + p1.y + (offset_y * y_sign));
            edge->offset[y] = x;
            edge->texture[y].x = uv.x;
            edge->texture[y].y = uv.y;
			uv.x += u_ratio;
            uv.y += v_ratio;
        }
    }
    
    else if (x_diff > y_diff)
    {  
        slope = (y_diff / x_diff) * y_sign;
	    u_ratio = u_diff / x_diff;
	    v_ratio = v_diff / x_diff;
            
        for (offset_x = 0; offset_x < x_diff; offset_x++)
        {
            offset_y = offset_x * slope;
            x = (int)(center.x + p1.x + (offset_x * x_sign));
            y = (int)(center.y + p1.y + offset_y);
            edge->offset[y] = x;
            edge->texture[y].x = uv.x;
            edge->texture[y].y = uv.y;
			uv.x += u_ratio;
            uv.y += v_ratio;
        }
    }
	
	// for debugging purposes
	if (KEY_IS_PRESSED(KEY_W))
	{
	    if (edge == &LeftEdge)
	        test_color = 32;
	    
	    else if (edge == & RightEdge)
	        test_color = 40;
			
		p1.x += center.x;
		p1.y += center.y;
		p2.x += center.x;
		p2.y += center.y;
		draw_line(p1, p2, test_color);
	}
}

void fillSpans(int top, int bottom, uint8_t color)
{
    int line;
    Vec2 p0, p1;
    
    for (line = top; line < bottom; line++)
    {
        draw_line_hor(LeftEdge.offset[line], line, (RightEdge.offset[line] - LeftEdge.offset[line]), color);
    }
}

void fillSpansColorBlended(int top, int bottom)
{
    int line;
    Vec2 p0, p1;
    uint8_t col_1, col_2;
    
    for (line = top; line < bottom; line++)
    {
        p0.y = line;
        p1.y = line;
        p0.x = LeftEdge.offset[line];
        p1.x = RightEdge.offset[line];
        col_1 = LeftEdge.color[line];
        col_2 = RightEdge.color[line];
        
        //drawLineColorBlended(p0, p1, col_1, col_2);
        drawLineHorzColorBlended(p0.x, p1.x, line, col_1, col_2);
    }
}

void fillSpansTextured(int top, int bottom, uint8_t* sprite)
{
    int line;
    Vec2 p0, p1;
    Vec2 tex_start;
    Vec2 tex_end;

    for (line = top; line < bottom; line++)
    {
        p0.x = LeftEdge.offset[line];
        p1.x = RightEdge.offset[line];
        
        tex_start.x = LeftEdge.texture[line].x;
        tex_start.y = LeftEdge.texture[line].y;
        tex_end.x = RightEdge.texture[line].x;
        tex_end.y = RightEdge.texture[line].y;
        
        drawLineHorzTextured(p0.x, p1.x, line, tex_start, tex_end, sprite);
    }
}

void fillSpansMeshTextured(int top, int bottom, Texture* texture)
{
    int line;
    Vec2 p0, p1;
    Vec2 tex_start;
    Vec2 tex_end;

    for (line = top; line < bottom; line++)
    {
        p0.x = LeftEdge.offset[line];
        p1.x = RightEdge.offset[line];
        
        tex_start.x = LeftEdge.texture[line].x;
        tex_start.y = LeftEdge.texture[line].y;
        tex_end.x = RightEdge.texture[line].x;
        tex_end.y = RightEdge.texture[line].y;
		
		//printf("Line %d OK!\n", line);
        
        drawLineHorzMeshTextured(p0.x, p1.x, line, tex_start, tex_end, texture);
    }
}

void sortPair(Vec2* v0, Vec2* v1)
{
    Vec2 temp;
    
    if (v0->y > v1->y)
    {
        temp = *v0;
        *v0 = *v1;
        *v1 = temp;
    }
}

void sortPairWithColor(Vec2* v0, Vec2* v1, uint8_t* color1, uint8_t* color2)
{
    Vec2 temp;
    uint8_t Coloremp;
    
    if (v0->y > v1->y)
    {
        temp = *v0;
        Coloremp = *color1;
        *v0 = *v1;
        *color1 = *color2;
        *v1 = temp;
        *color2 = Coloremp;
    }
}

void sortPairWithPoint(Vec2* v0, Vec2* v1, Vec2* p0, Vec2* p1)
{
    Vec2 temp;
    Vec2 point_temp;
    
    if (v0->y > v1->y)
    {
        temp = *v0;
        point_temp = *p0;
        *v0 = *v1;
        *p0 = *p1;
        *v1 = temp;
        *p1 = point_temp;
    }
}

void drawFilledTriangle(Polygon* triangle)
{    
    Vec2 A, B, C;
    Vec2 AB, AC;
    Vec2_int center = {159, 99};
    float cross_product;
    
    A = triangle->transformedV[0];
    B = triangle->transformedV[1];
    C = triangle->transformedV[2];
    
    sortPair(&A, &B);
    sortPair(&A, &C);
    sortPair(&B, &C);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLine(A, B, center, &RightEdge);
        plotTriangleLine(B, C, center, &RightEdge);
        plotTriangleLine(A, C, center, &LeftEdge);
    }
    else
    {
        plotTriangleLine(A, B, center, &LeftEdge);
        plotTriangleLine(B, C, center, &LeftEdge);
        plotTriangleLine(A, C, center, &RightEdge);
    }
    if (KEY_IS_PRESSED (KEY_E))
        fillSpans(center.y + A.y, center.y + C.y, 85);
}

void drawShadedTriangle(Polygon* triangle)
{    
    Vec2 A, B, C;
    Vec2 AB, AC;
    Vec2_int center = {99, 99};
    float cross_product;
    uint8_t A_color, B_color, C_color;
    
    A = triangle->transformedV[0];
    B = triangle->transformedV[1];
    C = triangle->transformedV[2];
    
    if (KEY_IS_PRESSED (KEY_1))
        triangle->vertexColors[0]++;
    
    else if (KEY_IS_PRESSED (KEY_2))
        triangle->vertexColors[1]++;
    
    else if (KEY_IS_PRESSED (KEY_3))
        triangle->vertexColors[2]++;
    
    else if (KEY_IS_PRESSED (KEY_4))
        triangle->vertexColors[0]--;
    
    else if (KEY_IS_PRESSED (KEY_5))
        triangle->vertexColors[1]--;
    
    else if (KEY_IS_PRESSED (KEY_6))
        triangle->vertexColors[2]--;
    
    A_color = triangle->vertexColors[0];
    B_color = triangle->vertexColors[1];
    C_color = triangle->vertexColors[2];
    
    sortPairWithColor(&A, &B, &A_color, &B_color);
    sortPairWithColor(&A, &C, &A_color, &C_color);
    sortPairWithColor(&B, &C, &B_color, &C_color);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLineColorBlendedInt(A, B, center, &RightEdge, A_color, B_color);
        plotTriangleLineColorBlendedInt(B, C, center, &RightEdge, B_color, C_color);
        plotTriangleLineColorBlendedInt(A, C, center, &LeftEdge, A_color, C_color);
    }
    else
    {
        plotTriangleLineColorBlendedInt(A, B, center, &LeftEdge, A_color, B_color);
        plotTriangleLineColorBlendedInt(B, C, center, &LeftEdge, B_color, C_color);
        plotTriangleLineColorBlendedInt(A, C, center, &RightEdge, A_color, C_color);
    }
    //if (KEY_IS_PRESSED (KEY_W))
    fillSpansColorBlendedInt(center.y + A.y, center.y + C.y);
}

void drawTexturedTriangle(Polygon* triangle)
{    
    Vec2 A, B, C, point_a, point_b, point_c;
    Vec2 AB, AC;
    Vec2_int center = triangle->center;
    float cross_product;
    
    A = triangle->transformedV[0];
    B = triangle->transformedV[1];
    C = triangle->transformedV[2];
    
    point_a = triangle->UVCoords[0];
    point_b = triangle->UVCoords[1];
    point_c = triangle->UVCoords[2];
    
    sortPairWithPoint(&A, &B, &point_a, &point_b);
    sortPairWithPoint(&A, &C, &point_a, &point_c);
    sortPairWithPoint(&B, &C, &point_b, &point_c);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLineTexture(A, B, center, &RightEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &RightEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &LeftEdge, point_a, point_c);
    }
    else
    {
        plotTriangleLineTexture(A, B, center, &LeftEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &LeftEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &RightEdge, point_a, point_c);
    }
    fillSpansTextured(center.y + A.y, center.y + C.y, tile);
}

void draw2DMeshTriangle(Mesh2D* mesh, int triangle_ID)
{    
    Vec2 A, B, C, point_a, point_b, point_c;
    Vec2 AB, AC;
    Vec2_int center = mesh->center;
    float cross_product;
    
    A = mesh->transformedP[mesh->triangleVertices[0 + triangle_ID * 3].pointID];
    B = mesh->transformedP[mesh->triangleVertices[1 + triangle_ID * 3].pointID];
    C = mesh->transformedP[mesh->triangleVertices[2 + triangle_ID * 3].pointID];
    
    point_a = mesh->triangleVertices[0 + triangle_ID * 3].UV;
    point_b = mesh->triangleVertices[1 + triangle_ID * 3].UV;
    point_c = mesh->triangleVertices[2 + triangle_ID * 3].UV;
    
    sortPairWithPoint(&A, &B, &point_a, &point_b);
    sortPairWithPoint(&A, &C, &point_a, &point_c);
    sortPairWithPoint(&B, &C, &point_b, &point_c);
    
    AB.x = B.x - A.x;
    AB.y = B.y - A.y;
    AC.x = C.x - A.x;
    AC.y = C.y - A.y;
    
    cross_product = (AB.x * AC.y) - (AB.y * AC.x);
    
    if (cross_product > 0)
    {
        plotTriangleLineTexture(A, B, center, &RightEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &RightEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &LeftEdge, point_a, point_c);
    }
    else
    {
        plotTriangleLineTexture(A, B, center, &LeftEdge, point_a, point_b);
        plotTriangleLineTexture(B, C, center, &LeftEdge, point_b, point_c);
        plotTriangleLineTexture(A, C, center, &RightEdge, point_a, point_c);
    }
    fillSpansMeshTextured(center.y + A.y, center.y + C.y, mesh->texture);
}

void draw2DMesh(Mesh2D* mesh)
{   
    int i = 0;

    while (i < mesh->numTriangles)
    {
        draw2DMeshTriangle(mesh, i);
        i++;
    }
}

void draw_polygon(Polygon* poly, int center_x, int center_y)
{
    char i = 0;
    Vec2 start_loc;
    Vec2 end_loc;
    
    while (i < poly->numPoints - 1)
    {
        start_loc.x = center_x + poly->transformedV[i].x;
        start_loc.y = center_y + poly->transformedV[i].y;
        
        end_loc.x = center_x + poly->transformedV[i + 1].x;
        end_loc.y = center_y + poly->transformedV[i + 1].y;

        draw_line(start_loc, end_loc, poly->color);
        
        i++;
    }
    
    start_loc.x = center_x + poly->transformedV[i].x;
    start_loc.y = center_y + poly->transformedV[i].y;
    
    end_loc.x = center_x + poly->transformedV[0].x;
    end_loc.y = center_y + poly->transformedV[0].y;
    
    draw_line(start_loc, end_loc, poly->color);
}

void draw_polygon_int(Polygon* poly, int center_x, int center_y)
{
    char i = 0;
    Vec2_int start_loc;
    Vec2_int end_loc;
    
    while (i < poly->numPoints - 1)
    {
        start_loc.x = center_x + (int)poly->transformedV[i].x;
        start_loc.y = center_y + (int)poly->transformedV[i].y;
        
        end_loc.x = center_x + (int)poly->transformedV[i + 1].x;
        end_loc.y = center_y + (int)poly->transformedV[i + 1].y;

        draw_line_int(start_loc, end_loc, poly->color);
        
        i++;
    }
    
    start_loc.x = center_x + (int)poly->transformedV[i].x;
    start_loc.y = center_y + (int)poly->transformedV[i].y;
    
    end_loc.x = center_x + (int)poly->transformedV[0].x;
    end_loc.y = center_y + (int)poly->transformedV[0].y;
    
    draw_line_int(start_loc, end_loc, poly->color);
}

void draw_lines()
{
    char i = 0;
    Vec2 p0 = {100.0, 185.0};
    Vec2 p1 = {170.0, 165.0};
    
    while (i < 5)
    {
        draw_line(line_array[i].startpos, line_array[i].endpos, line_array[i].color);
        i++;
    }
    draw_line_int(intline_array[0].startpos, intline_array[0].endpos, line_array[0].color);
    draw_line_int(intline_array[1].startpos, intline_array[1].endpos, line_array[1].color);
    drawLineColorBlended(p0, p1, 16, 31);
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
    draw_polygon_int(&poly_array[i], 60, 150);
}

void draw_stuff()
{
    //draw_lines();
    //draw_polygons();
    //test_draw();
    //drawFilledTriangle(&poly_array[5]);
    drawShadedTriangle(&poly_array[5]);
    //drawTexturedTriangle(&poly_array[6]);
    //drawTexturedTriangle(&poly_array[7]);
    //draw_sprite(144, 68, &CarTex);
    draw2DMesh(&CarMesh);
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

void makeQuadrilateral()
{
    Vec2 point_a = {24.5, 0.0};
    Vec2 point_b = {0.0, 99.0};
    Vec2 point_c = {49.0, 99.0};
    Vec2_int center = {100, 68};
    
    poly_array[6] = makeTexturedTriangle(0.0, 3, 35.0, 1.0, point_a, point_b, point_c, center);
    point_a.x = 74.5;
    point_a.y = 99.0;
    point_b.x = 50.0;
    point_b.y = 0.0;
    point_c.x = 99.0;
    point_c.y = 0.0;
    center.x = 130;
    center.y = 85;
    poly_array[7] = makeTexturedTriangle(0.0, 3, 35.0, 1.0, point_a, point_b, point_c, center);
    poly_array[6].angle = 5.76;
    poly_array[7].angle = 2.618;
    updatePoly(&poly_array[6]);
    updatePoly(&poly_array[7]);
}

void main()
{   
    poly_array[0] = makeSquare(0.0, 10.0, 1.0, 44);
    poly_array[1] = makePolygon(0.0, 3, 15.0, 1.0, 47);
    poly_array[2] = makePolygon(0.0, 5, 25.0, 1.0, 47);
    poly_array[3] = makePolygon(0.0, 5, 25.0, 1.0, 47);
    poly_array[4] = makePolygon(0.0, 3, 15.0, 1.0, 47);
    poly_array[5] = makeShadedTriangle(0.0, 3, 15.0, 1.0, 16, 23, 31);
    //makeQuadrilateral();
	
    set_mode(VGA_256_COLOR_MODE);
    loadPalette("Pal.bmp", &NewPalette);
    setPalette_VGA(&NewPalette);
    
    //load_font();
    load_gfx("WALL.7UP", tile, 10000);
    load_texture("CAR.7UP", &CarTex, 32, 64, TRUE);

    MakeCarMesh(&CarMesh);
    mesh_array[0] = CarMesh;
    
    init_keyboard();
    
    while (running == 1)
    {
        process_input();
        draw_stuff();
        render();
        //delay(10);
    }
    quit();
}