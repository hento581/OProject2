#include <GL/glew.h> 
#include <GL/freeglut.h> 

#include "GL_utilities.h" 
#include <math.h>
#include "loadobj.h"
#include "LoadTGA2.h"
#include "VectorUtils3.h"
#include "Player.h"
#include "World.h"
#include <time.h>


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
GLuint treeProgram;

GLfloat vertices[] = { -0.5f,-0.5f,0.0f,
						-0.5f,0.5f,0.0f,
						0.5f,-0.5f,0.0f };

struct XandZ
{
	GLfloat x;
	GLfloat z;

};

struct RandXZ				
{
	XandZ xz[256][256];

};

RandXZ* randXZ = new RandXZ;
// vertex array object
unsigned int vertexArrayObjID;

// vertex array object
Model *m, *m2, *tm, *bill, *tree;

// Reference to shader program

GLuint tex1, tex2,tex3,tex4, treeTex;
TextureData ttex; // terrain

Model* billboardModel(void)
{

	

	int vertexCount = 4;
	int triangleCount = 2;
	
	GLfloat vertexArray[] = { -1.0f,5.0f,0.0f,
						     1.0f,5.0f,0.0f,
						    -1.0f,0.0f,0.0f,
							 1.0f,0.0f,0.0f};
	GLfloat normalArray[] = { 0.0f,0.0f,1.0f,
						     0.0f,0.0f,1.0f,
						     0.0f,0.0f,1.0f,
							 0.0f,0.0f,1.0f,};
	GLfloat texCoordArray[] = { 1.0f,0.0f,
								0.0f,0.0f,
								1.0f,1.0f,
								0.0f,1.0f};

	GLuint indexArray[] = { 0,1,2,1,2,3};

	Model* m = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return m;
}

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
		case ' ':
			jumping = false;
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

	glutReshapeWindow(screenX,screenY);
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);


	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	//glEnable(GL_DEPTH_TEST); //TODO: This creates the error of trees not being transperant to each other
	glDisable(GL_CULL_FACE);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 100.0);
	
	
	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	treeProgram = loadShaders("treeShader.vert", "treeShader.frag");
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

	glUseProgram(treeProgram);
	glUniformMatrix4fv(glGetUniformLocation(treeProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	bill = billboardModel();

	LoadTGATextureSimple("tree.tga", &treeTex);

	printError("init terrain");

	
}


void DrawBillboard(Model* bm, int inx, int inz, mat4 view)
{

	GLfloat x = (GLfloat) inx;
	GLfloat z = (GLfloat) inz;
	if(randXZ->xz[inx][(int)z].x < 0.0){
		randXZ->xz[inx][(int)z].x = (float)rand()/((float)RAND_MAX)*2;
		randXZ->xz[inx][(int)z].z = (float)rand()/((float)RAND_MAX)*2;
	}
	

	x = x + randXZ->xz[inx][inz].x;
	z = z + randXZ->xz[inx][inz].z;
	vec3 billVec = VectorSub(player->getPos(),vec3(x,0,z));
	if(Norm(billVec) < 50)
	{
		billVec.y = 0;	
		mat4 translate=  T(x, world->findHeight(x, z), z);
		view = Mult(view, translate);
		vec3 billNorm = vec3(0,0,1);
		billVec = Normalize(billVec);
		vec3 upVec = CrossProduct(billNorm, billVec);
		GLfloat angle = acos(DotProduct(billNorm, billVec));
		mat4 billRotMat = ArbRotate(upVec, angle);

		view = Mult(view,billRotMat);
	
		glUniformMatrix4fv(glGetUniformLocation(treeProgram, "camMatrix"), 1, GL_TRUE, player->getCamMatrix().m);
		glUniformMatrix4fv(glGetUniformLocation(treeProgram, "mdlMatrix"), 1, GL_TRUE, view.m);

		DrawModel(bm, treeProgram, "inPosition",  NULL, "inTexCoord"); //TODO: put NULL instead of "inNormal" here to make it work..
	}

}

void display(void)
{
	if(wIsDown) 
	{
		player->goForward();
		
	}
	else
		player->stop();
	if(sIsDown)
	{
			player->goBackwards();
	}
	if(jumping)
	{
			player->jump();
	}

	
	player->heightUpdate();
	

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

	glUseProgram(treeProgram);
	glEnable(GL_BLEND);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, treeTex);
	glUniform1i(glGetUniformLocation(treeProgram, "tex1"), 4);

	for(int x=2; x<254; x=x+3)
	{
		for(int z=2; z<254; z=z+3)
		{
			DrawBillboard(bill,x,z,modelView);
		}
	}

	glDisable(GL_BLEND);
	glutSwapBuffers();
	
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 
	
	init();
	

	srand((unsigned)time(0));
	glutKeyboardFunc(keyboardFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(keyboardSpecFunction);
	glutSpecialUpFunc(keyboardSpecUpFunction);
	glutPassiveMotionFunc(mouse);
	glutTimerFunc(20, &timer, 0);
	glutMainLoop();
}
