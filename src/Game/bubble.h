//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef __BUBBLE__
#define __BUBBLE__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void BubbleInit();
void BubbleDraw(glm::mat4 mv, glm::mat4 mvp, glm::mat3 mn);
void DrawSubmarineBubble(float x, float y, float z);

#endif
