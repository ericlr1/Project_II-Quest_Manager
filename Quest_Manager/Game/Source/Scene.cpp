#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "PathFinding.h"
#include "GuiManager.h"
#include "QuestManager.h"		//TODO

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
	}
	
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	
	// L03: DONE: Load map
	bool retLoad = app->map->Load();

	// L12 Create walkability map
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if(retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	//Sets the camera to be centered in isometric map
	if (app->map->mapData.type == MapTypes::MAPTYPE_ISOMETRIC) {
		uint width, height;
		app->win->GetWindowSize(width, height);
		app->render->camera.x = width / 2;

		// Texture to highligh mouse position 
		mouseTileTex = app->tex->Load("Assets/Maps/path.png");

		// Texture to show path origin 
		originTex = app->tex->Load("Assets/Maps/x.png");
	}

	if (app->map->mapData.type == MapTypes::MAPTYPE_ORTHOGONAL) {

		// Texture to highligh mouse position 
		mouseTileTex = app->tex->Load("Assets/Maps/path_square.png");

		// Texture to show path origin 
		originTex = app->tex->Load("Assets/Maps/x_square.png");
	}

	// L15: DONE 2: Declare a GUI Button and create it using the GuiManager
	uint w, h;
	app->win->GetWindowSize(w, h);
	button1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Button 1", { (int)w / 2 - 50,(int)h / 2 - 30,100,20 }, this);
	button2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Button 2", { (int)w / 2 - 50,(int)h / 2,100,20 }, this);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	// L14: DONE 4: Make the camera movement independent of framerate
	float speed = 0.2 * dt;
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += ceil(speed);

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= ceil(speed);

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += ceil(speed);

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= ceil(speed);

	// Draw map
	app->map->Draw();

	//L15: Draw GUI
	app->guiManager->Draw();

	// L08: DONE 3: Test World to map method

	/*
	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	iPoint mouseTile = iPoint(0, 0); 

	if (app->map->mapData.type == MapTypes::MAPTYPE_ISOMETRIC) {
		mouseTile = app->map->WorldToMap(mouseX - app->render->camera.x - app->map->mapData.tileWidth / 2,
												mouseY - app->render->camera.y - app->map->mapData.tileHeight / 2);
	}
	if (app->map->mapData.type == MapTypes::MAPTYPE_ORTHOGONAL) {
		mouseTile = app->map->WorldToMap(mouseX - app->render->camera.x,
												mouseY - app->render->camera.y);
	}

	//Convert again the tile coordinates to world coordinates to render the texture of the tile
	iPoint highlightedTileWorld = app->map->MapToWorld(mouseTile.x, mouseTile.y);
	app->render->DrawTexture(mouseTileTex, highlightedTileWorld.x, highlightedTileWorld.y);

	//Test compute path function
	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (originSelected == true)
		{
			app->pathfinding->CreatePath(origin, mouseTile);
			originSelected = false;
		}
		else
		{
			origin = mouseTile;
			originSelected = true;
			app->pathfinding->ClearLastPath();
		}
	}

	// L12: Get the latest calculated path and draw
	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
	}

	// L12: Debug pathfinding
	iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
	app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
	*/

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Event by %d ",control->id);

	switch (control->id)
	{
	case 1:
		LOG("Button 1 click");
		break;
	case 2:
		LOG("Button 2 click");
		break;
	}

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

//TODO (Hacia abajo)

//TODO 5: Now that we have our base working, it's time to implement it with the context, 
//therefore we are gonna create a simple function that checks the events that we are interested in. 
//The skeleton is already implemented, therefore you will only need to fill the loop that will iterate the active_quests list and checks those conditions

//void Scene::CheckEvent()
//{
//	for (std::list <Quest*>::iterator it = App->quest_manager->active_quests.begin(); it != App->quest_manager->active_quests.end(); it++)
//	{
//		int quest_id = (*it)->id;
//
//		switch (quest_id)
//		{
//		case 1:
//			if (cherry == true && banana == true && orange == true && watermelon == true && mango == true && r > -1)
//			{
//				(*it)->completed = true;
//				r++;
//				App->fruit->Restart();
//				App->quest_manager->finished_quests.push_back((*it));
//				App->quest_manager->active_quests.erase(it);
//			}
//			break;
//		case 2:
//			if (cherry == true && banana == true && orange == true && watermelon == true && mango == true && r > 0)
//			{
//				(*it)->completed = true;
//				r++;
//				App->fruit->Restart();
//				App->quest_manager->finished_quests.push_back((*it));
//				App->quest_manager->active_quests.erase(it);
//			}
//			break;
//		case 3:
//			if (cherry == true && banana == true && orange == true && watermelon == true && mango == true && r > 2)
//			{
//				(*it)->completed = true;
//				r++;
//				App->fruit->Restart();
//				App->quest_manager->finished_quests.push_back((*it));
//				App->quest_manager->active_quests.erase(it);
//			}
//			break;
//		case 4:
//			if (cherry == true && banana == true && orange == true && watermelon == true && mango == true && score == 4)
//			{
//				(*it)->completed = true;
//				r++;
//				App->fruit->Restart();
//				App->quest_manager->finished_quests.push_back((*it));
//				App->quest_manager->active_quests.erase(it);
//			}
//			break;
//		case 5:
//			if (cherry == true && banana == true && orange == true && watermelon == true && mango == true && r > 3)
//			{
//				(*it)->completed = true;
//				App->fruit->Restart();
//				App->quest_manager->finished_quests.push_back((*it));
//				App->quest_manager->active_quests.erase(it);
//			}
//			break;
//		default:
//			break;
//		}
//
//		//TODO 6: We are almost done, we are currently correctly checking the active_quests list but we aren't doing anything with a quest
//		// once it's completed, therefore we need to transfer the complete quests to the finished_quests list 
//	}
//
//	//TODO 7: As the final TODO, just take a look at how we are only drawing an achievement (quest completed) once they are in the finished_quests list
//	// I'd also like you to take a look at how we make the 4th achievement a different quest by forcing the player to complete the side-quests in a particular order
//	for (std::list <Quest*>::iterator it = App->quest_manager->finished_quests.begin(); it != App->quest_manager->finished_quests.end(); it++)
//	{
//		int quest_id = (*it)->id;
//
//		switch (quest_id)
//		{
//		case 1:
//			//Draw something
//			break;
//		case 2:
//			//Draw something
//			break;
//		case 3:
//			//Draw something
//			break;
//		case 4:
//			//Draw something
//			break;
//		case 5:
//			//Draw something
//			break;
//		default:
//			break;
//		}
//	}
//
//}

//void Scene::OrderEvent()
//{
//	if (cherry == true && App->fruit->b_points == 0 && App->fruit->o_points == 0 && App->fruit->w_points == 0 && App->fruit->m_points == 0) {
//		score = 1;
//	}
//	if (cherry == true && banana == true && App->fruit->o_points == 0 && App->fruit->w_points == 0 && App->fruit->m_points == 0) {
//		score = 2;
//	}
//	if (cherry == true && banana == true && orange == true && App->fruit->w_points == 0 && App->fruit->m_points == 0) {
//		score = 3;
//	}
//	if (cherry == true && banana == true && orange == true && watermelon == true && App->fruit->m_points == 0) {
//		score = 4;
//	}
//}
