#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Structs.h"

void drawShadedTriangle(Polygon* triangle);
void drawTexturedTriangle(Polygon* triangle);
void draw2DMeshTriangle(Polygon* triangle);
void draw3DCubeTriangle(Mesh3D* cube, int triangle_ID, uint8_t color);
void draw3DCubeTriangleTex(Mesh3D* cube, int triangle_ID, int side_ID);
void draw3DCubeTriangleTexAlt(Mesh3D* cube, int triangle_ID, int side_ID);

#endif
