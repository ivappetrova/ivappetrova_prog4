#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "TextureComponent.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "RotatorComponent.h"
#include "MoveCommand.h"
#include "Controller.h"
#include "InputManager.h"
#include "DealDamageCommand.h"
#include "HealthComponent.h"
#include "LivesDisplayComponent.h"
#include "PlayerDiedDisplayComponent.h"
#include "PlayerPointsComponent.h"
#include "PointsDisplayComponent.h"
#include "PickUpCommand.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto bg = std::make_unique<dae::GameObject>();
	bg->AddComponent<dae::TextureComponent>()->SetTexture("background.png");
	scene.Add(std::move(bg));

	/*auto logo = std::make_unique<dae::GameObject>();
	logo->AddComponent<dae::TextureComponent>()->SetTexture("logo.png");
	logo->SetLocalPosition(358, 180);
	scene.Add(std::move(logo));*/

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto fpsGo = std::make_unique<dae::GameObject>();
	fpsGo->SetLocalPosition(10, 10);
	fpsGo->AddComponent<dae::TextComponent>("FPS: 0", font);
	fpsGo->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(fpsGo));

	///////////////////// Week 02- Bubble and bobble rotating 
	//auto pivot = std::make_unique<dae::GameObject>();
	//pivot->SetLocalPosition(20, 350);
	//
	//auto* pBubble = new dae::GameObject();
	//pBubble->AddComponent<dae::TextureComponent>()->SetTexture("bubble.png");
	//pBubble->AddComponent<dae::RotatorComponent>(100.f, 1.f);
	//pBubble->SetParent(pivot.get(), false);
	//
	//auto* pBobble = new dae::GameObject();
	//pBobble->AddComponent<dae::TextureComponent>()->SetTexture("bobble.png");
	//pBobble->AddComponent<dae::RotatorComponent>(50.f, -2.f);
	//pBobble->SetParent(pBubble, false);
	//
	//scene.Add(std::move(pivot));


	////////////////////////////////////////////////// Week 04 - Input & Week 05 - Observer and Event Queue

	auto& input = dae::InputManager::GetInstance();

	auto fontInstructions = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);


	////////////////////// Bubble

	// Instructions
	auto bubbleInstructions = std::make_unique<dae::GameObject>();
	bubbleInstructions->SetLocalPosition(10, 70);
	bubbleInstructions->AddComponent<dae::TextComponent>("Use WASD to move Bubble", fontInstructions);
	scene.Add(std::move(bubbleInstructions));

	auto bubbleDmgInstructions = std::make_unique<dae::GameObject>();
	bubbleDmgInstructions->SetLocalPosition(10, 100);
	bubbleDmgInstructions->AddComponent<dae::TextComponent>("Use P to deal dmg to Bubble", fontInstructions);
	scene.Add(std::move(bubbleDmgInstructions));

	// Player initialization
	auto character1 = std::make_unique<dae::GameObject>();
	character1->AddComponent<dae::TextureComponent>()->SetTexture("bubble.png");
	character1->SetLocalPosition(10, 300);
	dae::GameObject* pChar1 = character1.get();
	scene.Add(std::move(character1));

	auto* pHealth = pChar1->AddComponent<dae::HealthComponent>(3);

	// Lives display
	auto livesGo = std::make_unique<dae::GameObject>();
	livesGo->SetLocalPosition(10, 130);
	livesGo->AddComponent<dae::TextComponent>("Lives: 3", fontInstructions);
	auto* pLivesDisplay = livesGo->AddComponent<dae::LivesDisplayComponent>(3);
	scene.Add(std::move(livesGo));

	pHealth->AddObserver(pLivesDisplay);

	// Died display
	auto diedGo = std::make_unique<dae::GameObject>();
	diedGo->SetLocalPosition(10, 160);
	diedGo->AddComponent<dae::TextComponent>("", fontInstructions);
	auto* pDiedDisplay = diedGo->AddComponent<dae::PlayerDiedDisplayComponent>();
	scene.Add(std::move(diedGo));

	pHealth->AddObserver(pDiedDisplay);

	// Points
	auto* pPoints = pChar1->AddComponent<dae::PlayerPointsComponent>();

	// Points display
	auto pointsGo = std::make_unique<dae::GameObject>();
	pointsGo->SetLocalPosition(10, 190);
	pointsGo->AddComponent<dae::TextComponent>("Points: 0", fontInstructions);
	auto* pPointsDisplay = pointsGo->AddComponent<dae::PointsDisplayComponent>();
	scene.Add(std::move(pointsGo));

	pPoints->AddObserver(pPointsDisplay);


	////////////////////// Bobble

	// Instructions
	auto bobbleInstructions = std::make_unique<dae::GameObject>();
	bobbleInstructions->SetLocalPosition(500, 70);
	bobbleInstructions->AddComponent<dae::TextComponent>("Use D-Pad to move Bobble", fontInstructions);
	scene.Add(std::move(bobbleInstructions));

	auto bobbleDmgInstructions = std::make_unique<dae::GameObject>();
	bobbleDmgInstructions->SetLocalPosition(500, 100);
	bobbleDmgInstructions->AddComponent<dae::TextComponent>("Use A (gamepad) to deal dmg to Bubble", fontInstructions);
	scene.Add(std::move(bobbleDmgInstructions));

	// Player initialization
	auto character2 = std::make_unique<dae::GameObject>();
	character2->AddComponent<dae::TextureComponent>()->SetTexture("bobble.png");
	character2->SetLocalPosition(500, 300);
	dae::GameObject* pChar2 = character2.get();
	scene.Add(std::move(character2));

	auto* pHealth2 = pChar2->AddComponent<dae::HealthComponent>(3);

	// Lives display
	auto livesGo2 = std::make_unique<dae::GameObject>();
	livesGo2->SetLocalPosition(500, 130);
	livesGo2->AddComponent<dae::TextComponent>("Lives: 3", fontInstructions);
	auto* pLivesDisplay2 = livesGo2->AddComponent<dae::LivesDisplayComponent>(3);
	scene.Add(std::move(livesGo2));

	pHealth2->AddObserver(pLivesDisplay2);
	
	// Died display
	auto diedGo2 = std::make_unique<dae::GameObject>();
	diedGo2->SetLocalPosition(500, 160);
	diedGo2->AddComponent<dae::TextComponent>("", fontInstructions);
	auto* pDiedDisplay2 = diedGo2->AddComponent<dae::PlayerDiedDisplayComponent>();
	scene.Add(std::move(diedGo2));

	pHealth2->AddObserver(pDiedDisplay2);

	// Points
	auto* pPoints2 = pChar2->AddComponent<dae::PlayerPointsComponent>();

	// Points display
	auto pointsGo2 = std::make_unique<dae::GameObject>();
	pointsGo2->SetLocalPosition(500, 190);
	pointsGo2->AddComponent<dae::TextComponent>("Points: 0", fontInstructions);
	auto* pPointsDisplay2 = pointsGo2->AddComponent<dae::PointsDisplayComponent>();
	scene.Add(std::move(pointsGo2));

	pPoints2->AddObserver(pPointsDisplay2);

	//////////////////////////// INPUT
	const float SPEED1{ 50.f };
	const float SPEED2{ SPEED1 * 2.f };
	const int DAMAGE {1};
	const int POINTS{ 20 };

	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::InputManager::KeyState::Pressed,
							  std::make_unique<dae::MoveCommand>(pChar1, 0.f, -1.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_S, dae::InputManager::KeyState::Pressed,
							  std::make_unique<dae::MoveCommand>(pChar1, 0.f, 1.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Pressed,
							  std::make_unique<dae::MoveCommand>(pChar1, -1.f, 0.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Pressed,
							  std::make_unique<dae::MoveCommand>(pChar1, 1.f, 0.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_O, dae::InputManager::KeyState::Up,
							  std::make_unique<dae::DealDamageCommand>(pChar2, DAMAGE, pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_P, dae::InputManager::KeyState::Up,
							  std::make_unique<dae::PickUpCommand>(pChar1, POINTS));


	input.BindControllerCommand(0, dae::Controller::Button::DPadUp, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, 0.f, -1.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadDown, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, 0.f, 1.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, -1.f, 0.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, 1.f, 0.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonX, dae::Controller::KeyState::Up,
							    std::make_unique<dae::DealDamageCommand>(pChar1, DAMAGE, pChar2));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonA, dae::Controller::KeyState::Up,
								std::make_unique<dae::PickUpCommand>(pChar2, POINTS));

								
}


int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
