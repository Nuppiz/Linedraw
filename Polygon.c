#include "Common.h"
#include "Defines.h"
#include "Structs.h"

extern Polygon* poly_array;

void updatePoly(Polygon* poly)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_y;
    float new_x;
    float new_y;
    
    for (i = 0; i < poly->numPoints; i++)
    {
        old_x = poly->points[i].x;
        old_y = poly->points[i].y;
        cos_angle = cos(poly->angle);
        sin_angle = sin(poly->angle);
        new_x = (old_x * cos_angle - old_y * sin_angle) * poly->scale;
        new_y = (old_x * sin_angle + old_y * cos_angle) * poly->scale;
        poly->transformedV[i].x = new_x;
        poly->transformedV[i].y = new_y;
    }
}

Polygon makeSquare(float angle, float side_length, float scale, uint8_t color)
{
    Polygon newSquare;
    
    newSquare.numPoints = 4;
    newSquare.points = malloc(newSquare.numPoints * sizeof(Vec2));
    newSquare.transformedV = malloc(newSquare.numPoints * sizeof(Vec2));
    newSquare.angle = angle;
    newSquare.scale = scale;
    newSquare.color = color;
    
    newSquare.points[0].x = -side_length/2.0;
    newSquare.points[0].y = -side_length/2.0;
    newSquare.points[1].x = side_length/2.0;
    newSquare.points[1].y = -side_length/2.0;
    newSquare.points[2].x = side_length/2.0;
    newSquare.points[2].y = side_length/2.0;
    newSquare.points[3].x = -side_length/2.0;
    newSquare.points[3].y = side_length/2.0;
    
    memcpy(newSquare.transformedV, newSquare.points, sizeof(Vec2) * newSquare.numPoints);
    
    return newSquare;
}

Polygon makePolygon(float angle, int numPoints, float radius, float scale, uint8_t color)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numPoints;
    
    newPolygon.numPoints = numPoints;
    newPolygon.points = malloc(numPoints * sizeof(Vec2));
    newPolygon.transformedV = malloc(numPoints * sizeof(Vec2));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.scale = scale;
    newPolygon.color = color;
    
    while (i < newPolygon.numPoints)
    {
        newPolygon.points[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.points[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    memcpy(newPolygon.transformedV, newPolygon.points, sizeof(Vec2) * newPolygon.numPoints);
    
    return newPolygon;
}

Polygon makeShadedTriangle(float angle, int numPoints, float radius, float scale, uint8_t col_1, uint8_t col_2, uint8_t col_3)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numPoints;
    
    newPolygon.numPoints = numPoints;
    newPolygon.points = malloc(numPoints * sizeof(Vec2));
    newPolygon.transformedV = malloc(numPoints * sizeof(Vec2));
    newPolygon.vertexColors = malloc(3 * sizeof(uint8_t));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.scale = scale;
    
    while (i < newPolygon.numPoints)
    {
        newPolygon.points[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.points[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    newPolygon.vertexColors[0] = col_1;
    newPolygon.vertexColors[1] = col_2;
    newPolygon.vertexColors[2] = col_3;
    
    memcpy(newPolygon.transformedV, newPolygon.points, sizeof(Vec2) * newPolygon.numPoints);
    
    return newPolygon;
}

Polygon makeTexturedTriangle(float angle, int numPoints, float radius, float scale, Vec2 uv_1, Vec2 uv_2, Vec2 uv_3, Vec2_int center)
{
    Polygon newPolygon;
    char i = 0;
    float angle_step = RAD_360/numPoints;
    
    newPolygon.numPoints = numPoints;
    newPolygon.points = malloc(numPoints * sizeof(Vec2));
    newPolygon.transformedV = malloc(numPoints * sizeof(Vec2));
    newPolygon.UVCoords = malloc(3 * sizeof(Vec2));
    newPolygon.angle = angle;
    newPolygon.radius = radius;
    newPolygon.scale = scale;
    newPolygon.center.x = center.x;
    newPolygon.center.y = center.y;
    
    while (i < newPolygon.numPoints)
    {
        newPolygon.points[i].x = cos((angle_step + angle) * (i + 1)) * radius;
        newPolygon.points[i].y = sin((angle_step + angle) * (i + 1)) * radius;
        
        i++;
    }
    
    newPolygon.UVCoords[0] = uv_1;
    newPolygon.UVCoords[1] = uv_2;
    newPolygon.UVCoords[2] = uv_3;
    
    memcpy(newPolygon.transformedV, newPolygon.points, sizeof(Vec2) * newPolygon.numPoints);
    
    return newPolygon;
}

void createPolygons()
{
    poly_array[0] = makeSquare(0.0, 10.0, 1.0, 44);
    poly_array[1] = makePolygon(0.0, 3, 15.0, 1.0, 47);
    poly_array[2] = makePolygon(0.0, 5, 25.0, 1.0, 47);
    poly_array[3] = makePolygon(0.0, 5, 25.0, 1.0, 47);
    poly_array[4] = makePolygon(0.0, 3, 15.0, 1.0, 47);
    poly_array[5] = makeShadedTriangle(0.0, 3, 15.0, 1.0, 16, 23, 31);
}