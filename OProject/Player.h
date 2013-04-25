#pragma once

#include <GL/glew.h> 
#include <GL/freeglut.h> 

#include "VectorUtils3.h"
#include "World.h"

class Player
{
private:
	Point3D pos;
	Point3D look;
	GLfloat zspeed;
	World* w;

public:
	Player(void);
	Player(Point3D, Point3D);
	~Player(void);
	
	void zUpdate(void);
	mat4 getCamMatrix(void);

	void goForward(void);
	 
};

