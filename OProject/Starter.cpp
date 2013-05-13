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



#define near 0.2
 #define far 100.0
 #define right 0.1
 #define left -0.1
 #define top 0.1
 #define bottom -0.1

 

mat4 projectionMatrix, mapRotation;


//För keyboard
bool wIsDown = false;
bool sIsDown = false;
bool aIsDown = false;
bool dIsDown = false;
bool qIsDown = false;
bool eIsDown = false;
bool tIsDown = false;

bool showMap = false;



GLfloat jump = 0.0;

int treeRenderingDistance = 50;

GLfloat mapAngle=0.0;

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

GLfloat hitBoxTree = 0.4;

//För musen...
Point3D p = vec3(45, 5, 45);
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

GLuint program, billBoardProgram, skyProgram;

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


bool posIsTree[254][254];

// vertex array object
unsigned int vertexArrayObjID;

// vertex array object
Model *m, *m2, *tm, *bill, *tree, *sky, *map, *control, *compass;

// Reference to shader program

GLuint tex1, tex2,tex3,tex4, treeTex, skyTex, mapTex, controlTex;
TextureData ttex; // terrain

// Map texture
int mapWidth, mapHeight, mapN;



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

Model* controlModel(void)
{
	int vertexCount = 8;
	int triangleCount = 6;
	
	GLfloat vertexArray[] = {0.0f,0.0f,0.13f,
							 0.15f,0.0f,-0.13f,
							-0.15f,0.0f,-0.13f,
							0.0f,0.3f,0.13f,
							 0.15f,0.3f,-0.13f,
							 -0.15f,0.3f,-0.13f,
							0.0f,0.0f,0.13f,
							0.0f,0.3f,0.13f,};
	GLfloat normalArray[] = {0.0f,0.0f,0.13f,
							 0.1f,0.0f,-0.13f,
							-0.1f,0.0f,-0.13f,
							0.0f,0.0f,0.13f,
							 0.1f,0.0f,-0.13f,
							 -0.1f,0.0f,-0.13f};
	GLfloat texCoordArray[] = { 0.0f,0.0f,
								0.6667f,0.0f,
								0.3333f,0.0f,
								0.0f,1.0f,
								0.6667f,1.0f,
								0.3333f,1.0f,
								1.0f,0.0f,
								1.0f,1.0f};

	GLuint indexArray[] = { 0,3,2,
							3,5,2,
							2,5,1,
							5,4,1,
							1,4,6,
							4,7,6};

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



Model* mapModel(void)
{
	int vertexCount = 4;
	int triangleCount = 2;
	
	GLfloat vertexArray[] = { -0.4f,0.4f,0.0f,
							 0.4f,0.4f,0.0f,
							-0.4f,-0.4f,0.0f,
							 0.4f,-0.4f,0.0f};
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

Model* compassModel(void)
{
	int vertexCount = 4;
	int triangleCount = 2;
	
	GLfloat vertexArray[] = { -0.02f,0.1f,0.0f,
							 0.02f,0.1f,0.0f,
							-0.02f,-0.1f,0.0f,
							 0.02f,-0.1f,0.0f};
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
		case 'W':
			wIsDown = true;
		break;
		case 's':
			sIsDown = true;
		break;
		case 'S':
			sIsDown = true;
		break;
		case 'a':
			aIsDown = true;
		break;
		case 'A':
			aIsDown = true;
		break;
		case 'd':
			dIsDown = true;
		break;	 
		case 'D':
			dIsDown = true;
		break;	
		case 'q':
			qIsDown = true;
		break;
		case 'Q':
			qIsDown = true;
		break;
		case 'e':
			eIsDown = true;
		break;
		case 'E':
			eIsDown = true;
		break;
		
		case 'p':
			exit(0);
		break;
		case 'P':
			exit(0);
		break;
		case ' ':
			showMap = true;
		break;
		case 'z':
			jumping = true;
		break;
		case 'Z':
			jumping = true;
		break;
		case 't':
			player->setTurbo(0.2);
		break;
		case 'T':
			player->setTurbo(0.2);
		break;
		case 'm':
			printf("%f", player->getNextControl().x);
			printf(" ");
			printf("%f", player->getNextControl().z);
			printf(", ");
		break;
		case 'M':
			printf("%f", player->getPos().x);
			printf(" ");
			printf("%f", player->getPos().z);
			printf(", ");
		break;

		case 'y':
			
		
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
		case 'W':
			wIsDown = false;
		break;
		case 's':
			sIsDown = false;
		break;
		case 'S':
			sIsDown = false;
		break;
		case 'a':
			aIsDown = false;
		break;
		case 'A':
			aIsDown = false;
		break;
		case 'd':
			dIsDown = false;
		break;
		case 'D':
			dIsDown = false;
		break;
		case 'q':
			qIsDown = false;
		break;
		case 'Q':
			qIsDown = false;
		break;
		case 'e':
			eIsDown = false;
		break;
		case 'E':
			eIsDown = false;
		break;
	
		case ' ':
			showMap = false;
		break; 
		case 'z':
			jumping = false;
		break; 
		case 'Z':
			jumping = false;
		break;
		case 't':
			player->setTurbo(0.0);
		break;
		case 'T':
			player->setTurbo(0.0);
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

void TreeRandomNumberGen (GLfloat* randX, GLfloat* randZ, GLfloat x, GLfloat z){



	*randX=asin(sin(((x*z + sqrt(x) - x*x*x + z*z*z*z +x*z+z*z)/z)*cos(x)*sin(x)))+1;
	*randZ=asin(sin((z*z+x*z- x*z*x*x/sqrt(z*x) +z*z/x)*cos(z)*cos(z)))+1;

	
}

void DrawBillboard(Model* bm, int inx, int inz, mat4 view)
{

	GLfloat x = (GLfloat) inx;
	GLfloat z = (GLfloat) inz;

	if(randXZ->xz[inx][(int)z].x == -9999.0)
	{
		GLfloat randX;
		GLfloat randZ;
		TreeRandomNumberGen(&randX, &randZ, inx, inz);

		randXZ->xz[inx][(int)z].x = randX;
		randXZ->xz[inx][(int)z].z = randZ;
	}
	
	
	x = x + randXZ->xz[inx][inz].x;
	z = z + randXZ->xz[inx][inz].z;

	vec3 billVec = VectorSub(player->getPos(),vec3(x,0,z));
	vec3 playerLookAt = VectorSub(player->getPos(),player->getLook());
	billVec.y = 0;	
	playerLookAt.y = 0;
	if(Norm(billVec) < treeRenderingDistance && DotProduct(playerLookAt,billVec) > 0)
	{
		//billVec = playerLookAt;
		
		billVec = Normalize(billVec);
		
		playerLookAt.y = 0.0;

			mat4 translate=  T(x, world->findHeight(x, z), z);
			view = Mult(view, translate);
			vec3 billNorm = vec3(0,0,1);

			vec3 upVec = CrossProduct(billNorm, billVec);
			GLfloat cosAngle = DotProduct(billNorm, billVec);
		
			GLfloat angle = acos(cosAngle);
		
			mat4 billRotMat = ArbRotate(upVec, angle);

			view = Mult(view,billRotMat);
	
			glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "camMatrix"), 1, GL_TRUE, player->getCamMatrix().m);
			glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "mdlMatrix"), 1, GL_TRUE, view.m);
			DrawModel(bm, billBoardProgram, "inPosition",  NULL, "inTexCoord"); //TODO: put NULL instead of "inNormal" here to make it work..
	}

}

void DrawMap(Model* map, Model* compass, mat4 view)
{
		vec3 mapNorm = vec3(0,0,1);
		vec3 camPos = VectorAdd(player->getPos(),vec3(0,2.0,0));
		vec3 mapToCam = VectorSub(player->getPos(),player->getLook());
		//Put map in front of the cam
		vec3 mapPos = VectorSub(camPos,mapToCam);
		mat4 translate=  T(mapPos.x,mapPos.y,mapPos.z);
		view = Mult(view, translate);

		//Rotate in around Y-axis
		vec3 mapToCamXZ = mapToCam;
		mapToCamXZ.y = 0;
		mapToCamXZ = Normalize(mapToCamXZ);
		vec3 upVec = CrossProduct(mapNorm, mapToCamXZ);
		GLfloat angle = acos(DotProduct(mapNorm, mapToCamXZ));
		mat4 billRotMat = ArbRotate(upVec, angle);

			view = Mult(view,billRotMat);

		//Rotate in around XZ-axis
		mapToCam = Normalize(mapToCam);
		angle = acos(DotProduct(mapToCamXZ,mapToCam));
		
			if (mapToCam.y < 0)
				billRotMat = ArbRotate(vec3(1,0,0), angle);	
			else
				billRotMat = ArbRotate(vec3(-1,0,0), angle);

		view = Mult(view,billRotMat);
		
		//Rotate map 
		mat4 viewCompass = view;
		view = Mult(view,ArbRotate(vec3(0,0,1), mapAngle));
		//view = Mult(view,S(1,8/6,1)); 

		view = Mult(view,T(0,0,-0.1));
		glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "camMatrix"), 1, GL_TRUE, player->getCamMatrix().m);
		glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "mdlMatrix"), 1, GL_TRUE, view.m);
		DrawModel(map, billBoardProgram, "inPosition",  NULL, "inTexCoord"); 
		
		//Kompass, antagligen rätt fel med axlarna...
		viewCompass = Mult(viewCompass,T(-0.4,-0.3,0.1));
		GLfloat compassAngle = acos(DotProduct(vec3(1,0,0),mapToCamXZ));
		if (mapToCam.z < 0)
			viewCompass = Mult(viewCompass,ArbRotate(vec3(0,0,-1), compassAngle));
		else
			viewCompass = Mult(viewCompass,ArbRotate(vec3(0,0,1), compassAngle));

		glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "mdlMatrix"), 1, GL_TRUE, viewCompass.m);
		DrawModel(compass, billBoardProgram, "inPosition",  NULL, "inTexCoord");

}

bool nearTree()
{

		Point3D currentPos = player->getPos();
		currentPos.y = 0.0;
		Point3D treePos = currentPos;
		

		for(int i=-2; i < 3; i++){
			for(int j=-2; j < 3; j++){

				treePos.x = floor(currentPos.x)+i;
				treePos.z = floor(currentPos.z)+j;
				if(treePos.x >= 0.0 && treePos.z >= 0.0){
					Point3D actualTreePos = vec3(treePos.x,0.0,treePos.z);
					actualTreePos.x += randXZ->xz[(int)treePos.x][(int) treePos.z].x;
					actualTreePos.z += randXZ->xz[(int)treePos.x][(int) treePos.z].z;

					Point3D vecFromTree = VectorSub(currentPos, actualTreePos);
					GLfloat distFromTree = Norm(vecFromTree);
					if(distFromTree < hitBoxTree){
						printf("hitTree");
						return true;
					}
				}
			}
		}


	return false;
	
}

void drawControl(int x, int z)
{
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, controlTex);
	glUniform1i(glGetUniformLocation(billBoardProgram, "tex1"), 5);

	mat4 controlView = T(x, world->findHeight(x, z)+ 0.7, z);
	glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "camMatrix"), 1, GL_TRUE, player->getCamMatrix().m);
	glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "mdlMatrix"), 1, GL_TRUE, controlView.m);
	DrawModel(control, billBoardProgram, "inPosition",  NULL, "inTexCoord"); //TODO: put NULL instead of "inNormal" here to make it work..


}

void init(void)
{

	GLenum err = glewInit();

	glutReshapeWindow(screenX,screenY);
	//glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);


	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST); 
	glDisable(GL_CULL_FACE);
	
	
	glAlphaFunc(GL_GREATER, 0.0f);
	
	printError("GL inits");

	projectionMatrix = frustum(left,right, bottom, top, near, far);


	for(int i = 0; i<256; i++){
		for(int j = 0; j<256; j++){
			randXZ->xz[i][j].x=-9999.0;
			randXZ->xz[i][j].z=-9999.0;
		}

	}

	
	
	
	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	billBoardProgram = loadShaders("billBoardShader.vert", "billBoardShader.frag");
	skyProgram = loadShaders("skyShader.vert", "skyShader.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	
	
	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	LoadTGATextureSimple("mygrass.tga", &tex1);
	LoadTGATextureSimple("newterrain.tga", &tex2);
	LoadTGATextureSimple("ok1.tga", &tex3);
	LoadTGATextureSimple("water0hk.tga", &tex4);
	
// Load terrain data
	
	LoadTGATexture("ownterrain.tga", &ttex);
	world = new World(&ttex);
	player = new Player(p,l,world);
	tm = world->GenerateTerrain();

	for(int i=0; i<254; i++){
		for(int j=0; j<254; j++){
			if(world->findHeight(i,j) < 0.5){
				
				posIsTree[i][j]=false;
			}
			else{
				posIsTree[i][j]=true;
			}

		}
	}

	glUseProgram(billBoardProgram);
	glUniformMatrix4fv(glGetUniformLocation(billBoardProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	bill = billboardModel();
	map = mapModel();
	compass = compassModel();
	control = controlModel();

	LoadTGATextureSimple("tree.tga", &treeTex);
	LoadTGATextureSimple("control.tga", &controlTex);
	mapTex = LoadBMPTexture("curves.bmp", 512, 512);

	glUseProgram(skyProgram);
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	sky = LoadModelPlus("skybox.obj");
	LoadTGATextureSimple("SkyBox512.tga", &skyTex);

	printError("init terrain");

	
}

void display(void)
{
	if(wIsDown) 
	{
		if(nearTree()) player->playerHitTree();
		player->goForward();
		
	}
	else
		player->stop();
	if(sIsDown)
	{
			player->goBackwards();
	}
	if(qIsDown) 
	{
		mapAngle+=0.02;	
	}
	if(eIsDown)
	{
			mapAngle-=0.02;
	}
	if(jumping)
	{
			player->jump();
	}
	player->heightUpdate();
	if(player->isNextControl())
	{
		printf("yeeeeey");
		player->setNextControl(world->getControlPos(player->getPunshedControls()));
		

	}

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, modelView, translate;
	
	printError("pre display");

		modelView= IdentityMatrix();


//Sky box

	glUseProgram(skyProgram);
	glDisable(GL_DEPTH_TEST);
	mat4 camMatrix2 = player->getCamMatrix();

	camMatrix2.m[3] = 0;
	camMatrix2.m[7] = 0;
	camMatrix2.m[11] = 0;	

	mat4 modelView2 = modelView;

	mat4 translate2 = T(0,-0.5,0);
	modelView2 = Mult(modelView2,translate2);

	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "mdlMatrix"), 1, GL_TRUE, modelView2.m);
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "camMatrix"), 1, GL_TRUE, camMatrix2.m);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	glUniform1i(glGetUniformLocation(skyProgram, "tex1"), 5);

	DrawModel(sky, skyProgram, "inPosition", NULL, "inTexCoord");
	
	glEnable(GL_DEPTH_TEST);
	
	glUseProgram(program);


	// Build matrix
	


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

	glUseProgram(billBoardProgram);
	glEnable(GL_ALPHA_TEST);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, treeTex);
	glUniform1i(glGetUniformLocation(billBoardProgram, "tex1"), 4);

	for(int x=2; x<254; x=x+3)
	{
		for(int z=2; z<254; z=z+3)
		{
			if(posIsTree[x][z]){
				DrawBillboard(bill,x,z,modelView);
			}
		}
	}

	/*glEnable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mapWidth, mapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glDisable(GL_TEXTURE_2D);*/

	drawControl(40,40);
	drawControl(50,50);
	drawControl(60,60);
	
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, mapTex);
	glUniform1i(glGetUniformLocation(billBoardProgram, "tex1"), 6);


	if(showMap)
	{
			DrawMap(map,compass,modelView); 
	}
	
	
	glDisable(GL_ALPHA_TEST);


	
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
