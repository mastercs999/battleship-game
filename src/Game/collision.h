//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef COLLISION_H_
#define COLLISION_H_

#include <vector>

#include "pgr.h"
#include "model.h"

#define NONE -1

// structure holding coeficients of a plane equation
typedef struct sTestPlane {
	float  a, b, c, d;
	bool orientation; // true if 'outside' means positive result for substituted point
} tTestPlane;

void calculatePlane(glm::vec4 K, glm::vec4 L, glm::vec4 M, tTestPlane * plane);
void setPlaneOrientation(glm::vec3 P, tTestPlane * plane);
int checkForCollision(Model * testedModel, vector<Model*> &modelVector, int skipIndex);
void prepareBoxVector(Model * model, vector<glm::vec4> &bBox);
bool insideAquarium(Model * model);
void printBox(vector<glm::vec4> &box);

#endif
