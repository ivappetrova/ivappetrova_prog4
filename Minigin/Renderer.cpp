#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Renderer.h"
#include "SceneManager.h"
#include "Texture2D.h"

//imgui libs
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include <vector>
#include <chrono>

void dae::Renderer::Init(SDL_Window* window)
{
	m_pWindow = window;
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	m_pRenderer = SDL_CreateRenderer(window, nullptr);
	if (m_pRenderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#if __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
#endif

	ImGui_ImplSDL3_InitForSDLRenderer(window, m_pRenderer);
	ImGui_ImplSDLRenderer3_Init(m_pRenderer);

}

void dae::Renderer::Render() const
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
	
	DrawExercise1();
	DrawExercise2();

	ImGui::Render();

	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_pRenderer);

	SceneManager::GetInstance().Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_pRenderer);
	SDL_RenderPresent(m_pRenderer);
}

void dae::Renderer::Destroy()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (m_pRenderer != nullptr)
	{
		SDL_DestroyRenderer(m_pRenderer);
		m_pRenderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	SDL_GetTextureSize(texture.GetSDLTexture(), &dst.w, &dst.h);
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_pRenderer; }

///////////////////////////////////////////////////////////////////////////////////////////////////// Exercise 1 
void dae::Renderer::DrawExercise1() const
{

	ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_Once);
	if (ImGui::Begin("Exercise 1"))
	{
		static int nSamples1{ 100 };
		static std::vector<float> timings{};

		ImGui::InputInt("# samples", &nSamples1);

		if (ImGui::Button("Thrash the cache with GameObject3D"))
		{
			std::vector<int> myArr(1'000'000);
			timings.clear();

			for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
			{
				float total = 0;

				for (int sample = 0; sample < nSamples1; ++sample)
				{
					auto start = std::chrono::high_resolution_clock::now();
					for (size_t i = 0; i < myArr.size(); i += stepsize)
					{
						myArr[i] *= 2;
					}
					auto end = std::chrono::high_resolution_clock::now();
					total += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				}

				timings.push_back((float)total / nSamples1);
			}
		}

		// draw data if there is new data
		if (!timings.empty())
		{
			ImGui::Text("Step size: 1 -> 1024");
			ImGui::PlotLines("##timings", timings.data(), (int)timings.size(),
				0, "ms per step", 0.0f,
				*std::max_element(timings.begin(), timings.end()) * 1.2f,
				ImVec2(300, 150));

			int stepsize = 1;
			for (float t : timings)
			{
				ImGui::Text("Step %4d: %.1f us", stepsize, t);
				stepsize *= 2;
			}
		}
	}
	ImGui::End();
}

struct Transform
{
	float matrix[16] =
	{ 1, 0, 0, 0,
	  0, 1, 0, 0,
	  0, 0, 1, 0,
	  0, 0, 0, 1 };
};

class GameObject3D
{
public:
	Transform local;
	int id;
};

class GameObject3DAlt
{
public:
	Transform* local;
	int id;
};

void dae::Renderer::DrawExercise2() const
{
	/////////////////////////////// Exercise 2 
	ImGui::SetNextWindowPos(ImVec2(300, 50), ImGuiCond_Once);
	if (ImGui::Begin("Exercise 2"))
	{
		static int nSamples1{ 100 };
		static std::vector<float> timings{};

		ImGui::InputInt("# samples", &nSamples1);

		if (ImGui::Button("Thrash the cache with GameObject3D"))
		{
			std::vector<GameObject3D> myArr(1'000'000);
			timings.clear();

			for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
			{
				float total = 0;

				for (int sample = 0; sample < nSamples1; ++sample)
				{
					auto start = std::chrono::high_resolution_clock::now();
					for (size_t i = 0; i < myArr.size(); i += stepsize)
					{
						myArr[i].id *= 2;
					}
					auto end = std::chrono::high_resolution_clock::now();
					total += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				}

				timings.push_back((float)total / nSamples1);
			}
		}

		if (ImGui::Button("Thrash the cache with GameObject3DAlt"))
		{
			std::vector<GameObject3DAlt> myArr(1'000'000);
			timings.clear();

			for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
			{
				float total = 0;

				for (int sample = 0; sample < nSamples1; ++sample)
				{
					auto start = std::chrono::high_resolution_clock::now();
					for (size_t i = 0; i < myArr.size(); i += stepsize)
					{
						myArr[i].id *= 2;
					}
					auto end = std::chrono::high_resolution_clock::now();
					total += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				}

				timings.push_back((float)total / nSamples1);
			}
		}

		// draw the graph if we have data
		if (!timings.empty())
		{
			ImGui::Text("Step size: 1 -> 1024");
			ImGui::PlotLines("##timings", timings.data(), (int)timings.size(), 0, "ms per step", 0.0f, 
						     *std::max_element(timings.begin(), timings.end()) * 1.2f, ImVec2(300, 150));

			int stepsize = 1;
			for (float t : timings)
			{
				ImGui::Text("Step %4d: %.1f us", stepsize, t);
				stepsize *= 2;
			}
		}
	}
	ImGui::End();
}
