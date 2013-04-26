#include "World.h"



World::World(void)
{
}

World::World(TextureData* texture){

	this->tex=texture;

}


World::~World(void)
{
}

Point3D World::findNormal(int x, int z, GLfloat *vertexArray)
{
	Point3D edge=vec3(0,1,0);
	Point3D p0 = vec3(x,vertexArray[(x + z * this->tex->width)*3 + 1], z);
	Point3D p1,p2,p3;

	if(x > 0)
		p1= SetVector(x-1, vertexArray[(x-1 + z * this->tex->width)*3 + 1], z);
	else 
		return edge;

	if(z > 0)
	{
		p2 = SetVector(x-1, vertexArray[(x-1 + (z-1) * this->tex->width)*3 + 1], z-1);
		p3 = SetVector(x, vertexArray[(x + (z-1) * this->tex->width)*3 + 1], z-1);
	}
	else 
		return edge;
	Point3D v1,v2,v3;
	v1 = VectorSub(p1,p0);
	v2 = VectorSub(p2,p0);
	v3 = VectorSub(p3,p0);
	
	Point3D Nres,N1,N2,N3;

	N1 = Normalize(CrossProduct(v2,v1));
	N2 = Normalize(CrossProduct(v3,v2));
	N3 = Normalize(CrossProduct(v1,v3));

	Nres = VectorAdd(N1,N2);
	Nres = VectorAdd(N3,Nres);

	Nres = Normalize(Nres);
	
	return Nres;
}

Model* World::GenerateTerrain()
{
	int vertexCount = this->tex->width * this->tex->height;
	int triangleCount = (this->tex->width-1) * (this->tex->height-1) * 2;
	int x, z;
	
	GLfloat *vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat*)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint*)malloc(sizeof(GLuint) * triangleCount*3);
	
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * this->tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * this->tex->width)*3 + 1] = this->tex->imageData[(x + z * this->tex->width) * (this->tex->bpp/8)] / 10.0;
			vertexArray[(x + z * this->tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.

			Point3D normal = this->findNormal(x,z,vertexArray); 
			normalArray[(x + z * this->tex->width)*3 + 0] = normal.x;
			normalArray[(x + z * this->tex->width)*3 + 1] = normal.y;
			normalArray[(x + z * this->tex->width)*3 + 2] = normal.z;

// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * this->tex->width)*2 + 0] = x/10.0; // (float)x / tex->width;
			texCoordArray[(x + z * this->tex->width)*2 + 1] = z/10.0; // (float)z / tex->height;
		}
	for (x = 0; x < this->tex->width-1; x++)
		for (z = 0; z < this->tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (this->tex->width-1))*6 + 0] = x + z * this->tex->width;
			indexArray[(x + z * (this->tex->width-1))*6 + 1] = x + (z+1) * this->tex->width;
			indexArray[(x + z * (this->tex->width-1))*6 + 2] = x+1 + z * this->tex->width;
		// Triangle 2
			indexArray[(x + z * (this->tex->width-1))*6 + 3] = x+1 + z * this->tex->width;
			indexArray[(x + z * (this->tex->width-1))*6 + 4] = x + (z+1) * this->tex->width;
			indexArray[(x + z * (this->tex->width-1))*6 + 5] = x+1 + (z+1) * this->tex->width;
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

GLfloat World::interpolate(Point3D P, Point3D A, Point3D B, Point3D C)
{
	Point3D N,N1,N2;
	int Ax = A.x;
	int Az = A.z;
	int Bx = B.x;
	int Bz = B.z;
	int Cx = C.x;
	int Cz = C.z;


	A.y = this->tex->imageData[(Ax + Az * this->tex->width) * (this->tex->bpp/8)] / 10.0;
	B.y = this->tex->imageData[(Bx + Bz * this->tex->width) * (this->tex->bpp/8)] / 10.0;
	C.y = this->tex->imageData[(Cx + Cz * this->tex->width) * (this->tex->bpp/8)] / 10.0;

	N1 = VectorSub(C, A);
	N2 = VectorSub(B, A);

	N = CrossProduct(N1,N2);
	if(N.y == 0)
	return 100;

	return A.y - (N.x*(P.x - A.x) + N.z*(P.z-A.z))/N.y;
} 

int World::insideTriangle(Point3D P, Point3D A, Point3D B, Point3D C)
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

GLfloat World::findHeight(GLfloat x, GLfloat z)
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
		return 500; //TODO: better error handling
	}

	return this->interpolate(P,A,B,C);

}