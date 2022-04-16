#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Keyboard driver and control inputs */

extern Polygon poly_array [];
extern Mesh2D mesh_array [];
extern Object3D object_array [];
extern int running;

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

void initKeyboard()
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

void deinitKeyboard()
{
    // restore old keyhandler
    asm cli
    _dos_setvect(9, old_keyhandler);
    asm sti

    g_Input = NULL;
}

void getKeyboard()
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

void clearKeys()
{
    int i;
    for (i = 0; i < KB_ARRAY_LENGTH; i++)
        g_Input->kb_array[i] &= KEY_PRESSED_FLAG;
}

void controlIngame()
{
    if (KEY_IS_PRESSED(KEY_UP))
    {
        //poly_array[4].transformedV[2] = change_vec_angle(poly_array[4].transformedV[2], RAD_15);
        object_array[0].x_angle += TURN_RATE;
        applyObject3DRotation(&object_array[0]);
    }
    
    else if (KEY_IS_PRESSED(KEY_DOWN))
    {
        //poly_array[4].transformedV[2] = change_vec_angle(poly_array[4].transformedV[2], -RAD_15);
        object_array[0].x_angle -= TURN_RATE;
        applyObject3DRotation(&object_array[0]);
    }

    else if (KEY_IS_PRESSED(KEY_LEFT))
    {
        object_array[0].y_angle -= TURN_RATE;
        applyObject3DRotation(&object_array[0]);
    }

    else if (KEY_IS_PRESSED(KEY_RIGHT))
    {
        object_array[0].y_angle += TURN_RATE;
        applyObject3DRotation(&object_array[0]);
    }
    
    else if (KEY_IS_PRESSED(KEY_U))
    {
        //modObject.angle -= TURN_RATE;
        //mesh_array[0].angle -= TURN_RATE;
        object_array[0].z_angle -= TURN_RATE;
        //updatePoly(&modObject);
        //updateMesh(&mesh_array[0]);
        applyObject3DRotation(&object_array[0]);
    }
    
    else if (KEY_IS_PRESSED(KEY_I))
    {
        //modObject.angle += TURN_RATE;
        //mesh_array[0].angle += TURN_RATE;
        object_array[0].z_angle += TURN_RATE;
        //updatePoly(&modObject);
        //updateMesh(&mesh_array[0]);
        applyObject3DRotation(&object_array[0]);
    }
    
    else if (KEY_IS_PRESSED(KEY_ADD))
    {
        modObject.scale *= 1.05;
        mesh_array[0].scale *= 1.05;
        object_array[0].scale *= 1.05;
        updatePoly(&modObject);
        updateMesh(&mesh_array[0]);
        //applyObject3DRotation(&object_array[0]);
    }
    
    else if (KEY_IS_PRESSED(KEY_SUB))
    {
        modObject.scale /= 1.05;
        mesh_array[0].scale /= 1.05;
        object_array[0].scale /= 1.05;
        updatePoly(&modObject);
        updateMesh(&mesh_array[0]);
        //applyObject3DRotation(&object_array[0]);
    }
}

void processInput()
{
    getKeyboard();

    controlIngame();
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        running = 0;

    clearKeys();
}