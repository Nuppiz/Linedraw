#ifndef STRUCTS_H
#define STRUCTS_H

#include "Defines.h"

typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    int x, y;
} Vec2_int;

typedef struct
{
    float x, y, z;
} Vec3;

typedef struct
{
    Vec2 startpos;
    Vec2 endpos;
    uint8_t color;
} Line;

typedef struct
{
    Vec2_int startpos;
    Vec2_int endpos;
    uint8_t color;
} Line_int;

typedef struct
{
    int offset[SCREEN_HEIGHT];
    uint8_t color[SCREEN_HEIGHT];
    Vec2 texture[SCREEN_HEIGHT];
} Span;

typedef struct
{
    float angle;
    Vec2* points;
    Vec2* transformedV;
    int numPoints;
    float radius;
    float scale;
    uint8_t color;
    uint8_t* vertexColors;
    Vec2* UVCoords;
    Vec2_int center;
} Polygon;

typedef struct
{
    int pointID;
    Vec2 UV;
} Vertex;

typedef struct
{
    uint8_t* pixels;
    int height;
    int width;
    uint8_t transparent;
} Texture;

typedef struct
{
    float angle;
    float scale;
    int numPoints;
    Vec2* points;
    Vec2* transformedP;
    int numTriangles;
    Vertex* triangleVertices;
    Vec2_int center;
    Texture* texture;
} Mesh2D;

typedef struct
{
    uint8_t r,g,b;
} Color;

typedef struct
{
    Color colors[256];
} Palette;

typedef struct
{
    uint16_t p0;
    uint16_t p1;
    uint16_t p2;
    uint16_t uv0;
    uint16_t uv1;
    uint16_t uv2;
    Vec3 normal;
} Triangle;

typedef struct
{
    Triangle* triangles;
    int numTriangles;
    Texture* texture;
} TriangleGroup;

typedef struct
{
    float x_angle, y_angle, z_angle;
    float scale;
    int numPoints;
    Vec3* points;
    Vec3* transformedP;
    int numTriangles;
    Vertex* triangleVertices;
    Vec2* UVCoords;
    TriangleGroup* triangleGroups;
    int numTGs;
    Vec2_int center;
} Mesh3D;

enum TextureCount
{
    CAR,
    BEACH,
    BUSH,
    CONCRETE,
    GRASSSAND,
    TREES,
    WALL,
    NUM_TEXTURES
};

#endif