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

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto bg = std::make_unique<dae::GameObject>();
	bg->AddComponent<dae::TextureComponent>()->SetTexture("background.png");
	scene.Add(std::move(bg));

	auto logo = std::make_unique<dae::GameObject>();
	logo->AddComponent<dae::TextureComponent>()->SetTexture("logo.png");
	logo->SetLocalPosition(358, 180);
	scene.Add(std::move(logo));

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


	////////////////////////////////////////////////// Week 04 - Input

	auto& input = dae::InputManager::GetInstance();

	auto fontInstructions = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

	auto bubbleInstructions = std::make_unique<dae::GameObject>();
	bubbleInstructions->SetLocalPosition(10, 100);
	bubbleInstructions->AddComponent<dae::TextComponent>("Use WASD to move Bubble", fontInstructions);
	scene.Add(std::move(bubbleInstructions));

	auto bobbleInstructions = std::make_unique<dae::GameObject>();
	bobbleInstructions->SetLocalPosition(10, 130);
	bobbleInstructions->AddComponent<dae::TextComponent>("Use D-Pad to move Bobble", fontInstructions);
	scene.Add(std::move(bobbleInstructions));


	///////////// Bubble
	auto character1 = std::make_unique<dae::GameObject>();
	character1->AddComponent<dae::TextureComponent>()->SetTexture("bubble.png");
	character1->SetLocalPosition(200, 300);
	dae::GameObject* pChar1 = character1.get();
	scene.Add(std::move(character1));

	const float SPEED1 = 50.f;

	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::InputManager::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar1, 0.f, -1.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_S, dae::InputManager::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar1, 0.f, 1.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar1, -1.f, 0.f, SPEED1));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar1, 1.f, 0.f, SPEED1));

	///////////// Bobble
	auto character2 = std::make_unique<dae::GameObject>();
	character2->AddComponent<dae::TextureComponent>()->SetTexture("bobble.png");
	character2->SetLocalPosition(400, 300);
	dae::GameObject* pChar2 = character2.get();
	scene.Add(std::move(character2));

	const float SPEED2 = SPEED1 * 2.f;  

	input.BindControllerCommand(0, dae::Controller::Button::DPadUp, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, 0.f, -1.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadDown, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, 0.f, 1.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, -1.f, 0.f, SPEED2));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::Controller::KeyState::Pressed,
								std::make_unique<dae::MoveCommand>(pChar2, 1.f, 0.f, SPEED2));
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
