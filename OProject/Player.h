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
	GLfloat yspeed;
	Point3D oldSpeed;
	World* w;
	int hitTree;
	GLfloat turbo;
	GLfloat gravity;
	Point3D nextControl;
	int punshedControls;

public:
	Player(void);
	Player(Point3D, Point3D, World*);
	~Player(void);
	Point3D getPos(void){return pos;}
	Point3D getLook(void){return look;}
	void setLook(Point3D l){look = l;}
	void setPos(Point3D p){pos = p;}
	bool isNextControl();
	void setNextControl(vec3);
	int getPunshedControls();
	void heightUpdate(void);
	mat4 getCamMatrix(void);
	void mouseLook(GLfloat,GLfloat);

	void goForward(void);
	void jump(void);
	void goBackwards(void);
	void stop(void);
	void setOldSpeed(Point3D);
	void playerHitTree(void);
	void setTurbo(GLfloat);

};

