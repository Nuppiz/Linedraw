#include "Common.h"
#include "Structs.h"

extern uint8_t alphabet;

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