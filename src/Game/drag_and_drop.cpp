//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "drag_and_drop.h"
#include "collision.h"

// Globals

// global variable with the currently picked object
int selectedModel = NONE;

// external globals from main
extern float rx, ry, pz;
extern int height;
extern vector<Model*> gameModels;
extern vector<Model*> decorationModels;


// Functions

// Check if a model was selected by performed click event
// (Check which model is under the given x,y coords)
void checkForModelSelection(unsigned x, unsigned y)
{
	int modelCount = gameModels.size();
	
	// window coordinates start at top-left corner, opengl coordinates start at bottom-left
	y = height-y; // --> subtract y from the window height
		
	// render the game models (in picking mode)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glm::mat4 mvp = calculateMVP();
	//drawGameModels(mvp, true); 
	drawGameModels(true); 
	
	// read the clicked pixel color from the buffer
	int	copyWidth = 1; // we want just one pixel
	int copyHeight = 1;
	std::vector<float> data(copyWidth*copyHeight*4);
	glReadBuffer(GL_BACK);
	glReadPixels(x,y,copyWidth,copyHeight,GL_BLUE,GL_FLOAT,&data[0]);
	
	// determine which object was clicked based on the color of the obtained pixel
	if (data[0] != 0.0) // black (0.0) means no model was clicked
	{
		// calculate the index of the model (+0.5 is added to achieve correct int rounding)
		int modelIndex = (int)  ( ( (1.0-data[0]) * modelCount ) + 0.5) ;
				
		selectedModel = modelIndex; // write the index of the model to the global variable
		//printf("clicked model: %d \n", selectedModel); // DEBUG

		SDL_ShowCursor(false); // hide cursor while the model is selected
		return;
	}
	
	// no object was clicked	
	selectedModel = NONE; 
}


// translate selected model according to the mose movement and current camera view
void dragAndDropModel(int xrel, int yrel)
{
			float division_factor = 10.0; // to make the changes less rapid
			
			float deltax, deltay, deltaz;
    		float oldx, oldy, oldz;
    		float newx, newy, newz;

    		//float temp_x, temp_y, temp_z; // temporary variables to store the desired translation
    		
    		Model* model = gameModels[selectedModel];

			glm::mat4 mv = glm::rotate(
				    glm::rotate(
				        glm::translate(
				            glm::mat4(1.0f),
				            glm::vec3(0, 0, pz)
				            ),
				        0.0f /*ry*/, glm::vec3(1, 0, 0)
				        ),
				    rx, glm::vec3(0, 1, 0)
				    );
    		
    		glm::mat4 mat;    		
    		const float *pMat = (const float*)glm::value_ptr(mat); // pointer to the data in the matrix

    		mv = glm::rotate(mv, -ry, glm::vec3(1, 0, 0));
    		
			//mat = glm::translate(mv, glm::vec3(model.x, model.y, model.z));
			glm::vec3 position = model->getPositionVector();
			mat = glm::translate(mv, glm::vec3(position.x, position.y, position.z));
			
			// get values before translation
			oldx = pMat[12];
			oldy = pMat[13];
			oldz = pMat[14];
			
			mat = glm::translate(mat, glm::vec3(xrel/division_factor, yrel/division_factor, 0.0));
    		
    		// get values after translation
    		newx = pMat[12];
    		newy = pMat[13];
    		newz = pMat[14];
    		
    		// DEBUG:
    		/*
    		cout << "{" << endl;
			for (int i = 0; i < 4; ++i)
			{
			    cout << pMat[4*i] << " " << pMat[4*i+1] << " " << pMat[4*i+2] << " " << pMat[4*i+3] << endl;
			}
			cout << "}" << endl;
			*/
			
			// calculate change in the model coordinates
    		deltax = newx-oldx;
    		deltay = newy-oldy;
    		deltaz = newz-oldz;
    		
    		// check for collision and try to apply changes
    		int modelCollide;
    		int decorCollide;
    		
    		gameModels[selectedModel]->translate(deltax, 0.0, 0.0);
    		modelCollide = checkForCollision(gameModels[selectedModel], gameModels, selectedModel); 
    		decorCollide = checkForCollision(gameModels[selectedModel], decorationModels, NONE);
    		if (modelCollide != NONE || decorCollide != NONE || !insideAquarium(gameModels[selectedModel]) )
    		{
    			gameModels[selectedModel]->translate(-deltax, 0.0, 0.0);
    		}
    		
    		gameModels[selectedModel]->translate(0.0, -deltay, 0.0);
    		modelCollide = checkForCollision(gameModels[selectedModel], gameModels, selectedModel);
    		decorCollide = checkForCollision(gameModels[selectedModel], decorationModels, NONE);    		
    		if (modelCollide != NONE || decorCollide != NONE || !insideAquarium(gameModels[selectedModel]) )
    		{
    			gameModels[selectedModel]->translate(0.0, deltay, 0.0);
    		}
    		
    		gameModels[selectedModel]->translate(0.0, 0.0, -deltaz);
    		modelCollide = checkForCollision(gameModels[selectedModel], gameModels, selectedModel);
    		decorCollide = checkForCollision(gameModels[selectedModel], decorationModels, NONE);   		
    		if (modelCollide != NONE || decorCollide != NONE || !insideAquarium(gameModels[selectedModel]) )
    		{
    			gameModels[selectedModel]->translate(0.0, 0.0, deltaz);
    		}
    		
    		//printf("collision index: %d \n", collisionIndex); // DEBUG
    		
    		// apply changes
	    	//gameModels[selectedModel]->translate(deltax, -deltay, -deltaz);
}

void rotateSelectedModel(int rot)
{
	gameModels[selectedModel]->rotate(0.0, rot, 0.0);
	int modelCollide = checkForCollision(gameModels[selectedModel], gameModels, selectedModel);
    int decorCollide = checkForCollision(gameModels[selectedModel], decorationModels, NONE);   		
    if (modelCollide != NONE || decorCollide != NONE || !insideAquarium(gameModels[selectedModel]) )
    {
    	gameModels[selectedModel]->rotate(0.0, -rot, 0.0);
    }
}

void stopDragAndDrop()
{
	selectedModel = NONE; // clear model selection for drag-and-drop system
	SDL_ShowCursor(true); 	
}



