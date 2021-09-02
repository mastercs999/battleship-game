//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#ifndef DRAG_AND_DROP_H_
#define DRAG_AND_DROP_H_

#include "game_models.h"

#define NONE -1

void drawGameModels(bool picking); // (defined in main)

void checkForModelSelection(unsigned x, unsigned y);
void dragAndDropModel(int xrel, int yrel);
void rotateSelectedModel(int rot);
void stopDragAndDrop();


#endif
