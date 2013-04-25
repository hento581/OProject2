#include <GL/glew.h> 
#include <GL/freeglut.h> 

#include "GL_utilities.h" 
#include <math.h>
#include "loadobj.h"
#include "LoadTGA2.h"
#include "VectorUtils3.h"


#define near 1.0
 #define far 200.0
 #define right 0.5
 #define left -0.5
 #define top 0.5
 #define bottom -0.5

mat4 projectionMatrix;

mat4 camMatrix;

//För keyboard
bool wIsDown = false;
bool sIsDown = false;
bool aIsDown = false;
bool dIsDown = false;



GLfloat jump = 0.0;

//För gravitation
GLfloat lastHeight = -999.0;
GLfloat gravSpeed = 0.3;
GLfloat gravitation = 0.0982/400.0;
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

GLuint program;

GLfloat vertices[] = { -0.5f,-0.5f,0.0f,
						-0.5f,0.5f,0.0f,
						0.5f,-0.5f,0.0f };

// vertex array object
unsigned int vertexArrayObjID;


Point3D findNormal(int x, int z, GLfloat *vertexArray, TextureData *tex)
{
	Point3D edge=vec3(0,1,0);
	Point3D p0 = vec3(x,vertexArray[(x + z * tex->width)*3 + 1], z);
	Point3D p1,p2,p3;

	if(x > 0)
		p1= SetVector(x-1, vertexArray[(x-1 + z * tex->width)*3 + 1], z);
	else 
		return edge;

	if(z > 0)
	{
		p2 = SetVector(x-1, vertexArray[(x-1 + (z-1) * tex->width)*3 + 1], z-1);
		p3 = SetVector(x, vertexArray[(x + (z-1) * tex->width)*3 + 1], z-1);
	}
	else 
		return edge;
	Point3D v1,v2,v3;
	v1 = VectorSub(p1,p0);
	v2 = VectorSub(p2,p0);
	v3 = VectorSub(p3,p0);

	
	Point3D Nres,N1,N2,N3;

	N1 = CrossProduct(v2,v1);
	N2 = CrossProduct(v3,v2);
	N3 = CrossProduct(v1,v3);

	N1 = Normalize(N1);
	N2 = Normalize(N2);
	N3 = Normalize(N3);

	Nres = VectorAdd(N1,N2);
	Nres = VectorAdd(N3,Nres);

	Nres = Normalize(Nres);
	
	return Nres;
}


Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	

	
	GLfloat *vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat*)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint*)malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 10.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.

			Point3D normal = findNormal(x,z,vertexArray,tex); 
			normalArray[(x + z * tex->width)*3 + 0] = normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = normal.z;
/*printf("%d ", x);
printf("%d :", z);
printf("{%f, ", normal.x);
printf("%f, ", normal.y);
printf("%f }\n", normal.z);
*/

// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x/10.0; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z/10.0; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
	
	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}


// vertex array object
Model *m, *m2, *tm;
// Reference to shader program

GLuint tex1, tex2,tex3,tex4;
TextureData ttex; // terrain





GLfloat interpolate(Point3D P, Point3D A, Point3D B, Point3D C,TextureData *tex)
{
	Point3D N,N1,N2;
	int Ax = A.x;
	int Az = A.z;
	int Bx = B.x;
	int Bz = B.z;
	int Cx = C.x;
	int Cz = C.z;


	A.y = tex->imageData[(Ax + Az * tex->width) * (tex->bpp/8)] / 10.0;
	B.y = tex->imageData[(Bx + Bz * tex->width) * (tex->bpp/8)] / 10.0;
	C.y = tex->imageData[(Cx + Cz * tex->width) * (tex->bpp/8)] / 10.0;

	N1 = VectorSub(C, A);
	N2 = VectorSub(B, A);

	N = CrossProduct(N1,N2);
	if(N.y == 0)
	return 100;

	return A.y - (N.x*(P.x - A.x) + N.z*(P.z-A.z))/N.y;
} 


int insideTriangle(Point3D P, Point3D A, Point3D B, Point3D C)
{
	Point3D v0,v1,v2; 

	v0 = VectorSub(C, A);
	v1 = VectorSub(B, A);
	v2 = VectorSub(P, A);


	// Compute dot products
	GLfloat  dot00 = DotProduct(v0, v0);
	GLfloat  dot01 = DotProduct(v0, v1);
	GLfloat  dot02 = DotProduct(v0, v2);
	GLfloat  dot11 = DotProduct(v1, v1);
	GLfloat  dot12 = DotProduct(v1, v2);

	// Compute barycentric coordinates
	GLfloat invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	GLfloat u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	GLfloat v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	if((u >= 0) && (v >= 0) && (u + v < 1))
	return 1;
	return 0;
} 

GLfloat findHeight(GLfloat x, GLfloat z,TextureData *tex)
{
	int x1, z1, x2, z2;
	x1 = x;
	x2 = x1+1;
	z1 = z;
	z2 = z1+1;

	Point3D P = vec3(x, 0, z);

	Point3D A,B,C;

	Point3D A1 = vec3(x1, 0, z1);
	Point3D B1 = vec3(x1, 0, z2);
	Point3D C1 = vec3(x2, 0, z1);

	Point3D A2 = vec3(x1, 0, z2);
	Point3D B2 = vec3(x2, 0, z1);
	Point3D C2 = vec3(x2, 0, z2);

	if(insideTriangle(P, A1, B1, C1))
	{
		A=A1;
		B=B1;
		C=C1;
	}
	else if(insideTriangle(P, A2, B2, C2))
	{
		A=A2;
		B=B2;
		C=C2;
	}
	else
	{
	return 500;
	}

	return interpolate(P,A,B,C,&ttex);

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
	temp = VectorSub(l, p);
	temp2 = CrossProduct(camUp, temp);
	temp3 = CrossProduct(temp2, temp);
	temp = Normalize(temp); //vector till "synplanet"
	temp2 = Normalize(temp2); //vector till höger i synplanet
	temp3 = Normalize(temp3); //vector uppåt i synplanet
	temp2 = ScalarMult(temp2, xdiff);
	temp3 = ScalarMult(temp3, ydiff);
	l = VectorAdd(p, temp);
	l = VectorAdd(l, temp2);
	l = VectorAdd(l, temp3);

	if(relX != 0 || relY != 0)
	glutWarpPointer(screenX/2,screenY/2);
	
}


void init(void)
{

	GLenum err = glewInit();

	//glutReshapeWindow(screenX,screenY);
	 glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	Point3D up = vec3(0.0f, 1.0f, 0.0f);
	camMatrix = lookAtv(p, l, up);

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
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

// Load Sphere

	m = LoadModelPlus("groundsphere.obj");

}





void display(void)
{
	if(wIsDown)
	{
			temp = VectorSub(l, p);
			temp = Normalize(temp);
			temp = ScalarMult(temp, 0.3f);
			p = VectorAdd(temp, p);
			l = VectorAdd(temp, l);	
	}
	if(sIsDown){
			temp = VectorSub(p, l);
			temp = Normalize(temp);
			temp = ScalarMult(temp, 0.3f);
			p = VectorAdd(temp, p);
			l = VectorAdd(temp, l);
	}
	if(aIsDown){
			temp = VectorSub(p, l);	
			temp = CrossProduct(temp, camUp); 
			temp = Normalize(temp);
			temp = ScalarMult(temp, 0.3f);
			p = VectorAdd(temp, p);
			l = VectorAdd(temp, l);
	}
	if(dIsDown){
			temp = VectorSub(p, l);	
			temp = CrossProduct(camUp, temp); 
			temp = Normalize(temp);
			temp = ScalarMult(temp, 0.3f);
			p = VectorAdd(temp, p);
			l = VectorAdd(temp, l);
	}

	if(lastHeight == -999.0){
		l.y = l.y - p.y + findHeight(p.x,p.z, &ttex) + 2.0;
		p.y = findHeight(p.x,p.z, &ttex) + 2.0;
		lastHeight = p.y;
	}
	GLfloat groundHeight = findHeight(p.x,p.z, &ttex) + 2.0;
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
	
	

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, modelView, translate;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	Point3D up = vec3(0,1,0);

	camMatrix = lookAtv(p,l,up);
	modelView= IdentityMatrix();

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, modelView.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	
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
	
	

	translate=  T(gx, findHeight(gx, gz, &ttex), gz);
	total = Mult(modelView, translate);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex3);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex3);
	glUniform1i(glGetUniformLocation(program, "tex1"), 1);
	glUniform1i(glGetUniformLocation(program, "tex2"), 2);
	DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");


	printError("display 2");
	
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
