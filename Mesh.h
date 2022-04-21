#ifndef MESH_H
#define MESH_H

#include "Structs.h"

void MakeCarMesh(Mesh2D* carmesh);
void MakeCube(Mesh3D* cube);
void updateMesh(Mesh2D* mesh);
void updateMesh3D(Mesh3D* object);
void draw2DMesh(Mesh2D* mesh);
void drawFilled3DCube(Mesh3D* cube);
void draw3DCube(Mesh3D* cube);

#endif
