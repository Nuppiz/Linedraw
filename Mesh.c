#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Mesh- and wireframe-related functions */

Mesh2D CarMesh;
Texture CarTex;
Object3D Cube;

void MakeCarMesh(Mesh2D* carmesh)
{
    carmesh->angle = 0.0;
    carmesh->scale = 1.0;
    carmesh->center.x = 159;
    carmesh->center.y = 99;

    carmesh->numPoints = 4;
    carmesh->points = malloc(carmesh->numPoints * sizeof(Vec2));
    carmesh->points[0].x = -32;
    carmesh->points[0].y = -64;
    carmesh->points[1].x = 32;
    carmesh->points[1].y = -64;
    carmesh->points[2].x = 32;
    carmesh->points[2].y = 64;
    carmesh->points[3].x = -32;
    carmesh->points[3].y = 64;

    carmesh->transformedP = malloc(carmesh->numPoints * sizeof(Vec2));
    memcpy(carmesh->transformedP, carmesh->points, sizeof(Vec2) * carmesh->numPoints);

    carmesh->texture = &CarTex;
    carmesh->numTriangles = 2;

    carmesh->triangleVertices = malloc((carmesh->numTriangles * 3) * sizeof(Vertex));
    carmesh->triangleVertices[0].pointID = 0;
    carmesh->triangleVertices[0].UV.x = 0;
    carmesh->triangleVertices[0].UV.y = 0;
    carmesh->triangleVertices[1].pointID = 1;
    carmesh->triangleVertices[1].UV.x = 32;
    carmesh->triangleVertices[1].UV.y = 0;
    carmesh->triangleVertices[2].pointID = 3;
    carmesh->triangleVertices[2].UV.x = 0;
    carmesh->triangleVertices[2].UV.y = 64;
    carmesh->triangleVertices[3].pointID = 1;
    carmesh->triangleVertices[3].UV.x = 32;
    carmesh->triangleVertices[3].UV.y = 0;
    carmesh->triangleVertices[4].pointID = 2;
    carmesh->triangleVertices[4].UV.x = 32;
    carmesh->triangleVertices[4].UV.y = 64;
    carmesh->triangleVertices[5].pointID = 3;
    carmesh->triangleVertices[5].UV.x = 0;
    carmesh->triangleVertices[5].UV.y = 64;
}

void MakeCube(Object3D* cube)
{
    cube->angle = 0.0;
    cube->scale = 1.0;
    cube->center.x = 159;
    cube->center.y = 99;

    cube->numPoints = 8;
    cube->points = malloc(cube->numPoints * sizeof(Vec2));
    cube->points[0].x = -8;
    cube->points[0].y = 8;
    cube->points[1].x = -8;
    cube->points[1].y = -24;
    cube->points[2].x = 24;
    cube->points[2].y = -24;
    cube->points[3].x = 24;
    cube->points[3].y = 8;
    cube->points[4].x = 16;
    cube->points[4].y = 16;    
    cube->points[5].x = -16;
    cube->points[5].y = 16;
    cube->points[6].x = -16;
    cube->points[6].y = -16;
    cube->points[7].x = 16;
    cube->points[7].y = -16;

    cube->numTriangles = 12;
    cube->triangleVertices = malloc((cube->numTriangles * 3) * sizeof(Vertex));

    cube->transformedP = malloc(cube->numPoints * sizeof(Vec2));
    memcpy(cube->transformedP, cube->points, sizeof(Vec2) * cube->numPoints);
}

void updateMesh(Mesh2D* mesh)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_y;
    float new_x;
    float new_y;
    
    for (i = 0; i < mesh->numPoints; i++)
    {
        old_x = mesh->points[i].x;
        old_y = mesh->points[i].y;
        cos_angle = cos(mesh->angle);
        sin_angle = sin(mesh->angle);
        new_x = (old_x * cos_angle - old_y * sin_angle) * mesh->scale;
        new_y = (old_x * sin_angle + old_y * cos_angle) * mesh->scale;
        mesh->transformedP[i].x = new_x;
        mesh->transformedP[i].y = new_y;
    }
}

void updateObject(Object3D* object)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_y;
    float new_x;
    float new_y;
    
    for (i = 0; i < object->numPoints; i++)
    {
        old_x = object->points[i].x;
        old_y = object->points[i].y;
        cos_angle = cos(object->angle);
        sin_angle = sin(object->angle);
        new_x = (old_x * cos_angle - old_y * sin_angle) * object->scale;
        new_y = (old_x * sin_angle + old_y * cos_angle) * object->scale;
        object->transformedP[i].x = new_x;
        object->transformedP[i].y = new_y;
    }
}

void draw2DMesh(Mesh2D* mesh)
{   
    int i = 0;

    while (i < mesh->numTriangles)
    {
        draw2DMeshTriangle(mesh, i);
        i++;
    }
}

void draw3DCube(Object3D* cube)
{
    int i = 0;
    Vec2_int p1, p2;

    while (i < cube->numPoints - 1)
    {
        p1.x = cube->center.x + cube->transformedP[i].x;
        p1.y = cube->center.y + cube->transformedP[i].y;
        p2.x = cube->center.x + cube->transformedP[i + 1].x;
        p2.y = cube->center.y + cube->transformedP[i + 1].y;
        drawLineInt(p1, p2, COLOR_WHITE);
        i++;
    }
    p1.x = cube->center.x + cube->transformedP[0].x;
    p1.y = cube->center.y + cube->transformedP[0].y;
    p2.x = cube->center.x + cube->transformedP[3].x;
    p2.y = cube->center.y + cube->transformedP[3].y;
    drawLineInt(p1, p2, COLOR_WHITE);
    p1.x = cube->center.x + cube->transformedP[0].x;
    p1.y = cube->center.y + cube->transformedP[0].y;
    p2.x = cube->center.x + cube->transformedP[5].x;
    p2.y = cube->center.y + cube->transformedP[5].y;
    drawLineInt(p1, p2, COLOR_WHITE);
    p1.x = cube->center.x + cube->transformedP[1].x;
    p1.y = cube->center.y + cube->transformedP[1].y;
    p2.x = cube->center.x + cube->transformedP[6].x;
    p2.y = cube->center.y + cube->transformedP[6].y;
    drawLineInt(p1, p2, COLOR_WHITE);
    p1.x = cube->center.x + cube->transformedP[2].x;
    p1.y = cube->center.y + cube->transformedP[2].y;
    p2.x = cube->center.x + cube->transformedP[7].x;
    p2.y = cube->center.y + cube->transformedP[7].y;
    drawLineInt(p1, p2, COLOR_WHITE);
    p1.x = cube->center.x + cube->transformedP[4].x;
    p1.y = cube->center.y + cube->transformedP[4].y;
    p2.x = cube->center.x + cube->transformedP[7].x;
    p2.y = cube->center.y + cube->transformedP[7].y;
    drawLineInt(p1, p2, COLOR_WHITE);
}