#include "Common.h"
#include "Structs.h"

/* Graphics loading functions */

Texture Textures[NUM_TEXTURES];

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    // load raw graphics data (no dimensions and flags)
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void loadTexture(char* filename, int texture_index)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(&Textures[texture_index].width, 2, 1, file_ptr);
    fseek(file_ptr, 2, SEEK_SET);
    fread(&Textures[texture_index].height, 2, 1, file_ptr);
    fseek(file_ptr, 6, SEEK_SET);
    fread(&Textures[texture_index].transparent, 2, 1, file_ptr);
	fseek(file_ptr, 8, SEEK_SET);
    Textures[texture_index].pixels = malloc(Textures[texture_index].width * Textures[texture_index].height);
    fread(Textures[texture_index].pixels, 1, Textures[texture_index].width * Textures[texture_index].height, file_ptr);
    fclose(file_ptr);
}

void loadAllTextures()
{
    loadTexture("CAR.7UP", CAR);
    loadTexture("BEAWATER.7UP", BEACH);
    loadTexture("BUSH.7UP", BUSH);
    loadTexture("CONCRETE.7UP", CONCRETE);
    loadTexture("GRASSAND.7UP", GRASSSAND);
    loadTexture("TREES.7UP", TREES);
    loadTexture("WALL.7UP", WALL);
}