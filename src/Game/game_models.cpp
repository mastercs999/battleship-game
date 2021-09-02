//
// Project for course Computer Graphics (PGR)
// Faculty of Information Technology, Brno University of Technology
//
// Authors: Petr Dobes, xdobes14@stud.fit.vutbr.cz
//          Jaromir Deak, xdeakj00@stud.fit.vutbr.cz
//          Petr Huf, xhufpe00@stud.fit.vutbr.cz
// Date: December 2014
//

#include "game_models.h"

// Globals: 

// List of game models
vector<Model*> gameModels;
vector<Model*> decorationModels;

Model* missile;

//float move_submarine_z = 0.2;


// Fucntions

void initModels()
{
	// Model variables

	const GLchar* missile_path = "models/missile/rotated_missile.obj";

	Model* submarine_u48_0;
	Model* submarine_u48_1;
	const GLchar* submarine_u48_path = "models/u48/U48.obj";

	Model* old_submarine_0;
	Model* old_submarine_1;
	Model* old_submarine_2;
	const GLchar* old_submarine_path = "models/old_submarine/submarine.obj";

	Model* wreck;
	Model* wreck2;
	const GLchar* wreck_path = "models/wreck/0.obj";

	Model* rocks;
	Model* rocks2;
	const GLchar* rocks_path = "models/rocks/rock.obj";

	Model* boulders;
	const GLchar* boulders_path = "models/boulders/boulder.obj";


  // MODEL LOADING
  //-----------------------------------------------------

	// Missile
  missile = new Model(missile_path);
  missile->setScale(0.3);
  missile->setPosition(0, -7, 23);
  missile->setVisibility(false);

	// submarines
  submarine_u48_0 = new Model(submarine_u48_path);
  submarine_u48_0->setSwing(true);
  submarine_u48_0->setScale(0.055);
  submarine_u48_0->setRotation(0, 270, 0);
  submarine_u48_0->setPosition(10, -10, 0);

  submarine_u48_1 = new Model(submarine_u48_path);
  submarine_u48_1->setSwing(true);
  submarine_u48_1->setScale(0.055);
  submarine_u48_1->setPosition(-5, -5, 5);

  old_submarine_0 = new Model(old_submarine_path);
  old_submarine_0->setSwing(true);
  old_submarine_0->setScale(0.004);
  old_submarine_0->setPosition(-16, -4, -16);
  old_submarine_0->setRotation(0, 0, 0);

  old_submarine_1 = new Model(old_submarine_path);
  old_submarine_1->setSwing(true);
  old_submarine_1->setScale(0.004);
  old_submarine_1->setPosition(1, -8, -3);
  old_submarine_1->setRotation(0, 90, 0);

  old_submarine_2 = new Model(old_submarine_path);
  old_submarine_2->setSwing(true);
  old_submarine_2->setScale(0.004);
  old_submarine_2->setPosition(-12, -3, 16);
  old_submarine_2->setRotation(0, 0, 0);

	// decorations
  wreck = new Model(wreck_path);
  wreck->setScale(0.002);
  wreck->setPosition(13, -12.55, 12);
  wreck->setRotation(0, 0, 180);

  wreck2 = new Model(wreck_path);
  wreck2->setScale(0.002);
  wreck2->setPosition(-8, -11.9, 8);
  wreck2->setRotation(-30, 30, 170);

  rocks = new Model(rocks_path);
  rocks->setScale(4.5);
  rocks->setPosition(-8, -13.1, 9);
  rocks->setRotation(0, 45, 0);

  rocks2 = new Model(rocks_path);
  rocks2->setScale(6.5);
  rocks2->setPosition(6, -13.1, -9);
  rocks2->setRotation(0, 145, 0);

  boulders = new Model(boulders_path);
  boulders->setScale(0.005);
  boulders->setPosition(8, -13.1, 6);
  boulders->setRotation(0, 45, 0);

	// add models of submarines into the gameModels vector
	gameModels.push_back(submarine_u48_0);
	gameModels.push_back(submarine_u48_1);
	gameModels.push_back(old_submarine_0);
	gameModels.push_back(old_submarine_1);
	gameModels.push_back(old_submarine_2);
	
	// add all other models into the decorationModels vector
	decorationModels.push_back(wreck);
	decorationModels.push_back(wreck2);
	decorationModels.push_back(boulders);
	decorationModels.push_back(rocks);
	decorationModels.push_back(rocks2);
	
}



 
