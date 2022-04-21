#include "Common.h"
#include "Structs.h"

/* Graphics loading functions */

extern Texture CarTex;
extern Texture BeaWater;
extern Texture Bush;
extern Texture Concrete;
extern Texture GrasSand;
extern Texture Trees;
extern Texture Wall;

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void loadTexture(char* filename, Texture* texture, int width, int height, int transparent)
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

void loadAllTextures()
{
    loadTexture("CAR.7UP", &CarTex, 32, 64, TRUE);
    loadTexture("BEAWATER.7UP", &BeaWater, 32, 32, FALSE);
    loadTexture("BUSH.7UP", &Bush, 32, 32, FALSE);
    loadTexture("CONCRETE.7UP", &Concrete, 32, 32, FALSE);
    loadTexture("GRASSAND.7UP", &GrasSand, 32, 32, FALSE);
    loadTexture("TREES.7UP", &Trees, 32, 32, FALSE);
    loadTexture("WALL.7UP", &Wall, 32, 32, FALSE);
}