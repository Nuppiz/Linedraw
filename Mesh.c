#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Mesh- and wireframe-related functions */

Mesh2D CarMesh;
Mesh3D Cube;

extern uint8_t far screen_buf [];
extern Texture Textures[];

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

    carmesh->texture = &Textures[CAR];
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

Side MakeCubeSide(uint16_t p0, uint16_t p1, uint16_t p2, uint16_t p3)
{
    Side newSide;

    newSide.triangles[0].p0 = p0;
    newSide.triangles[0].uv0 = 0;
    newSide.triangles[0].p1 = p1;
    newSide.triangles[0].uv1 = 1;
    newSide.triangles[0].p2 = p2;
    newSide.triangles[0].uv2 = 2;
    newSide.triangles[1].p0 = p2;
    newSide.triangles[1].uv0 = 2;
    newSide.triangles[1].p1 = p3;
    newSide.triangles[1].uv1 = 3;
    newSide.triangles[1].p2 = p0;
    newSide.triangles[1].uv2 = 0;

    return newSide;
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

    cube->UVCoords = malloc(4 * sizeof(Vec2));
    cube->UVCoords[0].x = 0;
    cube->UVCoords[0].y = 31;
    cube->UVCoords[1].x = 0;
    cube->UVCoords[1].y = 0;
    cube->UVCoords[2].x = 31;
    cube->UVCoords[2].y = 0;
    cube->UVCoords[3].x = 31;
    cube->UVCoords[3].y = 31;

    cube->transformedP = malloc(cube->numPoints * sizeof(Vec3));
    memcpy(cube->transformedP, cube->points, sizeof(Vec3) * cube->numPoints);

    cube->numTriangles = 12;
    cube->numSides = 6;
    cube->triangleVertices = malloc((cube->numTriangles * 3) * sizeof(Vertex));
    cube->sides = malloc((cube->numSides) * sizeof(Side));

    cube->sides[0] = MakeCubeSide(5, 6, 7, 4);
    cube->sides[1] = MakeCubeSide(0, 1, 6, 5);
    cube->sides[2] = MakeCubeSide(3, 2, 1, 0);
    cube->sides[3] = MakeCubeSide(4, 7, 2, 3);
    cube->sides[4] = MakeCubeSide(6, 1, 2, 7);
    cube->sides[5] = MakeCubeSide(0, 5, 4, 3);

    cube->sides[0].texture = &Textures[BEACH];
    cube->sides[1].texture = &Textures[BUSH];
    cube->sides[2].texture = &Textures[CONCRETE];
    cube->sides[3].texture = &Textures[GRASSSAND];
    cube->sides[4].texture = &Textures[TREES];
    cube->sides[5].texture = &Textures[WALL];
}

void updateMesh(Mesh2D* mesh)
{
    int i;
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
    int i;
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
        new_y = (old_y * cos_angle - old_z * sin_angle);
        new_z = (old_z * cos_angle + old_y * sin_angle);
        rotatedPoints[i].y = new_y;
        rotatedPoints[i].z = new_z;
    }
}

void rotatePoints_Y(int numPoints, Vec3* originalPoints, Vec3* rotatedPoints, float angle)
{
    int i;
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
        new_x = (old_x * cos_angle - old_z * sin_angle);
        new_z = (old_z * cos_angle + old_x * sin_angle);
        rotatedPoints[i].x = new_x;
        rotatedPoints[i].z = new_z;
    }
}

void rotatePoints_Z(int numPoints, Vec3* originalPoints, Vec3* rotatedPoints, float angle)
{
    int i;
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
        new_x = (old_x * cos_angle + old_y * sin_angle);
        new_y = (old_y * cos_angle - old_x * sin_angle);
        rotatedPoints[i].x = new_x;
        rotatedPoints[i].y = new_y;
    }
}

void scalePoints(int numPoints, Vec3* originalPoints, Vec3* scaledPoints, float scale)
{
    int i;
    float old_x;
    float old_y;
    float old_z;
    float new_x;
    float new_y;
    float new_z;
    
    for (i = 0; i < numPoints; i++)
    {
        old_x = originalPoints[i].x;
        old_y = originalPoints[i].y;
        old_z = originalPoints[i].z;
        new_x = old_x * scale;
        new_y = old_y * scale;
        new_z = old_z * scale;
        scaledPoints[i].x = new_x;
        scaledPoints[i].y = new_y;
        scaledPoints[i].z = new_z;
    }
}

void updateMesh3D(Mesh3D* object)
{
    char X_axis_str [20];
    char Y_axis_str [20];
    char Z_axis_str [20];
    memcpy(object->transformedP, object->points, sizeof(Vec3) * object->numPoints);
    rotatePoints_X(object->numPoints, object->transformedP, object->transformedP, object->x_angle);
    rotatePoints_Y(object->numPoints, object->transformedP, object->transformedP, object->y_angle);
    rotatePoints_Z(object->numPoints, object->transformedP, object->transformedP, object->z_angle);
    scalePoints(object->numPoints, object->transformedP, object->transformedP, object->scale);
    sprintf(X_axis_str, "X ANGLE: %f", object->x_angle);
    sprintf(Y_axis_str, "Y ANGLE: %f", object->y_angle);
    sprintf(Z_axis_str, "Z ANGLE: %f", object->z_angle);
    /*renderText(0, 30, X_axis_str, COLOR_WHITE);
    renderText(0, 40, Y_axis_str, COLOR_WHITE);
    renderText(0, 50, Z_axis_str, COLOR_WHITE);*/
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

void drawFilled3DCube(Mesh3D* cube)
{   
    int i = 0;
    uint8_t color = 192;

    while (i < cube->numTriangles)
    {
        draw3DCubeTriangle(cube, i, color);
        i++;
        color++;
    }
}

void drawTextured3DCube(Mesh3D* cube)
{   
    int i = 0;
    int side_i = 0;
    int triangle_i = 0;

    while (side_i < cube->numSides)
    {
        while (i < 2)
        {
            draw3DCubeTriangleTex(cube, triangle_i, side_i);
            i++;
            triangle_i++;
        }
        i = 0;
        side_i++;
    }
}

void drawTextured3DCubeAlt(Mesh3D* cube)
{   
    int side_i = 0;
    int triangle_i = 0;

    while (side_i < cube->numSides)
    {
        while (triangle_i < 2)
        {
            draw3DCubeTriangleTexAlt(cube, triangle_i, side_i);
            triangle_i++;
        }
        triangle_i = 0;
        side_i++;
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
        drawLineInt(p1, p2, COLOR_PURPLE);
        i++;
    }
    p1.x = cube->center.x + cube->transformedP[0].x;
    p1.y = cube->center.y + cube->transformedP[0].y;
    p2.x = cube->center.x + cube->transformedP[3].x;
    p2.y = cube->center.y + cube->transformedP[3].y;
    drawLineInt(p1, p2, COLOR_PURPLE);
    p1.x = cube->center.x + cube->transformedP[0].x;
    p1.y = cube->center.y + cube->transformedP[0].y;
    p2.x = cube->center.x + cube->transformedP[5].x;
    p2.y = cube->center.y + cube->transformedP[5].y;
    drawLineInt(p1, p2, COLOR_PURPLE);
    p1.x = cube->center.x + cube->transformedP[1].x;
    p1.y = cube->center.y + cube->transformedP[1].y;
    p2.x = cube->center.x + cube->transformedP[6].x;
    p2.y = cube->center.y + cube->transformedP[6].y;
    drawLineInt(p1, p2, COLOR_PURPLE);
    p1.x = cube->center.x + cube->transformedP[2].x;
    p1.y = cube->center.y + cube->transformedP[2].y;
    p2.x = cube->center.x + cube->transformedP[7].x;
    p2.y = cube->center.y + cube->transformedP[7].y;
    drawLineInt(p1, p2, COLOR_PURPLE);
    p1.x = cube->center.x + cube->transformedP[4].x;
    p1.y = cube->center.y + cube->transformedP[4].y;
    p2.x = cube->center.x + cube->transformedP[7].x;
    p2.y = cube->center.y + cube->transformedP[7].y;
    drawLineInt(p1, p2, COLOR_PURPLE);

    sprintf(X_str, "X: %f", cube->transformedP[0].x);
    sprintf(Y_str, "Y: %f", cube->transformedP[0].y);
    sprintf(Z_str, "Z: %f", cube->transformedP[0].z);
    renderText(0, 0, X_str, COLOR_WHITE);
    renderText(0, 10, Y_str, COLOR_WHITE);
    renderText(0, 20, Z_str, COLOR_WHITE);

    //SET_PIXEL((int)(cube->center.x + cube->transformedP[0].x), (int)(cube->center.y + cube->transformedP[0].y), COLOR_RED);
}