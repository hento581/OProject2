#include <GL/glew.h> 
#include <GL/freeglut.h> 

#include "GL_utilities.h" 
#include <math.h>
#include "loadobj.h"
#include "LoadTGA2.h"
#include "VectorUtils3.h"
#include "Player.h"
#include "World.h"


#define near 1.0
 #define far 200.0
 #define right 0.5
 #define left -0.5
 #define top 0.5
 #define bottom -0.5

mat4 projectionMatrix;

//För keyboard
bool wIsDown = false;
bool sIsDown = false;
bool aIsDown = false;
bool dIsDown = false;



GLfloat jump = 0.0;

//För gravitation
GLfloat lastHeight = -999.0;
GLfloat gravSpeed = 0.3;
GLfloat gravitation = 0.0982/100.0;
GLfloat inAirTime = 0.0;
bool inAir = false;

//För jump
bool jumping = false;
GLfloat jumpSpeed = 1.0/5.0;
GLfloat jumpTime = 0.0;
GLfloat maxJumpTime = 15.0;

//För musen...
Point3D p = vec3(0, 5, 8);
Point3D l = vec3(2, 0, 2);
Point3D test;
Point3D temp;
Point3D temp2;
Point3D temp3;
Point3D camUp = vec3(0,1,0);
GLfloat screenX = 800.0;
GLfloat screenY = 600.0;
GLfloat oldx = 0;
GLfloat oldy = 0;
GLfloat gx = 10;
GLfloat gz = 10;

//Skapa spelare
Player* player = new Player;

//Skpa world
World* world = new World;

GLuint program;

GLfloat vertices[] = { -0.5f,-0.5f,0.0f,
						-0.5f,0.5f,0.0f,
						0.5f,-0.5f,0.0f };

// vertex array object
unsigned int vertexArrayObjID;

// vertex array object
Model *m, *m2, *tm;
// Reference to shader program

GLuint tex1, tex2,tex3,tex4;
TextureData ttex; // terrain


void keyboardFunction (unsigned char key, int xmouse, int ymouse)
{	
	switch (key){
		case 'w':
			wIsDown = true;
		break;
		case 's':
			sIsDown = true;
		break;
		case 'a':
			aIsDown = true;
		break;
		case 'd':
			dIsDown = true;
		break;	 
		case 'q':
			exit(0);
		break;
		case ' ':
			jumping = true;
			if(!inAir){
				inAir =true;
				inAirTime=0.0;
			}
		break;
		default:
         break;
	}
	
}


void keyboardUpFunction (unsigned char key, int xmouse, int ymouse)
{	
	switch (key){
		case 'w':
			wIsDown = false;
		break;
		case 's':
			sIsDown = false;
		break;
		case 'a':
			aIsDown = false;
		break;
		case 'd':
			dIsDown = false;
		break; 
		default:
         break;
	}

}

void keyboardSpecFunction (int key, int xmouse, int ymouse)
{	
	switch (key){
		

		default:
         break;
	}
	
}


void keyboardSpecUpFunction (int key, int xmouse, int ymouse)
{	
	switch (key){
		
		default:
         break;
	}
}


void mouse(int x, int y)
{
	GLfloat  relX = (screenX/2 - x)/screenX;
	GLfloat  relY = (screenY/2 - y)/screenY;
	GLfloat xdiff = relX;
	GLfloat ydiff = -relY;
	
	player->mouseLook(xdiff,ydiff);		
	
	if(relX != 0 || relY != 0)
	glutWarpPointer(screenX/2,screenY/2);
	
}


void init(void)
{

	GLenum err = glewInit();

	//glutReshapeWindow(screenX,screenY);
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);


	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 100.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	LoadTGATextureSimple("mygrass.tga", &tex1);
	LoadTGATextureSimple("newterrain.tga", &tex2);
	LoadTGATextureSimple("ok1.tga", &tex3);
	LoadTGATextureSimple("water0hk.tga", &tex4);
	
// Load terrain data
	
	LoadTGATexture("fft-terrain.tga", &ttex);
	world = new World(&ttex);
	player = new Player(p,l,world);
	tm = world->GenerateTerrain();
	printError("init terrain");

}





void display(void)
{
	if(wIsDown) 
	{
		player->goForward();
	}
	if(sIsDown)
	{
			player->goBackwards();
	}
	player->heightUpdate();
	/*
	if(aIsDown) //TODO: Ta bort?
	{
			temp = VectorSub(p, l);	
			temp = CrossProduct(temp, camUp); 
			temp = Normalize(temp);
			temp = ScalarMult(temp, 0.3f);
			p = VectorAdd(temp, p);
			l = VectorAdd(temp, l);
	}
	if(dIsDown) //TODO: Ta bort?
	{
			temp = VectorSub(p, l);	
			temp = CrossProduct(camUp, temp); 
			temp = Normalize(temp);
			temp = ScalarMult(temp, 0.3f);
			p = VectorAdd(temp, p);
			l = VectorAdd(temp, l);
	}
	
	
	if(lastHeight == -999.0){
		l.y = l.y - p.y + world->findHeight(p.x,p.z) + 2.0;
		p.y = world->findHeight(p.x,p.z) + 2.0;
		lastHeight = p.y;
	}
	GLfloat groundHeight = world->findHeight(p.x,p.z) + 2.0;
	GLfloat newHeight = lastHeight;
	if(newHeight > groundHeight && !inAir){
		inAirTime = 0.0;
		inAir = true;
	}
	
	if(inAir){
		if(jumping){
			//jumpSpeed = -gravitation*jumpTime*jumpTime/2+jumpSpeed*jumpTime;
			newHeight = lastHeight-gravitation*inAirTime*inAirTime/2+jumpSpeed;
		}
		else{
			newHeight = lastHeight-gravitation*inAirTime*inAirTime/2;
		}
	}
	if(newHeight <= groundHeight) inAir=false;
	if(inAir){
		l.y = l.y - p.y + newHeight;
		p.y = newHeight;
		
	} 
	else{
		inAir=false;
			l.y = l.y - p.y + groundHeight;
			p.y = groundHeight;
			
	}
	lastHeight = p.y;
	*/

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, modelView, translate;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	modelView= IdentityMatrix();

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, modelView.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, player->getCamMatrix().m);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex4);
	glUniform1i(glGetUniformLocation(program, "tex1"), 1);
	glUniform1i(glGetUniformLocation(program, "tex2"), 2);
	glUniform1i(glGetUniformLocation(program, "tex3"), 3);
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	
	translate=  T(gx, world->findHeight(gx, gz), gz);
	total = Mult(modelView, translate);

	glutSwapBuffers();

}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	if(jumping){
		if(jumpTime<maxJumpTime){
			jumpTime+=1.0;
		}
		else{
			jumping=false;
			jumpTime=0.0;
		}
	}
	inAirTime+=1.0;
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 
	
	init();
	
	glutKeyboardFunc(keyboardFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(keyboardSpecFunction);
	glutSpecialUpFunc(keyboardSpecUpFunction);
	glutPassiveMotionFunc(mouse);
	glutTimerFunc(20, &timer, 0);
	glutMainLoop();
}
