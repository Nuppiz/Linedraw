#ifndef LINES_H
#define LINES_H

#include "Structs.h"

void drawLine(Vec2 v1, Vec2 v2, uint8_t color);
void drawLineInt(Vec2_int p0, Vec2_int p1, uint8_t color);
void drawLineHorzColorBlendedInt(int start_x, int end_x, int y, uint8_t start_color, uint8_t end_color);
void drawLineHorzColorBlended(int start_x, int end_x, int start_y, uint8_t start_color, uint8_t end_color);
void drawLineHorzTextured(int start_x, int end_x, int start_y, Vec2 sprite_start, Vec2 sprite_end, uint8_t* sprite);
void drawLineHorzMeshTextured(int start_x, int end_x, int start_y, Vec2 tex_start, Vec2 tex_end, Texture* texture);

#endif
