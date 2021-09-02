//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef __WATER__
#define __WATER__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void WaterInit();
void WaterDraw(glm::mat4 mv, glm::mat4 mvp, glm::mat3 mn);

double fRand(double fMin, double fMax);
float GetWaveTime();
int GetNumWaves();
float* GetAmplitudes();
float* GetWaveLengths();
float* GetSpeeds();
float* GetDirections();

#endif
