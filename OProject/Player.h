#pragma once

#include <GL/glew.h> 
#include <GL/freeglut.h> 

class Player
{
private:
	GLfloat xpos;
	GLfloat ypos;
	GLfloat zpos;
	GLfloat zspeed;

public:
	Player(void);
	~Player(void);
	
	void zUpdate(void);
};

