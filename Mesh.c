#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Mesh- and wireframe-related functions */

Mesh2D CarMesh;
Texture CarTex;
Mesh3D Cube;

Texture BeaWater;
Texture Bush;
Texture Concrete;
Texture GrasSand;
Texture Trees;
Texture Wall;

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

    cube->transformedP = malloc(cube->numPoints * sizeof(Vec3));
    memcpy(cube->transformedP, cube->points, sizeof(Vec3) * cube->numPoints);

    cube->numTriangles = 12;
    cube->numFaces = 6;
    cube->triangleVertices = malloc((cube->numTriangles * 3) * sizeof(Vertex));
    cube->faces = malloc((cube->numFaces) * sizeof(Face));
    // back face
    cube->triangleVertices[0].pointID = 1;
    cube->triangleVertices[0].UV.x = 0;
    cube->triangleVertices[0].UV.y = 0;
    cube->triangleVertices[1].pointID = 2;
    cube->triangleVertices[1].UV.x = 31;
    cube->triangleVertices[1].UV.y = 0;
    cube->triangleVertices[2].pointID = 3;
    cube->triangleVertices[2].UV.x = 31;
    cube->triangleVertices[2].UV.y = 31;
    cube->triangleVertices[3].pointID = 3;
    cube->triangleVertices[3].UV.x = 31;
    cube->triangleVertices[3].UV.y = 31;
    cube->triangleVertices[4].pointID = 0;
    cube->triangleVertices[4].UV.x = 0;
    cube->triangleVertices[4].UV.y = 31;
    cube->triangleVertices[5].pointID = 1;
    cube->triangleVertices[5].UV.x = 0;
    cube->triangleVertices[5].UV.y = 0;
    cube->faces[0].texture = &BeaWater;
    // right face
    cube->triangleVertices[6].pointID = 2;
    cube->triangleVertices[6].UV.x = 31;
    cube->triangleVertices[6].UV.y = 0;
    cube->triangleVertices[7].pointID = 3;
    cube->triangleVertices[7].UV.x = 31;
    cube->triangleVertices[7].UV.y = 31;
    cube->triangleVertices[8].pointID = 4;
    cube->triangleVertices[8].UV.x = 0;
    cube->triangleVertices[8].UV.y = 31;
    cube->triangleVertices[9].pointID = 4;
    cube->triangleVertices[9].UV.x = 0;
    cube->triangleVertices[9].UV.y = 31;
    cube->triangleVertices[10].pointID = 7;
    cube->triangleVertices[10].UV.x = 0;
    cube->triangleVertices[10].UV.y = 0;
    cube->triangleVertices[11].pointID = 2;
    cube->triangleVertices[11].UV.x = 31;
    cube->triangleVertices[11].UV.y = 0;
    cube->faces[1].texture = &Bush;
    // front face
    cube->triangleVertices[12].pointID = 4;
    cube->triangleVertices[12].UV.x = 31;
    cube->triangleVertices[12].UV.y = 31;
    cube->triangleVertices[13].pointID = 5;
    cube->triangleVertices[13].UV.x = 0;
    cube->triangleVertices[13].UV.y = 31;
    cube->triangleVertices[14].pointID = 6;
    cube->triangleVertices[14].UV.x = 0;
    cube->triangleVertices[14].UV.y = 0;
    cube->triangleVertices[15].pointID = 6;
    cube->triangleVertices[15].UV.x = 0;
    cube->triangleVertices[15].UV.y = 0;
    cube->triangleVertices[16].pointID = 7;
    cube->triangleVertices[16].UV.x = 31;
    cube->triangleVertices[16].UV.y = 0;
    cube->triangleVertices[17].pointID = 4;
    cube->triangleVertices[17].UV.x = 31;
    cube->triangleVertices[17].UV.y = 31;
    cube->faces[2].texture = &Concrete;
    // left face
    cube->triangleVertices[18].pointID = 6;
    cube->triangleVertices[18].UV.x = 31;
    cube->triangleVertices[18].UV.y = 0;
    cube->triangleVertices[19].pointID = 1;
    cube->triangleVertices[19].UV.x = 0;
    cube->triangleVertices[19].UV.y = 0;
    cube->triangleVertices[20].pointID = 0;
    cube->triangleVertices[20].UV.x = 0;
    cube->triangleVertices[20].UV.y = 31;
    cube->triangleVertices[21].pointID = 0;
    cube->triangleVertices[21].UV.x = 0;
    cube->triangleVertices[21].UV.y = 31;
    cube->triangleVertices[22].pointID = 5;
    cube->triangleVertices[22].UV.x = 31;
    cube->triangleVertices[22].UV.y = 31;
    cube->triangleVertices[23].pointID = 6;
    cube->triangleVertices[23].UV.x = 31;
    cube->triangleVertices[23].UV.y = 0;
    cube->faces[3].texture = &GrasSand;
    // top face
    cube->triangleVertices[24].pointID = 6;
    cube->triangleVertices[24].UV.x = 0;
    cube->triangleVertices[24].UV.y = 31;
    cube->triangleVertices[25].pointID = 1;
    cube->triangleVertices[25].UV.x = 0;
    cube->triangleVertices[25].UV.y = 0;
    cube->triangleVertices[26].pointID = 2;
    cube->triangleVertices[26].UV.x = 31;
    cube->triangleVertices[26].UV.y = 0;
    cube->triangleVertices[27].pointID = 2;
    cube->triangleVertices[27].UV.x = 31;
    cube->triangleVertices[27].UV.y = 0;
    cube->triangleVertices[28].pointID = 7;
    cube->triangleVertices[28].UV.x = 31;
    cube->triangleVertices[28].UV.y = 31;
    cube->triangleVertices[29].pointID = 6;
    cube->triangleVertices[29].UV.x = 0;
    cube->triangleVertices[29].UV.y = 31;
    cube->faces[4].texture = &Trees;
    // bottom face
    cube->triangleVertices[30].pointID = 4;
    cube->triangleVertices[30].UV.x = 31;
    cube->triangleVertices[30].UV.y = 0;
    cube->triangleVertices[31].pointID = 5;
    cube->triangleVertices[31].UV.x = 0;
    cube->triangleVertices[31].UV.y = 0;
    cube->triangleVertices[32].pointID = 0;
    cube->triangleVertices[32].UV.x = 0;
    cube->triangleVertices[32].UV.y = 31;
    cube->triangleVertices[33].pointID = 0;
    cube->triangleVertices[33].UV.x = 0;
    cube->triangleVertices[33].UV.y = 31;
    cube->triangleVertices[34].pointID = 3;
    cube->triangleVertices[34].UV.x = 31;
    cube->triangleVertices[34].UV.y = 31;
    cube->triangleVertices[35].pointID = 4;
    cube->triangleVertices[35].UV.x = 31;
    cube->triangleVertices[35].UV.y = 0;
    cube->faces[5].texture = &Wall;
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
        new_y = -(old_z * cos_angle - old_y * sin_angle);
        new_z = (old_z * sin_angle + old_y * cos_angle);
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
        new_z = (old_x * sin_angle + old_z * cos_angle);
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
        new_x = (old_x * cos_angle - old_y * sin_angle);
        new_y = (old_x * sin_angle + old_y * cos_angle);
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
    int face_i = 0;
    int triangle_i = 0;

    while (face_i < cube->numFaces)
    {
        while (i < 2)
        {
            draw3DCubeTriangleTex(cube, triangle_i, face_i);
            i++;
            triangle_i++;
        }
        i = 0;
        face_i++;
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