#include "Common.h"
#include "Defines.h"
#include "Video.h"
#include "Draw.h"
#include "Lines.h"
#include "Loadgfx.h"
#include "Keyb.h"
#include "Mesh.h"
#include "Polygon.h"
#include "Structs.h"
#include "Text.h"
#include "Triangle.h"

int running = 1;
uint8_t tile [10000];
uint8_t car [2048];

extern Palette NewPalette;
extern Mesh2D CarMesh;
extern Texture CarTex;
extern Object3D Cube;

extern uint8_t far screen_buf [];

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
Object3D object_array[1];

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

void quit()
{   
    deinitKeyboard();
    setMode(TEXT_MODE);
}

void main()
{   	
    setMode(VGA_256_COLOR_MODE);
    loadPalette("Pal.bmp", &NewPalette);
    setPalette_VGA(&NewPalette);
    
    loadGfx("WALL.7UP", tile, 10000);
    loadTexture("CAR.7UP", &CarTex, 32, 64, TRUE);
    loadFont();

    createPolygons();

    MakeCarMesh(&CarMesh);
    MakeCube(&Cube);
    mesh_array[0] = CarMesh;
    object_array[0] = Cube;
    
    initKeyboard();
    
    while (running == 1)
    {
        processInput();
        drawStuff();
        render();
        delay(10);
    }
    quit();
}