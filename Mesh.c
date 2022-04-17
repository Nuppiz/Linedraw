#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Mesh- and wireframe-related functions */

Mesh2D CarMesh;
Texture CarTex;
Mesh3D Cube;

extern uint8_t far screen_buf [];

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
    carmesh->triangleVertices[1].UV.x = 31;
    carmesh->triangleVertices[1].UV.y = 0;
    carmesh->triangleVertices[2].pointID = 3;
    carmesh->triangleVertices[2].UV.x = 0;
    carmesh->triangleVertices[2].UV.y = 63;
    carmesh->triangleVertices[3].pointID = 1;
    carmesh->triangleVertices[3].UV.x = 31;
    carmesh->triangleVertices[3].UV.y = 0;
    carmesh->triangleVertices[4].pointID = 2;
    carmesh->triangleVertices[4].UV.x = 31;
    carmesh->triangleVertices[4].UV.y = 63;
    carmesh->triangleVertices[5].pointID = 3;
    carmesh->triangleVertices[5].UV.x = 0;
    carmesh->triangleVertices[5].UV.y = 63;
}

void MakeCube(Mesh3D* cube)
{
    cube->x_angle = 0.0;
    cube->y_angle = 0.0;
    cube->z_angle = 0.0;
    cube->scale = 1.0;
    cube->center.x = 159;
    cube->center.y = 99;

    cube->numPoints = 8;
    cube->points = malloc(cube->numPoints * sizeof(Vec3));
    cube->points[0].x = -CUBE_SIZE;
    cube->points[0].y = CUBE_SIZE;
    cube->points[0].z = -CUBE_SIZE;
    cube->points[1].x = -CUBE_SIZE;
    cube->points[1].y = -CUBE_SIZE;
    cube->points[1].z = -CUBE_SIZE;
    cube->points[2].x = CUBE_SIZE;
    cube->points[2].y = -CUBE_SIZE;
    cube->points[2].z = -CUBE_SIZE;
    cube->points[3].x = CUBE_SIZE;
    cube->points[3].y = CUBE_SIZE;
    cube->points[3].z = -CUBE_SIZE;
    cube->points[4].x = CUBE_SIZE;
    cube->points[4].y = CUBE_SIZE;
    cube->points[4].z = CUBE_SIZE;    
    cube->points[5].x = -CUBE_SIZE;
    cube->points[5].y = CUBE_SIZE;
    cube->points[5].z = CUBE_SIZE;
    cube->points[6].x = -CUBE_SIZE;
    cube->points[6].y = -CUBE_SIZE;
    cube->points[6].z = CUBE_SIZE;
    cube->points[7].x = CUBE_SIZE;
    cube->points[7].y = -CUBE_SIZE;
    cube->points[7].z = CUBE_SIZE;

    cube->numTriangles = 12;
    cube->triangleVertices = malloc((cube->numTriangles * 3) * sizeof(Vertex));

    cube->transformedP = malloc(cube->numPoints * sizeof(Vec3));
    memcpy(cube->transformedP, cube->points, sizeof(Vec3) * cube->numPoints);
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

void rotatePoints_X(int numPoints, Vec3* originalPoints, Vec3* rotatedPoints, float angle)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_y;
    float old_z;
    float new_y;
    float new_z;
    
    for (i = 0; i < numPoints; i++)
    {
        old_y = originalPoints[i].y;
        old_z = originalPoints[i].z;
        cos_angle = cos(angle);
        sin_angle = sin(angle);
        new_y = -(old_z * cos_angle - old_y * sin_angle); //* object->scale;
        new_z = (old_z * sin_angle + old_y * cos_angle); //* object->scale;
        rotatedPoints[i].y = new_y;
        rotatedPoints[i].z = new_z;
    }
}

void rotatePoints_Y(int numPoints, Vec3* originalPoints, Vec3* rotatedPoints, float angle)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_z;
    float new_x;
    float new_z;
    
    for (i = 0; i < numPoints; i++)
    {
        old_x = originalPoints[i].x;
        old_z = originalPoints[i].z;
        cos_angle = cos(angle);
        sin_angle = sin(angle);
        new_x = (old_x * cos_angle - old_z * sin_angle); //* object->scale;
        new_z = (old_x * sin_angle + old_z * cos_angle); //* object->scale;
        rotatedPoints[i].x = new_x;
        rotatedPoints[i].z = new_z;
    }
}

void rotatePoints_Z(int numPoints, Vec3* originalPoints, Vec3* rotatedPoints, float angle)
{
    char i;
    float cos_angle;
    float sin_angle;
    float old_x;
    float old_y;
    float new_x;
    float new_y;
    
    for (i = 0; i < numPoints; i++)
    {
        old_x = originalPoints[i].x;
        old_y = originalPoints[i].y;
        cos_angle = cos(angle);
        sin_angle = sin(angle);
        new_x = (old_x * cos_angle - old_y * sin_angle); //* object->scale;
        new_y = (old_x * sin_angle + old_y * cos_angle); //* object->scale;
        rotatedPoints[i].x = new_x;
        rotatedPoints[i].y = new_y;
    }
}

void applyObject3DRotation(Mesh3D* object)
{
    char X_axis_str [20];
    char Y_axis_str [20];
    char Z_axis_str [20];
    rotatePoints_X(object->numPoints, object->points, object->transformedP, object->x_angle);
    rotatePoints_Y(object->numPoints, object->transformedP, object->transformedP, object->y_angle);
    rotatePoints_Z(object->numPoints, object->transformedP, object->transformedP, object->z_angle);
    sprintf(X_axis_str, "X ANGLE: %f", object->x_angle);
    sprintf(Y_axis_str, "Y ANGLE: %f", object->y_angle);
    sprintf(Z_axis_str, "Z ANGLE: %f", object->z_angle);
    renderText(0, 30, X_axis_str, COLOR_WHITE);
    renderText(0, 40, Y_axis_str, COLOR_WHITE);
    renderText(0, 50, Z_axis_str, COLOR_WHITE);
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

void draw3DCube(Mesh3D* cube)
{
    int i = 0;
    Vec2_int p1, p2;
    char X_str [20];
    char Y_str [20];
    char Z_str [20];

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

    sprintf(X_str, "X: %f", cube->transformedP[0].x);
    sprintf(Y_str, "Y: %f", cube->transformedP[0].y);
    sprintf(Z_str, "Z: %f", cube->transformedP[0].z);
    renderText(0, 0, X_str, COLOR_WHITE);
    renderText(0, 10, Y_str, COLOR_WHITE);
    renderText(0, 20, Z_str, COLOR_WHITE);

    SET_PIXEL((int)(cube->center.x + cube->transformedP[0].x), (int)(cube->center.y + cube->transformedP[0].y), COLOR_RED);
}