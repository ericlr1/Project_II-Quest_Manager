#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "GuiButton.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Define multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

	//TODO
	void CheckEvent();
	void OrderEvent();
	void ResetTriggers();

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;

private:
	SDL_Texture* img;
	SDL_Texture* mouseTileTex = nullptr;
	SDL_Texture* originTex = nullptr;
	SDL_Texture* completed_mision = nullptr;

	// L12: Debug pathfing
	iPoint origin;
	bool originSelected = false;

	// L15: DONE 2: Declare a GUI Button and create it using the GuiManager
	GuiButton* button1; 
	GuiButton* button2;

	//TODO (Hacia abajo)
	SDL_Texture* diamond = nullptr;
	SDL_Texture* emerald = nullptr;
	SDL_Texture* gold = nullptr;

	int r = 0; //Times you completed any mision
	int score = 0; //Order the misions variable

	//Triggers
	int trigger_1 = 0;
	int trigger_2 = 0;
	int trigger_3 = 0;

	//Misions completed
	bool first_mision = false;
	bool second_mision = false;
	bool third_mision = false;
};

#endif // __SCENE_H__