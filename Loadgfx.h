#ifndef LOADGFX_H
#define LOADGFX_H

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
void loadTexture(char* filename, Texture* texture, int width, int height, int transparent);

#endif
