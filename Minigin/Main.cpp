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
