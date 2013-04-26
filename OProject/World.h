#pragma once

#include <GL/glew.h> 
#include <GL/freeglut.h> 
#include "VectorUtils3.h"
#include "LoadTGA2.h"
#include "loadobj.h"


class World
{
private:
	TextureData* tex;
public:
	World(void);
	World(TextureData*);
	~World(void);
	TextureData* getTex(void){return tex;}
	Point3D findNormal(int, int, GLfloat*);
	Model* GenerateTerrain();
	GLfloat interpolate(Point3D, Point3D, Point3D, Point3D);
	int insideTriangle(Point3D, Point3D, Point3D, Point3D);
	GLfloat findHeight(GLfloat, GLfloat); 
};

