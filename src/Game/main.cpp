//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef WIN
#include <glm/gtc/matrix_projection.hpp>
#endif
#include <glm/gtc/type_ptr.hpp>

#include "aquarium.h"
#include "water.h"
#include "bubble.h"

#include "model.h"
#include "model_shader.h"

#include "game_models.h"
#include "drag_and_drop.h"
#include "collision.h"

#include "skybox.h"


using namespace std;


int width, height;
float rx = 0.0f, ry = 0.0f, pz = -70.0f, px = 0.0f, py = 0.0f;

glm::mat4 projection;
glm::mat4 mv;
glm::mat4 mvp;
glm::mat3 mn;

// Extern globals
extern int selectedModel;
extern vector<Model*> gameModels;
extern vector<Model*> decorationModels;
extern Model* missile;

// Fucntion declarations
void drawGameModels(bool picking);
void drawDecorations();
void drawMissile();

// Aquarium
bool cullBackface = true;
bool showAquarium = true;

// Models
ModelShader* modelShader;
bool showModels = true;

// Drag and drop
bool enableDragAndDrop = true;

// Game
bool gameStarted = false;

// Camera
bool rotateCamera = true;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void onInit()
{
    // Skybox
    SkyboxInit();
    
    // AQUARIUM
    initAquarium();
    
    // MODELS
    initModels();
	
    // Shader for game models
    modelShader = new ModelShader();
    
    // BUBBLE
    BubbleInit();

    // WATER
    WaterInit();
}

void onWindowRedraw()
{
    // Set some funcs
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // Calculate some matrix
    projection = glm::perspective(45.0f, (float)width/(float)height, 1.0f, 1000.0f);
    
    glm::mat4 skyboxV;
    if (rotateCamera)
    {
        mv = glm::rotate(
                glm::rotate(
                    glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(px, 0, pz)
                        ),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                );
        
        skyboxV = glm::rotate(
            glm::rotate(
                glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(px, 0, 0)
                    ),
                ry, glm::vec3(1, 0, 0)
                ),
            rx, glm::vec3(0, 1, 0)
            );
    }
    else
    {
        mv = glm::translate(
                glm::rotate(
                    glm::rotate(
                        glm::mat4(1.0f),
                        ry, glm::vec3(1, 0, 0)
                        ),
                    rx, glm::vec3(0, 1, 0)
                    ),
                glm::vec3(px, py, pz)
                );
        
        skyboxV = glm::translate(
            glm::rotate(
                glm::rotate(
                    glm::mat4(1.0f),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                ),
            glm::vec3(0, 0, 0)
            );
    }
    
    

    mvp = projection*mv;
	
    mn = glm::mat3(
            glm::rotate(
                glm::rotate(
                    glm::mat4(1.0f),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                )
            );
    
    // Skybox
    SkyboxDraw(skyboxV, projection);
    
    // AQUARIUM
    if (showAquarium) {
        drawAquarium(mv, mvp, mn);
    } 

    // MODEL 
    if(showModels)
    {
      // Set actual MVP !!!
      modelShader->setM(glm::mat4(1.0f));
      modelShader->setV(mv);
      modelShader->setP(projection);  
      
      // Draw models
      drawDecorations();
      drawGameModels(false);

		// Draw missile
		drawMissile();
    }
    
    
    // BUBBLE and WATER
    if (ry > 0)
    {
        BubbleDraw(mv, mvp, mn);
	    WaterDraw(mv, mvp, mn);
    }
    else
    {
	    WaterDraw(mv, mvp, mn);
        BubbleDraw(mv, mvp, mn);
    }    

    // Switch to new buffer
    SDL_GL_SwapBuffers();
}

// draw missile and check for collisions
void drawMissile()
{
	if (!gameStarted)
	{
		return;	
	}
	missile->moveAhead();
    missile->Draw(modelShader);
      
      // test for missile collision with submarines
      int collisionIndex = checkForCollision(missile, gameModels, NONE);
      
      if (collisionIndex != NONE && !gameModels[collisionIndex]->isDestroyed())
      {
      	
      	 gameModels[collisionIndex]->setDestroyed(true); // destroy the submarine
      	 missile->hit();
      }
       
      // test for missile collision with decorations
      collisionIndex = checkForCollision(missile, decorationModels, NONE);
      if (collisionIndex != NONE)
      {
      	  missile->hit();
      }
}

// draw models to decorate the aquarium
void drawDecorations()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDisable(GL_BLEND);
      
	int modelCount = decorationModels.size();
	for (int i=0; i < modelCount; ++i)
	{	
    	decorationModels[i]->Draw(modelShader);
    }
}

// draw all game models that can be selected
void drawGameModels(bool picking)
{
	int modelCount = gameModels.size();
	
	for (int i=0; i < modelCount; ++i)
	{
		Model* model = gameModels[i];
		
		if (picking)
    	{
    		// draw for the selection mode
    		float pickingColor = 1.0 - i * (1.0 / modelCount);
    		model->DrawBox(modelShader, pickingColor);
    	}
    	else
    	{
    		// regular draw
    		model->Draw(modelShader);
    	}
	}
}

void fireMissile(unsigned x, unsigned y)
{
  // Hold actual cullBackface value, set false
  bool set_cullBackface = cullBackface;
  cullBackface = false;

  // Draw Aquarium without backface culling
  drawAquarium(mv, mvp, mn);

  // Set previous cillBackface value
  cullBackface = set_cullBackface;

  // Get data from screen plane and depth buffer
  GLfloat depth;
  glReadPixels(x, height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

  // Compute coordinates in object area
  glm::vec4 viewport = glm::vec4(0, 0, width, height);
  glm::vec3 wincoord = glm::vec3(x, height - y - 1, depth);
  glm::vec3 objcoord = glm::unProject(wincoord, mv /*view*/, projection, viewport);

  // Fire missile
  missile->fire(objcoord.x, objcoord.y, objcoord.z);
}

void onWindowResized(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w; height = h;
}

void startGame()
{
	if (gameStarted)
	{
		return;
	}
	gameStarted = true;
    enableDragAndDrop = false;
    
    int modelCount = gameModels.size();
    for(int i=0; i<modelCount; ++i)
    {
    	gameModels[i]->setVisibility(false);
    }
    missile->setVisibility(true);
}

void restartGame()
{
	gameStarted = false;
    enableDragAndDrop = true;
    SDL_ShowCursor(true);
    
    // show all models and reset them
	int modelCount = gameModels.size();
    for(int i=0; i<modelCount; ++i)
    {
    	gameModels[i]->setDestroyed(false);
    	gameModels[i]->setVisibility(true);
    }
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); return;
        
        case SDLK_b : cullBackface = !cullBackface; return;
        case SDLK_n : showAquarium = !showAquarium; return;
        case SDLK_m : showModels = !showModels; return;
        case SDLK_v : enableDragAndDrop = !enableDragAndDrop; return;
        case SDLK_r : restartGame(); return;
        case SDLK_c : rotateCamera = !rotateCamera; return;
        
        case SDLK_SPACE : startGame(); return;
        
        default : return;
    }
}

void CheckHoldingKey(Uint8* keystate)
{
    if (rotateCamera)
        return;
    
    float rxRad = rx * 0.0174532925199;
    float ryRad = ry * 0.0174532925199;
    if(keystate[SDLK_w])
    {
        px -= sin(rxRad) * abs(cos(ryRad));
        py += sin(ryRad);
        pz += cos(rxRad) * abs(cos(ryRad));
    }
    if(keystate[SDLK_s])
    {
        px += sin(rxRad) * abs(cos(ryRad));
        py -= sin(ryRad);
        pz -= cos(rxRad) * abs(cos(ryRad));
    }
    if(keystate[SDLK_a])
    {
        px -= -cos(rxRad);
        pz += sin(rxRad);
    }
    if(keystate[SDLK_d])
    {
        px += -cos(rxRad);
        pz -= sin(rxRad);
    }
}

void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/)
{
}

void onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons)
{
    if(buttons & SDL_BUTTON_LMASK)
    {
    	if (selectedModel == NONE)
    	{
    		// if no model is selected, move the camera
	        rx += xrel;
    	    ry += yrel;
        
    	    // restrict the up-down rotation
    	    if (ry > 90.0) 
    	    {
    	    	ry = 90.0; // top view 
    	    }
    	    else if (ry < -90.0)
    	    {
    	    	ry = -90.0; // view from underneath the aquarium
    	    }
    	}
    	else
    	{
    		dragAndDropModel(xrel, yrel);// translate the selected model
    	}   
        //redraw();
    }
    
    // right mouse button performs rotation of the selected model
    if(buttons & SDL_BUTTON_RMASK)
    {
	    if (selectedModel != NONE) 
    	{
			rotateSelectedModel(xrel);
    		//redraw();
    	}
    }

    // Missile follows cursor
    if(gameStarted) //TODO DEBUG
    {
      // Hold actual cullBackface value, set false
        bool set_cullBackface = cullBackface;
        cullBackface = false;

        // Draw Aquarium without backface culling
        drawAquarium(mv, mvp, mn);

        // Set previous cillBackface value
        cullBackface = set_cullBackface;

        // Get data from screen plane and depth buffer
        GLfloat depth;
        glReadPixels(x, height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

        // Compute coordinates in object area
        glm::vec4 viewport = glm::vec4(0, 0, width, height);
        glm::vec3 wincoord = glm::vec3(x, height - y - 1, depth);
        glm::vec3 objcoord = glm::unProject(wincoord, mv /*view*/, projection, viewport);

        missile->setCursorPosition(objcoord.x, objcoord.y, objcoord.z);
    }
}

void onMouseDown(Uint8 button, unsigned x, unsigned y)
{
	float zoomStep = 4; // o kolik se bude zoomovat (lze libovolne zmenit)
	
    switch(button) {
    
    	// zoom in/out pomoci kolecka:
        case SDL_BUTTON_WHEELUP : pz += zoomStep; break;
        case SDL_BUTTON_WHEELDOWN : pz -= zoomStep; break;
        
        // left or right button -> check if a model should be selected
        case SDL_BUTTON_LEFT : 
                if (enableDragAndDrop)
                {
                  checkForModelSelection(x,y);
                }
                break;
        case SDL_BUTTON_RIGHT : 
        	if (enableDragAndDrop)
			{
	        	checkForModelSelection(x,y);
	        }
        	if(gameStarted) fireMissile(x, y);
        	break;
        	
        case SDL_BUTTON_MIDDLE : //fireMissile(x, y);
                                 break; //zprava

        default : return;
    };
    //redraw();
}

void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
	stopDragAndDrop();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
    try {
        // Init SDL - only video subsystem will be used
        if(SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();

        // Shutdown SDL when program ends
        atexit(SDL_Quit);

        init(800, 600, 24, 16, 0);

		onWindowRedraw();

        mainLoop(25);

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
