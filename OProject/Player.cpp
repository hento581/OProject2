#include "Player.h"
#include "World.h"


Player::Player(void)
{
}

Player::Player(Point3D p, Point3D l, World* world)
{
	this->pos = p;
	this->look = l;
	this->w = world;
}

Player::~Player(void)
{
}

void Player::heightUpdate(void)
{
	// this->zspeed = this->zspeed - T*g; //TODO: fixa konstanter samt kollisionshantering med mark.
	pos.y = pos.y + yspeed;
	look.y = look.y + yspeed;
	if(pos.y > w->findHeight(pos.x,pos.z))
		yspeed = yspeed - 0.01;

	if(pos.y <= w->findHeight(pos.x,pos.z))
	{
		look.y = look.y - pos.y + w->findHeight(pos.x,pos.z);
		pos.y = w->findHeight(pos.x,pos.z);
		yspeed = 0;
	}
}

void Player::jump(void)
{
	if(pos.y - w->findHeight(pos.x,pos.z) <0.1 && pos.y - w->findHeight(pos.x,pos.z) >-0.1)
		yspeed = 0.2f;
}

mat4 Player::getCamMatrix(void)
{
	Point3D up = vec3(0.0f, 1.0f, 0.0f);
	Point3D camDiff = vec3(0.0f, 2.0f, 0.0f); //TODO: Inte så snyggt... 2.0 konstant mm...
	return lookAtv(VectorAdd(pos,camDiff), VectorAdd(look,camDiff), up);
}

void Player::goForward(void)
{
	Point3D move = VectorSub(this->look, this->pos);
	Point3D normal = w->findNormal(pos.x,pos.z,w->getModel()->vertexArray);
	move.y = 0;
	move= Normalize(move);
	move = VectorAdd(move,ScalarMult(move,DotProduct(move,normal)));
	move = ScalarMult(move, 0.2f); //TODO: set variable instead of numerica value
	pos = VectorAdd(move, pos);
	look = VectorAdd(move, look);
}

void Player::goBackwards(void)
{
	Point3D move = VectorSub(this->pos, this->look);
	move.y = 0;
	move = ScalarMult(Normalize(move), 0.2f);
	this->pos = VectorAdd(move, this->pos);
	this->look = VectorAdd(move, this->look);
}

void Player::mouseLook(GLfloat xdiff,GLfloat ydiff)
{
Point3D upVec = vec3(0,1,0); //vector uppåt i planet
	
Point3D lookVec = Normalize(VectorSub(look, pos)); //vector till "synplanet"
Point3D rightVec = Normalize(CrossProduct(upVec, lookVec));  //vector till höger i synplanet
upVec = Normalize(CrossProduct(rightVec, lookVec)); //ny vector uppåt i synplanet
look = VectorAdd(pos, lookVec);
look = VectorAdd(look, ScalarMult(rightVec, xdiff));
look = VectorAdd(look, ScalarMult(upVec, ydiff));
}
