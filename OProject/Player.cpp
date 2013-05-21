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
	this->oldSpeed= vec3(0,0,0);
	this->hitTree = 0;
	this->turbo = 0.0;
	this->gravity = 1.0;
	this->nextControl = vec3(110,0,72);
	this->punshedControls=0;
}

Player::~Player(void)
{
}

/*
 Returns true if the player is in the proximity of the next control
*/
bool Player::isNextControl()
{
	Point3D tempPos = pos;
	tempPos.y = 0;
	if(Norm(tempPos-nextControl)<0.5) return true;
	
	return false;
}

/*
	Sets the position for the next control
*/
void Player::setNextControl(vec3 newNextControl)
{
	nextControl = newNextControl;
	punshedControls++;
}

/*
 Returns the number of controls the player has punshed
*/
int Player::getPunshedControls()
{
	return punshedControls;
}

/*
 Updats the players height-position
*/
void Player::heightUpdate(void)
{
	Point3D oldPos = pos;
	Point3D oldLook = look;
	pos.y = pos.y + yspeed;
	look.y = look.y + yspeed;
	if(pos.y > w->findHeight(pos.x,pos.z))
	{
		pos = VectorAdd(oldSpeed, pos);
		look = VectorAdd(oldSpeed, look);
		yspeed = yspeed - 0.02*gravity;
		if(w->findHeight(pos.x,pos.z)<0.3 || pos.x <0 || pos.x >255 || pos.z<0 || pos.z>255)
		{
			pos = oldPos;
			look = oldLook;
			oldSpeed = vec3(0,0,0);
		}

	}

	if(pos.y <= w->findHeight(pos.x,pos.z))
	{
		look.y = look.y - pos.y + w->findHeight(pos.x,pos.z);
		pos.y = w->findHeight(pos.x,pos.z);
		yspeed = 0;
	}
}

/*
	Tells the player to jump
*/
void Player::jump(void)
{
	if(pos.y - w->findHeight(pos.x,pos.z) <0.2 && pos.y - w->findHeight(pos.x,pos.z) >-0.2)
		yspeed = 0.3f;
}

/*
 Returns the cam matrix
*/
mat4 Player::getCamMatrix(void)
{
	Point3D up = vec3(0.0f, 1.0f, 0.0f);
	Point3D camDiff = vec3(0.0f, 2.0f, 0.0f); 
	return lookAtv(VectorAdd(pos,camDiff), VectorAdd(look,camDiff), up);
}

/*
 Walks the player forward
*/
void Player::goForward(void)
{
	if(pos.y - w->findHeight(pos.x,pos.z) <0.1 && pos.y - w->findHeight(pos.x,pos.z) >-0.1)
	{
		GLfloat h = w->findHeight(pos.x,pos.z);
		Point3D move = VectorSub(this->look, this->pos);
		Point3D oldPos = pos;
		Point3D oldLook = look;
		move.y = 0;
		move= Normalize(move);
		
			if(this->hitTree == 0)
			{
				for(int i = 1; i<=10; i++)
				{
						pos = VectorAdd(ScalarMult(move,0.01+turbo), pos);
						look = VectorAdd(ScalarMult(move,0.01+turbo), look);
						if(w->findHeight(pos.x,pos.z) >= h+0.03)
							i=30;
				}			
			}		
			else
			{
				this->hitTree--;

			}

			for(int i = 1; i<=10; i++)
				{
						pos = VectorAdd(ScalarMult(move,0.01+turbo), pos);
						look = VectorAdd(ScalarMult(move,0.01+turbo), look);
						if(w->findHeight(pos.x,pos.z) >= h+0.01)
							i=30;
				}
			for(int i = 1; i<=5; i++)
				{
						pos = VectorAdd(ScalarMult(move,0.01+turbo), pos);
						look = VectorAdd(ScalarMult(move,0.01+turbo), look);
						if(w->findHeight(pos.x,pos.z) >= h)
							i=30;
				}
			oldSpeed = VectorSub(pos,oldPos);
			look.y = look.y - pos.y + w->findHeight(pos.x,pos.z);
			pos.y = w->findHeight(pos.x,pos.z);
			if(pos.y <0.3 || pos.x <0 || pos.x >255 || pos.z<0 || pos.z>255)
			{
				pos = oldPos;
				look = oldLook;
				oldSpeed = vec3(0,0,0);
			}		
	}
}

/*
 Sets the oldSpeed of player to {0,0,0}
*/
void Player::stop(void)
{
	if(pos.y - w->findHeight(pos.x,pos.z) <0.1 && pos.y - w->findHeight(pos.x,pos.z) >-0.1)
	{
		oldSpeed = vec3(0,0,0);
	}
}

/*
	Walks the player backwards, mostly for debugging
*/
void Player::goBackwards(void)
{
	Point3D move = VectorSub(this->pos, this->look);
	move.y = 0;
	move = ScalarMult(Normalize(move), 0.2f);
	this->pos = VectorAdd(move, this->pos);
	this->look = VectorAdd(move, this->look);
}

/*
 Looks around according to the mouse
*/
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

/*
 Sets the oldSpeed
*/
void Player::setOldSpeed(Point3D newOldSpeed)
{
	oldSpeed = newOldSpeed;

}

void Player::playerHitTree(void){

	this->hitTree = 5;
}

void Player::setTurbo(GLfloat tempo)
{

	this->turbo = tempo;
}

