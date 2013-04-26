#include "Player.h"
#include "World.h"


Player::Player(void)
{
}

Player::Player(Point3D p, Point3D l)
{
	this->pos = p;
	this->look = l;
	this->zspeed = 0.0f;
}

Player::~Player(void)
{
}

void Player::zUpdate(void)
{
	// this->zpos = this->zpos + this->zspeed*T;
	// this->zspeed = this->zspeed - T*g; //TODO: fixa konstanter samt kollisionshantering med mark.

}

mat4 Player::getCamMatrix(void)
{
	Point3D up = vec3(0.0f, 1.0f, 0.0f);
	Point3D camDiff = vec3(0.0f, 0.0f, 2.0f);
	return lookAtv(VectorAdd(camDiff, this->pos), VectorAdd(camDiff, this->pos), up);
}

void Player::goForward(void)
{
	Point3D temp = VectorSub(this->look, this->pos);
	temp = Normalize(temp);
	temp = ScalarMult(temp, 0.3f);
	this->pos = VectorAdd(temp, this->pos);
	this->look = VectorAdd(temp, this->pos);
	this->pos.z = w->findHeight(this->pos.x,this->pos.x);
}

