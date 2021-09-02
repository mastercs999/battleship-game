//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "collision.h"
#include "aquarium_data.h"


// Calculate coeficients a,b,c,d of for equation of a plane
// ax + by + cz + d = 0
// based on given points K, L, M
void calculatePlane(glm::vec4 K, glm::vec4 L, glm::vec4 M, tTestPlane * plane)
{
	// vector a = L-K
	glm::vec3 a = glm::vec3(L.x - K.x,  L.y - K.y,  L.z - K.z);
	
	// vector b = L-M
	glm::vec3 b = glm::vec3(L.x - M.x,  L.y - M.y,  L.z - M.z);
		
	// calculate vector product n = a x b (normal of the plane)
	glm::vec3 n = glm::vec3( a.y * b.z - a.z * b.y,
							 a.z * b.x - a.x * b.z,
							 a.x * b.y - a.y * b.x
							);
	
	// claculate coeficient d (substitute one of the points)
	float d = -n.x*K.x - n.y*K.y - n.z*K.z;

	// store calculated coeficients
	plane->a = n.x;
	plane->b = n.y;
	plane->c = n.z;
	plane->d = d;
}


// set the orientation property of a plane (knowing that P is inside)
void setPlaneOrientation(glm::vec3 P, tTestPlane * plane)
{
	// substitute P into the equation
	float result = plane->a * P.x + plane->b * P.y + plane->c * P.z + plane->d;
	
	// positive result maps to true, negative to false
	if (result <= 0.0) 
	{
		// inside point produces negative values -> outside means positive values
		plane->orientation = true;
	}
	else
	{
		plane->orientation = false;
	}
}

// calculate parameters of test planes for collision detection
void calculateTestPlanes(tTestPlane * testPlanes, vector<glm::vec4> &points)
{
	int testPlanesCount = 6;
	int pointsCount = points.size();
	
	calculatePlane(points[0],points[1],points[2], &testPlanes[0]); // A,B,C
	calculatePlane(points[0],points[1],points[4], &testPlanes[1]); // A,B,E
	calculatePlane(points[1],points[2],points[6], &testPlanes[2]); // B,C,G 
	calculatePlane(points[2],points[3],points[7], &testPlanes[3]); // C,D,H
	calculatePlane(points[0],points[3],points[4], &testPlanes[4]); // A,D,E
	calculatePlane(points[4],points[5],points[6], &testPlanes[5]); // E,F,G
	
	// determine a point iside the bounding box (center point)
	float sumx = 0, sumy = 0, sumz = 0;
	for (int i=0; i<pointsCount; ++i)
	{
		sumx += points[i].x;
		sumy += points[i].y;
		sumz += points[i].z;
	}
	
	glm::vec3 P = glm::vec3( sumx/pointsCount, sumy/pointsCount, sumz/pointsCount);
	
	// set orientation property for all planes based on the point P (inside point)
	for (int i=0; i<testPlanesCount; ++i)
	{
		setPlaneOrientation(P, &testPlanes[i]);
	}
}

// determine if collision occured based on a set of test planes and bounding box points
bool determineCollision(tTestPlane * testPlanes, vector<glm::vec4> &points)
{
	int testPlanesCount = 6;
	int	bbVertexCount = points.size();
	bool testPassed;
	bool collision = true;
	
		// try all testing planes
		for (int i=0; i<testPlanesCount; ++i)
		{
			tTestPlane plane = testPlanes[i];
			
			// check if all the vertices are on the same side of the testing plane
			// if yes, then there is no collision with this object
			
			testPassed = true;
			for (int j=0; j<bbVertexCount; ++j)
			{
				float x = points[j].x;
				float y = points[j].y;
				float z = points[j].z;
			
				// substitute the coords of the vertex into the equation of the testing plane
				float result = plane.a * x + plane.b * y + plane.c * z + plane.d;
				
				bool side; 
				if (result > 0.0)
				{
					side = true;
				}
				else
				{
					side = false;
				}
							
				// if the vertex is on a wrong side -> try next plane
				if ( side != plane.orientation )
				{
					testPassed = false;
					break;
				}
			}
			
			if (testPassed)
			{
				collision = false;
				break; // there is no collision with this model
			}
		}
		
	return collision;
}

// returns index of the first model with which collision occured (in modelVector)
// or NONE if no collision was detected
int checkForCollision(Model * testedModel, vector<Model*> &modelVector, int skipIndex)
{
	// calculate all testing planes
	const int testPlanesCount = 6;
	tTestPlane testPlanes[testPlanesCount];
	tTestPlane testPlanes2[testPlanesCount];
	
	// determine points A, B, C, etc...
	vector<glm::vec4> bBox2;
	prepareBoxVector(testedModel, bBox2);
	
	// calculate testing planes
	calculateTestPlanes( testPlanes, bBox2);
	
	int vectorCount = modelVector.size();
	 
	bool collision;
	vector<glm::vec4> bBox;
	
	// check with all models in the given vector
	for (int i=0; i<vectorCount; ++i)
	{
		if (i == skipIndex)
		{
			continue;
		}
		
		bBox.clear();
		prepareBoxVector(modelVector[i], bBox);
		
		collision = determineCollision(testPlanes, bBox);
		
		if (collision)
		{
			// collision occured when testing first set of testing planes
			
			// calculate next set of test planes (for this bounding box)
			calculateTestPlanes(testPlanes2, bBox);
			
			collision = determineCollision(testPlanes2, bBox2);
			if (collision)
			{
				// there is no plane that would separate the objects (= there is collision)
				return i;
			}
		}
	}
	
	return NONE;
}


// check if a model is inside aquarium
bool insideAquarium(Model * model)
{
	vector<glm::vec4> bBox;
	prepareBoxVector(model, bBox);
	
	int count = bBox.size();
	for(int i=0; i<count; ++i)
	{
		if (bBox[i].x > X_MAX || bBox[i].x < X_MIN ||
			bBox[i].y > -1.0 || bBox[i].y < Y_MIN ||
			bBox[i].z > Z_MAX || bBox[i].z < Z_MIN  ) 
		{
			return false;
		}
	}
	
	return true;
}

// fill vector of bounding box points
void prepareBoxVector(Model * model, vector<glm::vec4> &bBox)
{
	bBox.push_back(model->BoxVertA());
	bBox.push_back(model->BoxVertB());
	bBox.push_back(model->BoxVertC());
	bBox.push_back(model->BoxVertD());
	bBox.push_back(model->BoxVertE());
	bBox.push_back(model->BoxVertF());
	bBox.push_back(model->BoxVertG());
	bBox.push_back(model->BoxVertH());
}

// output the points of a bounding box
void printBox(vector<glm::vec4> &box)
{
	int count = box.size();
	printf("bBox: \n");
	for (int i=0; i<count; ++i)
	{
		printf(" x: %lf, y: %lf, z: %lf \n", box[i].x, box[i].y, box[i].z);
	}
}

