#include "Common.h"
#include "Structs.h"

/* Graphics loading functions */

Texture Textures[NUM_TEXTURES];

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void loadTexture(char* filename, int texture_index, int width, int height, int transparent)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    Textures[texture_index].pixels = malloc(width * height);
    fread(Textures[texture_index].pixels, 1, width * height, file_ptr);
    Textures[texture_index].height = height;
    Textures[texture_index].width = width;
    Textures[texture_index].transparent = transparent;
    fclose(file_ptr);
}

void loadAllTextures()
{
    loadTexture("CAR.7UP", CAR, 32, 64, TRUE);
    loadTexture("BEAWATER.7UP", BEACH, 32, 32, FALSE);
    loadTexture("BUSH.7UP", BUSH, 32, 32, FALSE);
    loadTexture("CONCRETE.7UP", CONCRETE, 32, 32, FALSE);
    loadTexture("GRASSAND.7UP", GRASSSAND, 32, 32, FALSE);
    loadTexture("TREES.7UP", TREES, 32, 32, FALSE);
    loadTexture("WALL.7UP", WALL, 32, 32, FALSE);
}