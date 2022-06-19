#ifndef VIDEO_H
#define VIDEO_H

#include "Structs.h"

void setMode(uint8_t mode);
void loadPalette(char* filename, Palette* pal);
void setPalette_VGA(Palette* pal);
void render();

#endif /* VIDEO_H */
